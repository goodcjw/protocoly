#ifndef nsNDNCore_h__
#define nsNDNCore_h__

#include "nsCOMPtr.h"
#include "nsAutoPtr.h"
#include "nsIAsyncInputStream.h"
#include "nsIEventTarget.h"
#include "nsITransport.h"

class nsNDNChannel;

class nsNDNCore : public nsIAsyncInputStream {
public:
  NS_DECL_ISUPPORTS
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
  void DispatchCallback();

protected:
  virtual ~nsNDNCore();

private:
  nsRefPtr<nsNDNChannel>              mChannel;
  nsCOMPtr<nsITransport>              mDataTransport;
  nsCOMPtr<nsIAsyncInputStream>       mDataStream;
  nsresult                            mStatus;
  bool                                mNonBlocking;
  nsCOMPtr<nsIInputStreamCallback>    mCallback;
  nsCOMPtr<nsIEventTarget>            mCallbackTarget;
};

#endif // nsNDNCore_h__
