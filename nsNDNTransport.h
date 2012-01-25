#ifndef nsNDNTransport_h__
#define nsNDNTransport_h__

#include "mozilla/Mutex.h"
#include "nsITransport.h"
#include "nsISocketTransport.h"


class nsNDNTransport : public nsITransport {
  typedef mozilla::Mutex Mutex;

public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSITRANSPORT
  NS_DECL_NSISOCKETTRANSPORT

  nsNDNTransport();

  // this method instructs the NDN transport to open a transport of a
  // given type(s) to the given name
  nsresult Init(const char **socketTypes, PRUint32 typeCount);

protected:
  virtual ~nsNDNTransport();
};

#endif // nsNDNTransport_h__
