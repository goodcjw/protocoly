#ifndef nsNDNCore_h__
#define nsNDNCore_h__

#include "nsString.h"
#include "nsCOMPtr.h"
#include "nsAutoPtr.h"
#include "nsIAsyncInputStream.h"
#include "nsIEventTarget.h"
#include "nsITransport.h"


class nsNDNChannel;
class nsNDNTransport;

typedef enum _NDN_STATE {
  NDN_INIT,
  NDN_CONNECT,
  NDN_ERROR,
  NDN_COMPLETE

} NDN_STATE;

class nsNDNCore : public nsIAsyncInputStream,
                  public nsIInputStreamCallback {
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIINPUTSTREAMCALLBACK
  NS_DECL_NSIINPUTSTREAM
  NS_DECL_NSIASYNCINPUTSTREAM

  nsNDNCore();

  nsresult Status() { return mStatus; }
  bool IsNonBlocking() { return mNonBlocking; }
  bool IsClosed() { return NS_FAILED(mStatus); }

  // Called to test if the stream has a pending callback.
  bool HasPendingCallback() { return mCallback != nsnull; }

  // The current dispatch target (may be null) for the pending callback if any.
  nsIEventTarget *CallbackTarget() { return mCallbackTarget; }

  nsresult Init(nsNDNChannel *channel);
  void DispatchCallback(bool async);
  void DispatchCallbackAsync() { DispatchCallback(true); }
  void DispatchCallbackSync() { DispatchCallback(false); }

protected:
  virtual ~nsNDNCore();

private:
  // Called from the base stream(nsNDNCore)'s AsyncWait method when a pending
  // callback is installed on the stream.
  void OnCallbackPending();
  NDN_STATE Connect();
  NS_IMETHODIMP CreateTransport(nsNDNTransport **result);

private:
  nsRefPtr<nsNDNChannel>              mChannel;
  nsCString                           mInterest;
  nsCOMPtr<nsITransport>              mDataTransport;
  nsCOMPtr<nsIAsyncInputStream>       mDataStream;
  NDN_STATE                           mState;
  nsresult                            mStatus;
  bool                                mNonBlocking;
  nsCOMPtr<nsIInputStreamCallback>    mCallback;
  nsCOMPtr<nsIEventTarget>            mCallbackTarget;
};

#endif // nsNDNCore_h__
