#ifndef nsNDNInputStream_h__
#define nsNDNInputStream_h__

#include "nsIAsyncInputStream.h"
#include "nsIEventTarget.h"
#include "nsCOMPtr.h"

class nsNDNTransport;

class nsNDNInputStream : public nsIAsyncInputStream {
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIINPUTSTREAM
  NS_DECL_NSIASYNCINPUTSTREAM

  nsNDNInputStream(nsNDNTransport *);
  virtual ~nsNDNInputStream();

  // called by the ndn transport on the ndn thread ??
  void OnNDNReady(nsresult condition);

private:
  nsNDNTransport                     *mTransport;
  nsrefcnt                            mReaderRefCnt;

  // access to these is protected by mTransport->mLock
  nsresult                            mCondition;
  nsCOMPtr<nsIInputStreamCallback>    mCallback;
  PRUint32                            mCallbackFlags;

  //    nsrefcnt                         mReaderRefCnt;
  //    PRUint64                         mByteCount;
};

#endif // nsNDNInputStream_h__
