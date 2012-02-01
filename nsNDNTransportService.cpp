#include "nsNDNTransportService.h"

NS_IMPL_ISUPPORTS2(nsNDNTransportService,
                   nsIEventTarget,
                   nsIRunnable)

//-----------------------------------------------------------------------------
// nsIEVentTarget Methods

nsNDNTransportService::nsNDNTransportService() {
}

nsNDNTransportService::~nsNDNTransportService() {
}

NS_IMETHODIMP
nsNDNTransportService::Dispatch(nsIRunnable *event, PRUint32 flags) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransportService::IsOnCurrentThread(bool *_retval NS_OUTPARAM) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransportService::Run() {
  return NS_ERROR_NOT_IMPLEMENTED;
}
