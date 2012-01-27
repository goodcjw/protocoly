#ifndef nsNDNTransport_h__
#define nsNDNTransport_h__

#include "nsNDNInputStream.h"

#include "mozilla/Mutex.h"
#include "nsITransport.h"
#include "nsISocketTransport.h"

#include <ccn/ccn.h>
#include <ccn/charbuf.h>
#include <ccn/uri.h>
#include <ccn/fetch.h>

class nsNDNTransport : public nsITransport {
  typedef mozilla::Mutex Mutex;

public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSITRANSPORT
  NS_DECL_NSISOCKETTRANSPORT

  nsNDNTransport();
  virtual ~nsNDNTransport();

  // this method instructs the NDN transport to open a transport of a
  // given type(s) to the given name
  nsresult Init(const char **socketTypes, PRUint32 typeCount);

private:

  //
  // mNDN access methods: called with mLock held.
  //
  struct ndn *GetNDN_Locked();
  void        ReleaseNDN_Locked(struct ndn *fd);

private:

  Mutex              mLock;
  struct ndn        *mNDN;
  nsrefcnt           mNDNref;         // mNDN is closed when mFDref goes to zero
  bool               mNDNonline;

  nsNDNInputStream   mInput;

  friend class nsNDNInputStream;
};

#endif // nsNDNTransport_h__
