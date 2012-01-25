#ifndef nsNDNInputStream_h__
#define nsNDNInputStream_h__

#include "nsIAsyncInputStream.h"
#include "nsIEventTarget.h"
#include "nsCOMPtr.h"

class nsNDNInputStream : public nsIAsyncInputStream {
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIINPUTSTREAM
  NS_DECL_NSIASYNCINPUTSTREAM

  nsNDNInputStream(bool nonBlocking)
    : mStatus(NS_OK)
    , mNonBlocking(nonBlocking) {
  }

protected:
  virtual ~nsNDNInputStream() {}

private:
  // Called from the base stream's AsyncWait method when a pending callback
  // is installed on the stream.
  void OnCallbackPending();

private:
  nsCOMPtr<nsIInputStreamCallback> mCallback;
  nsCOMPtr<nsIEventTarget>         mCallbackTarget;
  nsresult                         mStatus;
  bool                             mNonBlocking;
};

#endif // nsNDNInputStream_h__
