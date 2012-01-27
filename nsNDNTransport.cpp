#include "nsNDNTransport.h"

NS_IMPL_ISUPPORTS1(nsNDNTransport,
                   nsITransport);

nsNDNTransport::nsNDNTransport()
    : mLock("nsNDNTransport.mLock"),
      mNDN(nsnull),
      mNDNref(0),
      mNDNonline(false),
      mInput(this) {
}

nsNDNTransport::~nsNDNTransport() {
}

nsresult Init(const char **socketTypes, PRUint32 typeCount) {
}
             
NS_IMETHODIMP
nsNDNTransport::OpenInputStream(PRUint32 flags,
                                PRUint32 segsize,
                                PRUint32 segcount,
                                nsIInputStream **result) {
  return NS_OK;
}

NS_IMETHODIMP
nsNDNTransport::OpenOutputStream(PRUint32 flags,
                                 PRUint32 segsize,
                                 PRUint32 segcount,
                                 nsIOutputStream **result) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::Close(nsresult reason) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::SetEventSink(nsITransportEventSink *sink,
                             nsIEventTarget *target) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetHost(nsACString &host) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetPort(PRInt32 *port) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetPeerAddr(PRNetAddr *addr) {
    // once we are in the connected state, mNetAddr will not change.
    // so if we can verify that we are in the connected state, then
    // we can freely access mNetAddr from any thread without being
    // inside a critical section.

  /*
    NS_ENSURE_TRUE(mState == STATE_TRANSFERRING, NS_ERROR_NOT_AVAILABLE);

    memcpy(addr, &mNetAddr, sizeof(mNetAddr));
  */
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetSelfAddr(PRNetAddr *addr) {
    // we must not call any PR methods on our file descriptor
    // while holding mLock since those methods might re-enter
    // socket transport code.
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetSecurityInfo(nsISupports **secinfo) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetSecurityCallbacks(nsIInterfaceRequestor **callbacks) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::SetSecurityCallbacks(nsIInterfaceRequestor *callbacks) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::IsAlive(bool *result) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetTimeout(PRUint32 type, PRUint32 *value) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::SetTimeout(PRUint32 type, PRUint32 value) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetConnectionFlags(PRUint32 *value) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::SetConnectionFlags(PRUint32 value) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::SetQoSBits(PRUint8 aQoSBits) {
  // Don't do any checking here of bits.  Why?  Because as of RFC-4594
  // several different Class Selector and Assured Forwarding values
  // have been defined, but that isn't to say more won't be added later.
  // In that case, any checking would be an impediment to interoperating
  // with newer QoS definitions.
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
nsNDNTransport::GetQoSBits(PRUint8 *aQoSBits) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

struct ndn*
nsNDNTransport::GetNDN_Locked() {
  // mNDN is not available to the streams while it's not oneline
  if (!mNDNonline)
    return nsnull;

  if (mNDN)
    mNDNref++;

  return mNDN;
}

void
nsNDNTransport::ReleaseNDN_Locked(struct ndn *fd) {
  NS_ASSERTION(mNDN == fd, "wrong ndn");

  if (--mNDNref == 0) {
    // close ndn here
  }
}

