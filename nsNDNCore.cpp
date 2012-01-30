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
