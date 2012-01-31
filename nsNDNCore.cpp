#include "nsNDNCore.h"
#include "nsNDNChannel.h"
#include "nsNDNTransport.h"

#include "nsIOService.h"
#include "nsIURL.h"
#include "nsStreamUtils.h"

// nsBaseContentStream::nsISupports

NS_IMPL_THREADSAFE_ADDREF(nsNDNCore)
NS_IMPL_THREADSAFE_RELEASE(nsNDNCore)

// We only support nsIAsyncInputStream when we are in non-blocking mode.
NS_INTERFACE_MAP_BEGIN(nsNDNCore)
  NS_INTERFACE_MAP_ENTRY(nsIInputStream)
  NS_INTERFACE_MAP_ENTRY(nsIInputStreamCallback)
  NS_INTERFACE_MAP_ENTRY_CONDITIONAL(nsIAsyncInputStream, mNonBlocking)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIInputStream)
NS_INTERFACE_MAP_END_THREADSAFE;

nsNDNCore::nsNDNCore()
    : mChannel(nsnull)
    , mDataTransport(nsnull)
    , mDataStream(nsnull)
    , mState(NDN_INIT)
    , mStatus(NS_OK)
    , mNonBlocking(true)
    , mCallback(nsnull)
    , mCallbackTarget(nsnull) {
}

nsNDNCore::~nsNDNCore() {
}

nsresult
nsNDNCore::Init(nsNDNChannel *channel) {
  mChannel = channel;

  //  nsCOMPtr<nsIURL> url, URL should be parsed here
  //  URI can be access by mChannel->URI()

  nsresult rv;
  nsCOMPtr<nsIURL> url = do_QueryInterface(mChannel->URI());
  rv = url->GetAsciiSpec(mInterest);
  // XXX temopry walkaround when nsNDNURL is absent
  mInterest.Trim("y:", true, false, false);
  if (NS_FAILED(rv))
    return rv;

  return NS_OK;
}

//-----------------------------------------------------------------------------

void
nsNDNCore::DispatchCallback(bool async)
{
  if (!mCallback)
    return;

  // It's important to clear mCallback and mCallbackTarget up-front because the
  // OnInputStreamReady implementation may call our AsyncWait method.
  nsCOMPtr<nsIInputStreamCallback> callback;
  if (async) {
    NS_NewInputStreamReadyEvent(getter_AddRefs(callback), mCallback,
                                mCallbackTarget);
    if (!callback)
      return;  // out of memory!
    mCallback = nsnull;
  }
  else {
    callback.swap(mCallback);
  }
  mCallbackTarget = nsnull;

  callback->OnInputStreamReady(this);
}

void
nsNDNCore::OnCallbackPending() {
    // If this is the first call, then see if we could use the cache.  If we
    // aren't going to read from (or write to) the cache, then just proceed to
    // connect to the server.
    if (mState == NDN_INIT) {
      mState = Connect();
    }
    if (mState == NDN_CONNECT && mDataStream) {
      mDataStream->AsyncWait(this, 0, 0, CallbackTarget());
    }
}


NDN_STATE
nsNDNCore::Connect() {
  // create the NDN transport
  nsCOMPtr<nsNDNTransport> ntrans;
  nsresult rv;
  rv = CreateTransport(getter_AddRefs(ntrans));
  if (NS_FAILED(rv))
    return NDN_ERROR;
  mDataTransport = ntrans;
  // we are reading from the ndn
  nsCOMPtr<nsIInputStream> input;
  rv = mDataTransport->OpenInputStream(0,
                                       nsIOService::gDefaultSegmentSize,
                                       nsIOService::gDefaultSegmentCount,
                                       getter_AddRefs(input));
  NS_ENSURE_SUCCESS(rv, NDN_ERROR);
  mDataStream = do_QueryInterface(input);
  return NDN_CONNECT;
}

NS_IMETHODIMP
nsNDNCore::CreateTransport(nsNDNTransport **result) {
  nsNDNTransport *ntrans = new nsNDNTransport();
  if (!ntrans)
    return NS_ERROR_OUT_OF_MEMORY;
  NS_ADDREF(ntrans);

  nsresult rv = ntrans->Init(mInterest.get());
  if (NS_FAILED(rv)) {
    NS_RELEASE(ntrans);
    return rv;
  }

  *result = ntrans;
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsIInputStream Methods

NS_IMETHODIMP
nsNDNCore::Close() {
  return IsClosed() ? NS_OK : CloseWithStatus(NS_BASE_STREAM_CLOSED);
}

NS_IMETHODIMP
nsNDNCore::Available(PRUint32 *avail) {
  *avail = 0;
  return mStatus;
}

NS_IMETHODIMP
nsNDNCore::Read(char *buf, PRUint32 count, PRUint32 *countRead) {
  return ReadSegments(NS_CopySegmentToBuffer, buf, count, countRead);
}

NS_IMETHODIMP
nsNDNCore::ReadSegments(nsWriteSegmentFun writer, void *closure,
                        PRUint32 count, PRUint32 *countRead) {
  *countRead = 0;

  if (mStatus == NS_BASE_STREAM_CLOSED)
    return NS_OK;

  // No data yet
  if (!IsClosed() && IsNonBlocking())
    return NS_BASE_STREAM_WOULD_BLOCK;

  return mStatus;
}

NS_IMETHODIMP
nsNDNCore::IsNonBlocking(bool *nonblocking) {
  *nonblocking = true;
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsIAsyncInputStream Methods

NS_IMETHODIMP
nsNDNCore::CloseWithStatus(nsresult reason) {
  if (IsClosed())
    return NS_OK;

  NS_ENSURE_ARG(NS_FAILED(reason));
  mStatus = reason;

  DispatchCallbackAsync();
  return NS_OK;
}

NS_IMETHODIMP
nsNDNCore::AsyncWait(nsIInputStreamCallback *callback,
                     PRUint32 flags,
                     PRUint32 amount,
                     nsIEventTarget *target) {
  // Our _only_ consumer is nsInputStreamPump, so we simplify things here by
  // making assumptions about how we will be called.
  mCallback = callback;
  mCallbackTarget = target;

  if (!mCallback)
    return NS_OK;

  if (IsClosed()) {
    DispatchCallbackAsync();
    return NS_OK;
  }

  OnCallbackPending();
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsIInputStreamCallback Methods

NS_IMETHODIMP
nsNDNCore::OnInputStreamReady(nsIAsyncInputStream *aInStream) {
    // We are receiving a notification from our data stream, so just forward it
    // on to our stream callback.
    if (HasPendingCallback())
        DispatchCallbackSync();

    return NS_OK;
}

