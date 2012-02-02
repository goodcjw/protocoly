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
    , mReaderRefCnt(0)
    , mByteCount(0)
    , mCondition(NS_OK)
    , mCallbackFlags(0) {
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

  // ccn_fetch_read is non-blocking, so we have to block it manually
  // however, the bright side is that we don't need to move ccn_fetch_open
  // here. It must be non-blocking as well.
  //  res = ccn_fetch_read(ccnfs, buf, count);

  nsresult rv;
  {
    MutexAutoLock lock(mTransport->mLock);
    while ((res = ccn_fetch_read(ccnfs, buf, count)) != 0) {
      if (res > 0) {
        *countRead += res;
      } else if (res == CCN_FETCH_READ_NONE) {
        if (ccn_run(mTransport->mNDN, 1000) < 0) {
          res = CCN_FETCH_READ_NONE;
        }
      } else if (res == CCN_FETCH_READ_TIMEOUT) {
        ccn_reset_timeout(ccnfs);
        if (ccn_run(mTransport->mNDN, 1000) < 0) {
          res = CCN_FETCH_READ_NONE;
          break;
        }
      } else {
        // CCN_FETCH_READ_NONE
        // CCN_FETCH_READ_END
        // and other errors
        break;
      }
    }
    mTransport->ReleaseNDN_Locked(ccnfs);
  }

  mCondition = ErrorAccordingToCCND(res);
  mByteCount = *countRead;
  rv = mCondition;
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

