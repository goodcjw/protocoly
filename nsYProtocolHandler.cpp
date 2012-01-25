#include "nsYProtocolHandler.h"
#include "nsNetUtil.h"
#include "nsIURL.h"
#include "nsNetCID.h"
#include "nsIClassInfoImpl.h"
#include "nsStandardURL.h"
#include "nsNDNChannel.h"

#include "mozilla/ModuleUtils.h"
#include "nsAutoPtr.h"

nsYProtocolHandler* gYHandler = nsnull;

NS_IMPL_CLASSINFO(nsYProtocolHandler, NULL, 0, NS_YHANDLER_CID)
NS_IMPL_ISUPPORTS2(nsYProtocolHandler,
                   nsIProtocolHandler,
                   nsIYProtocolHandler);

nsYProtocolHandler::nsYProtocolHandler() {
  gYHandler = this;
}

nsYProtocolHandler::~nsYProtocolHandler() {
  gYHandler = nsnull;
}

nsresult nsYProtocolHandler::Init() {
  nsresult rv;
  mIOService = do_GetIOService(&rv);
  if (NS_FAILED(rv))
    return rv;
  return NS_OK;
}

NS_IMETHODIMP nsYProtocolHandler::GetScheme(nsACString & result) {
  result.AssignLiteral("y");
  return NS_OK;
}

NS_IMETHODIMP
nsYProtocolHandler::GetDefaultPort(PRInt32 *result) {
  *result = 80;        // no port for res: URLs
  return NS_OK;
}

NS_IMETHODIMP nsYProtocolHandler::GetProtocolFlags(PRUint32 *result) {
  *result = URI_NORELATIVE | URI_NOAUTH | URI_LOADABLE_BY_ANYONE;
  return NS_OK;
}

NS_IMETHODIMP nsYProtocolHandler::AllowPort(PRInt32 port, const char * scheme, 
                                           bool *_retval NS_OUTPARAM) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP nsYProtocolHandler::NewURI(const nsACString & aSpec,
                                         const char * aCharset,
                                         nsIURI *aBaseURI,
                                         nsIURI * *result) {
  nsresult rv;
  NS_ASSERTION(!aBaseURI, "base url passed into finger protocol handler");
  nsStandardURL* url = new nsStandardURL();
  if (!url)
    return NS_ERROR_OUT_OF_MEMORY;
  NS_ADDREF(url);

  rv = url->Init(nsIStandardURL::URLTYPE_STANDARD, -1, aSpec, aCharset, aBaseURI);
  if (NS_SUCCEEDED(rv))
    rv = CallQueryInterface(url, result);
  NS_RELEASE(url);

  return rv;
}

NS_IMETHODIMP nsYProtocolHandler::NewChannel(nsIURI *aURI,
                                             nsIChannel * *result) {
  nsresult rv;
  //  nsCAString tag = aURI->spec.split(":")[1];
  /*
  nsCAutoString tag("http://twitter.com/goodcjw");
  nsCOMPtr<nsIURI> uri;
  rv = mIOService->NewURI(tag, nsnull, nsnull,
                          getter_AddRefs(uri));
  rv = mIOService->NewChannel(tag, nsnull, nsnull, result);
  return NS_OK;
  */
  nsRefPtr<nsNDNChannel> channel;
  channel = new nsNDNChannel(aURI);

  rv = channel->Init();
  if (NS_FAILED(rv)) {
    return rv;
  }
  channel.forget(result);
  return rv;
}

