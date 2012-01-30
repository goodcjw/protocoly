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
  nsresult Init(const char *ndnName, PRUint32 typeCount);

private:

  void NDN_Close();
  void NDN_MakeTemplate(int allow_stale);
  //
  // mNDN access methods: called with mLock held.
  //
  struct ccn_fetch_stream *GetNDN_Locked();
  void ReleaseNDN_Locked(struct ccn_fetch_stream* ccnfs);

private:

  Mutex                      mLock;
  // connector to ccnd
  struct ccn                *mNDN;
  struct ccn_fetch          *mNDNfetch;
  struct ccn_charbuf        *mNDNname;
  struct ccn_charbuf        *mNDNtmpl;
  struct ccn_fetch_stream   *mNDNstream;

  nsrefcnt                   mNDNref;   // mNDN is closed when mFDref goes to zero
  bool                       mNDNonline;

  nsNDNInputStream           mInput;

  friend class nsNDNInputStream;
};

#endif // nsNDNTransport_h__
