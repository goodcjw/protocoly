#ifndef nsYHandler_h___
#define nsYHandler_h___

#include "nsIYProtocolHandler.h"
#include "nsIIOService.h"
#include "nsCOMPtr.h"

class nsYProtocolHandler : public nsIYProtocolHandler {
public:
  nsYProtocolHandler();
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPROTOCOLHANDLER
  NS_DECL_NSIYPROTOCOLHANDLER

  nsresult Init();
  //  static NS_METHOD Create(nsISupports* aOuter, const nsIID& aIID, void* *aResult);
  virtual ~nsYProtocolHandler();

private:
  nsCOMPtr<nsIIOService> mIOService;
};

#endif /* nsYHandler_h___ */
