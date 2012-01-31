#include "nsNDNCore.h"
#include "nsNDNChannel.h"

#include "nsStreamUtils.h"

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
    , mDataStream(nsnull)
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

  //  nsresult rv;
  //  nsCOMPtr<nsIURL> url, URL should be parsed here
  //  URI can be access by mChannel->URI()
  return NS_OK;
}

//-----------------------------------------------------------------------------

void
nsNDNCore::DispatchCallback()
{
  if (!mCallback)
    return;

  // It's important to clear mCallback and mCallbackTarget up-front because the
  // OnInputStreamReady implementation may call our AsyncWait method.
  nsCOMPtr<nsIInputStreamCallback> callback;
  NS_NewInputStreamReadyEvent(getter_AddRefs(callback), mCallback,
                              mCallbackTarget);
  if (!callback)
    return;  // out of memory!
  mCallback = nsnull;
  mCallbackTarget = nsnull;

  callback->OnInputStreamReady(this);
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

  DispatchCallback();
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
    DispatchCallback();
    return NS_OK;
  }

  //  OnCallbackPending();
  return NS_OK;
}

