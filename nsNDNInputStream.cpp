#include "mozilla/Mutex.h"

#include "nsNDNInputStream.h"
#include "nsNDNTransport.h"

using namespace mozilla;

NS_IMPL_QUERY_INTERFACE2(nsNDNInputStream,
                         nsIInputStream,
                         nsIAsyncInputStream);

nsNDNInputStream::nsNDNInputStream(nsNDNTransport* trans)
    : mTransport(trans)
    , mReaderRefCnt(0) {
}

nsNDNInputStream::~nsNDNInputStream() {
}

void
nsNDNInputStream::OnNDNReady(nsresult condition) {
    // not impletmented yet
}


//-----------------------------------------------------------------------------
// nsISupports Methods

NS_IMETHODIMP_(nsrefcnt)
nsNDNInputStream::AddRef()
{
    NS_AtomicIncrementRefcnt(mReaderRefCnt);
    return mTransport->AddRef();
}

NS_IMETHODIMP_(nsrefcnt)
nsNDNInputStream::Release()
{
    if (NS_AtomicDecrementRefcnt(mReaderRefCnt) == 0)
        Close();
    return mTransport->Release();
}

//-----------------------------------------------------------------------------
// nsIInputStream Methods

NS_IMETHODIMP
nsNDNInputStream::Close() {
  return CloseWithStatus(NS_BASE_STREAM_CLOSED);
}

NS_IMETHODIMP
nsNDNInputStream::Available(PRUint32 *avail) {
  // currently not being used
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNInputStream::Read(char *buf, PRUint32 count, PRUint32 *countRead) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNInputStream::ReadSegments(nsWriteSegmentFun writer, void *closure,
                               PRUint32 count, PRUint32 *countRead) {
  // NDN stream is unbuffered
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNInputStream::IsNonBlocking(bool *nonblocking) {
  *nonblocking = true;
  return NS_OK;
}

//-----------------------------------------------------------------------------
// nsIInputStream Methods

NS_IMETHODIMP
nsNDNInputStream::CloseWithStatus(nsresult reason) {
  // minic nsSocketInputStream::CloseWithStatus

  // may be called from any thread
  nsresult rv;
  {
    MutexAutoLock lock(mTransport->mLock);

    if (NS_SUCCEEDED(mCondition))
      rv = mCondition = reason;
    else
      rv = NS_OK;
  }
  //  if (NS_FAILED(rv))
  //    mTransport->OnInputClosed(rv);
  return NS_OK;
}

NS_IMETHODIMP
nsNDNInputStream::AsyncWait(nsIInputStreamCallback *callback,
                            PRUint32 flags,
                            PRUint32 amount,
                            nsIEventTarget *target) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

