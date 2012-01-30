#include "nsNDNCore.h"
#include "nsNDNChannel.h"

// nsBaseContentStream::nsISupports

NS_IMPL_THREADSAFE_ADDREF(nsNDNCore)
NS_IMPL_THREADSAFE_RELEASE(nsNDNCore)

// We only support nsIAsyncInputStream when we are in non-blocking mode.
NS_INTERFACE_MAP_BEGIN(nsNDNCore)
  NS_INTERFACE_MAP_ENTRY(nsIInputStream)
  NS_INTERFACE_MAP_ENTRY_CONDITIONAL(nsIAsyncInputStream, mNonBlocking)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIInputStream)
NS_INTERFACE_MAP_END_THREADSAFE;

nsNDNCore::nsNDNCore()
    : mChannel(nsnull)
    , mDataTransport(nsnull)
    , mDataStream(nsnull) {
}

nsNDNCore::~nsNDNCore() {
}

nsresult
nsNDNCore::Init(nsNDNChannel *channel) {
  mChannel = channel;

  //  nsresult rv;
  //  nsCOMPtr<nsIURL> url, URL should be parsed here
  //  URI can be access by mChannel->URI()
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsIInputStream Methods

NS_IMETHODIMP
nsNDNCore::Close() {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNCore::Available(PRUint32 *avail) {
  // currently not being used
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNCore::Read(char *buf, PRUint32 count, PRUint32 *countRead) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNCore::ReadSegments(nsWriteSegmentFun writer, void *closure,
                        PRUint32 count, PRUint32 *countRead) {
  // NDN stream is unbuffered
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNCore::IsNonBlocking(bool *nonblocking) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

//-----------------------------------------------------------------------------
// nsIAsyncInputStream Methods

NS_IMETHODIMP
nsNDNCore::CloseWithStatus(nsresult reason) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNCore::AsyncWait(nsIInputStreamCallback *callback,
                     PRUint32 flags,
                     PRUint32 amount,
                     nsIEventTarget *target) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

