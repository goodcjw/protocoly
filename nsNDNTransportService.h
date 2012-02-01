#ifndef nsNDNTransportService_h__
#define nsNDNTransportService_h__

#include "nsIEventTarget.h"

class nsNDNTransportService : public nsIEventTarget {
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIEVENTTARGET

  nsNDNTransportService();

private:
  virtual ~nsNDNTransportService();
};

#endif // nsNDNTransportService_h__
