#ifndef nsNDNTransportService_h__
#define nsNDNTransportService_h__

#include "nsIEventTarget.h"
#include "nsIRunnable.h"

class nsNDNTransportService : public nsIEventTarget,
                              public nsIRunnable {
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIEVENTTARGET
  NS_DECL_NSIRUNNABLE

  nsNDNTransportService();
  virtual ~nsNDNTransportService();
};

#endif // nsNDNTransportService_h__
