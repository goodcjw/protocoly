#include "nsNDNTransportService.h"

using namespace mozilla;

nsNDNTransportService *gNDNTransportService = nsnull;
PRThread              *gNDNThread           = nsnull;

NS_IMPL_ISUPPORTS2(nsNDNTransportService,
                   nsIEventTarget,
                   nsIRunnable)

//-----------------------------------------------------------------------------
// nsIEVentTarget Methods

nsNDNTransportService::nsNDNTransportService()
    : mLock("nsNDNTransportService.mLock")
    , mInitialized(false)
    , mShuttingDown(false) {
}

nsNDNTransportService::~nsNDNTransportService() {
}

NS_IMETHODIMP
nsNDNTransportService::Dispatch(nsIRunnable *event, PRUint32 flags) {
  nsCOMPtr<nsIThread> thread = GetThreadSafely();
  NS_ENSURE_TRUE(thread, NS_ERROR_NOT_INITIALIZED);
  nsresult rv = thread->Dispatch(event, flags);
  if (rv == NS_ERROR_UNEXPECTED) {
    // Thread is no longer accepting events. We must have just shut it
    // down on the main thread. Pretend we never saw it.
    rv = NS_ERROR_NOT_INITIALIZED;
  }
  return rv;
}

NS_IMETHODIMP
nsNDNTransportService::IsOnCurrentThread(bool *result) {
  nsCOMPtr<nsIThread> thread = GetThreadSafely();
  NS_ENSURE_TRUE(thread, NS_ERROR_NOT_INITIALIZED);
  return thread->IsOnCurrentThread(result);
}

NS_IMETHODIMP
nsNDNTransportService::Run() {
  gNDNThread = PR_GetCurrentThread();

  nsIThread *thread = NS_GetCurrentThread();

  for (;;) {
    bool pendingEvents = false;
    thread->HasPendingEvents(&pendingEvents);

    do {
      // If there are pending events for this thread then
      // DoPollIteration() should service the network without blocking.
      //DoPollIteration(!pendingEvents);
            
      // If nothing was pending before the poll, it might be now
      if (!pendingEvents)
        thread->HasPendingEvents(&pendingEvents);

      if (pendingEvents) {
        NS_ProcessNextEvent(thread);
        pendingEvents = false;
        thread->HasPendingEvents(&pendingEvents);
      }
    } while (pendingEvents);

    // now that our event queue is empty, check to see if we should exit
    {
      MutexAutoLock lock(mLock);
      if (mShuttingDown)
        break;
    } 
  }
}

//-----------------------------------------------------------------------------
// public Methods
NS_IMETHODIMP
nsNDNTransportService::Init() {
  if (!NS_IsMainThread()) {
    NS_ERROR("wrong thread");
    return NS_ERROR_UNEXPECTED;
  }

  if (mInitialized)
    return NS_OK;

  if (mShuttingDown)
    return NS_ERROR_UNEXPECTED;

  //  nsCOMPtr<nsIThread> thread;
  //  if (NS_FAILED(rv)) return rv;
    
  nsresult rv;
  {
    MutexAutoLock lock(mLock);
    // Install our mThread, protecting against concurrent readers
    rv = NS_NewThread(getter_AddRefs(mThread), this);
    if (NS_FAILED(rv)) return rv;
  }

  mInitialized = true;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNTransportService::Shutdown() {
  // called from the Main thread
  NS_ENSURE_STATE(NS_IsMainThread());
  if (!mInitialized)
    return NS_OK;

  if (mShuttingDown)
    return NS_ERROR_UNEXPECTED;
  
  {
    MutexAutoLock lock(mLock);

    // signal the socket thread to shutdown
    mShuttingDown = true;

  }

  // join with thread
  mThread->Shutdown();
  {
    MutexAutoLock lock(mLock);
    // Drop our reference to mThread and make sure that any concurrent
    // readers are excluded
    mThread = nsnull;
  }

  mInitialized = false;
  mShuttingDown = false;

  return NS_OK;
}

//-----------------------------------------------------------------------------
// private Methods

already_AddRefed<nsIThread>
nsNDNTransportService::GetThreadSafely() {
    MutexAutoLock lock(mLock);
    nsIThread* result = mThread;
    NS_IF_ADDREF(result);
    return result;
}
