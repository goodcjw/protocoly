#include "nsNDNChannel.h"
#include "nsILoadGroup.h"
#include "nsIURL.h"
#include <ccn/ccn.h>

NS_IMPL_ISUPPORTS4(nsNDNChannel,
                   nsIChannel,
                   nsIRequest,
                   nsIStreamListener,
                   nsIRequestObserver);

nsNDNChannel::nsNDNChannel(nsIURI *aURI) {
  SetURI(aURI);
}

nsNDNChannel::~nsNDNChannel() {
}

nsresult nsNDNChannel::Init() {
  //  nsresult rv;
  return NS_OK;
}

nsresult
nsNDNChannel::BeginPumpingData() {
  nsresult rv;
  nsCOMPtr<nsIInputStream> stream;
  nsCOMPtr<nsIChannel> channel;

  rv = OpenContentStream(true, getter_AddRefs(stream),
                         getter_AddRefs(channel));

  if (NS_FAILED(rv))
    return rv;

  NS_ASSERTION(!stream || !channel, "Got both a channel and a stream?");

  /*
  if (channel) {
      rv = NS_DispatchToCurrentThread(new RedirectRunnable(this, channel));
      if (NS_SUCCEEDED(rv))
          mWaitingOnAsyncRedirect = true;
      return rv;
  }
  */

  rv = nsInputStreamPump::Create(getter_AddRefs(mPump), stream, -1, -1, 0, 0,
                                 true);
  if (NS_SUCCEEDED(rv))
    rv = mPump->AsyncRead(this, nsnull);

  return rv;
}

nsresult
nsNDNChannel::OpenContentStream(bool async, nsIInputStream **stream,
                                nsIChannel** channel) {
  // 
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsNDNChannel::nsIRequest

NS_IMETHODIMP
nsNDNChannel::GetName(nsACString &result)
{
  if (!mURI) {
    result.Truncate();
    return NS_OK;
  }
  return mURI->GetSpec(result);
}

NS_IMETHODIMP
nsNDNChannel::IsPending(bool *result) {
  //  NS_NOTREACHED("nsNDNChannel::IsPending");
  //  return NS_ERROR_NOT_IMPLEMENTED;
  *result = IsPending();
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::GetStatus(nsresult *status) {
  *status = mStatus;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::Cancel(nsresult status) {
  NS_ASSERTION(NS_FAILED(status), "shouldn't cancel with a success code");
  nsresult rv = NS_ERROR_FAILURE;

  mStatus = status;
  if (mTransport) {
    rv = mTransport->Cancel(status);
  }
  return rv;
}

NS_IMETHODIMP
nsNDNChannel::Suspend(void)
{
  NS_NOTREACHED("nsNDNChannel::Suspend");
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::Resume(void)
{
  NS_NOTREACHED("nsNDNChannel::Resume");
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::GetLoadFlags(nsLoadFlags *aLoadFlags)
{
  *aLoadFlags = mLoadFlags;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::SetLoadFlags(nsLoadFlags aLoadFlags)
{
  mLoadFlags = aLoadFlags;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::GetLoadGroup(nsILoadGroup* *aLoadGroup) {
  *aLoadGroup = mLoadGroup;
  NS_IF_ADDREF(*aLoadGroup);
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::SetLoadGroup(nsILoadGroup* aLoadGroup) {
  mLoadGroup = aLoadGroup;
  CallbacksChanged();
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsNDNChannel::nsIChannel

NS_IMETHODIMP
nsNDNChannel::GetOriginalURI(nsIURI **aURI) {
  *aURI = OriginalURI();
  NS_ADDREF(*aURI);
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::SetOriginalURI(nsIURI *aURI) {
  NS_ENSURE_ARG_POINTER(aURI);
  mOriginalURI = aURI;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::GetURI(nsIURI **aURI) {
  NS_IF_ADDREF(*aURI = mURI);
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::GetOwner(nsISupports **aOwner) {
  NS_IF_ADDREF(*aOwner = mOwner);
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::SetOwner(nsISupports *aOwner) {
  mOwner = aOwner;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::GetNotificationCallbacks(nsIInterfaceRequestor **aCallbacks) {
  NS_IF_ADDREF(*aCallbacks = mCallbacks);
  return NS_OK;
}

NS_IMETHODIMP
nsNDNChannel::SetNotificationCallbacks(nsIInterfaceRequestor *aCallbacks) {
  mCallbacks = aCallbacks;
  CallbacksChanged();
  return NS_OK;
}

NS_IMETHODIMP 
nsNDNChannel::GetSecurityInfo(nsISupports **aSecurityInfo) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::GetContentType(nsACString &aContentType) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::SetContentType(const nsACString &aContentType) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::GetContentCharset(nsACString &aContentCharset) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::SetContentCharset(const nsACString &aContentCharset) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::GetContentLength(PRInt32 *aContentLength)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::SetContentLength(PRInt32 aContentLength)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::Open(nsIInputStream **result) {
  // OpenInputStream in old API?
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::AsyncOpen(nsIStreamListener *listener, nsISupports *ctxt) {
  // AsyncRead in old API?
  //  return NS_ERROR_NOT_IMPLEMENTED;
  mListener = listener;
  mListenerContext = ctxt;
}

NS_IMETHODIMP
nsNDNChannel::GetContentDisposition(PRUint32 *aContentDisposition)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::GetContentDispositionFilename(nsAString &aContentDispositionFilename)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::GetContentDispositionHeader(nsACString &aContentDispositionHeader)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

//-----------------------------------------------------------------------------
// nsNDNChannel::nsIStreamListener

NS_IMETHODIMP
nsNDNChannel::OnDataAvailable(nsIRequest *request, nsISupports *ctxt,
                               nsIInputStream *stream, PRUint32 offset,
                               PRUint32 count) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

/*
NS_IMETHODIMP
nsNDNChannel::OnRedirectVerifyCallback(nsresult result) {
  return NS_ERROR_NOT_IMPLEMENTED;
}
*/

//-----------------------------------------------------------------------------
// nsNDNChannel::nsIRequestObserver

NS_IMETHODIMP
nsNDNChannel::OnStartRequest(nsIRequest *request, nsISupports *ctxt) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNChannel::OnStopRequest(nsIRequest *request, nsISupports *ctxt,
                            nsresult status) {
  return NS_ERROR_NOT_IMPLEMENTED;
}
