#include "mozilla/Mutex.h"
#include "nsStreamUtils.h"

#include "nsNDNInputStream.h"
#include "nsNDNTransport.h"
#include "nsNDNError.h"

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
  int res;

  //  return NS_ERROR_NOT_IMPLEMENTED;
  *countRead = 0;

  struct ccn_fetch_stream *ccnfs;
  {
    MutexAutoLock lock(mTransport->mLock);

    // TODO: how this works?
    if (NS_FAILED(mCondition))
      return (mCondition == NS_BASE_STREAM_CLOSED) ? NS_OK : mCondition;

    ccnfs = mTransport->GetNDN_Locked();
    if (!ccnfs)
      return NS_BASE_STREAM_WOULD_BLOCK;
  }

  // Actually reading process
  if (NULL == ccnfs)
    return NS_ERROR_CCND_STREAM_UNAVAIL;

  res = ccn_fetch_read(ccnfs, buf, count);
 
  nsresult rv;
  {
    MutexAutoLock lock(mTransport->mLock);
    mTransport->ReleaseNDN_Locked(ccnfs);

    if (res > 0) {
      mByteCount += (*countRead = res);
    } else if (res == CCN_FETCH_READ_END) {
      // CCN_FETCH_READ_END is 0
      // Do nothing
    } else {
      // res < 0
      // possible errors
      //    CCN_FETCH_READ_NONE
      //    CCN_FETCH_READ_TIMEOUT
      mCondition = ErrorAccordingToCCND(res);
    }
    rv = mCondition;
  }
  return rv;
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
// nsIAsyncInputStream Methods

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
  // copied from nsSocketInputStream::AsyncWait
  nsCOMPtr<nsIInputStreamCallback> directCallback;
  {
    MutexAutoLock lock(mTransport->mLock);

    if (callback && target) {
      nsCOMPtr<nsIInputStreamCallback> temp;
      nsresult rv = NS_NewInputStreamReadyEvent(getter_AddRefs(temp),
                                                callback, target);
      if (NS_FAILED(rv)) return rv;
      mCallback = temp;
    }
    else {
      mCallback = callback;
    }
    
    if (NS_FAILED(mCondition))
      directCallback.swap(mCallback);
    else
      mCallbackFlags = flags;
  }
  if (directCallback)
    directCallback->OnInputStreamReady(this);
  else
    return NS_ERROR_NOT_IMPLEMENTED;

  return NS_OK;
}

