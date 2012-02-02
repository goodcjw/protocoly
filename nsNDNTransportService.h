#ifndef nsNDNTransportService_h__
#define nsNDNTransportService_h__

#include "nsIEventTarget.h"
#include "nsIRunnable.h"
#include "nsThreadUtils.h"
#include "mozilla/Mutex.h"

class nsNDNTransportService : public nsIEventTarget,
                              public nsIRunnable {
  typedef mozilla::Mutex Mutex;

public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIEVENTTARGET
  NS_DECL_NSIRUNNABLE

  nsNDNTransportService();
  virtual ~nsNDNTransportService();
  NS_IMETHODIMP Init();
  NS_IMETHODIMP Shutdown();

private:

  already_AddRefed<nsIThread> GetThreadSafely();

  nsCOMPtr<nsIThread>        mThread;
  Mutex                      mLock;
  bool                       mInitialized;
  bool                       mShuttingDown;
};

extern nsNDNTransportService *gNDNTransportService;

#endif // nsNDNTransportService_h__
