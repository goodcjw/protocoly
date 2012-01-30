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

  bool IsNonBlocking() { return true; }

  nsresult Init(nsNDNChannel *channel);

protected:
  virtual ~nsNDNCore();

private:
  nsRefPtr<nsNDNChannel>         mChannel;
  nsCOMPtr<nsITransport>         mDataTransport;
  nsCOMPtr<nsIAsyncInputStream>  mDataStream;

  bool                           mNonBlocking;
};

#endif // nsNDNCore_h__
