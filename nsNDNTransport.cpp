#include "nsNetSegmentUtils.h"
#include "nsStreamUtils.h"

#include "nsIPipe.h"

#include "nsNDNTransport.h"
#include "nsNDNError.h"

NS_IMPL_ISUPPORTS1(nsNDNTransport,
                   nsITransport);

nsNDNTransport::nsNDNTransport()
    : mLock("nsNDNTransport.mLock"),
      mNDN(nsnull),
      mNDNfetch(nsnull),
      mNDNname(nsnull),
      mNDNtmpl(nsnull),
      mNDNstream(nsnull),
      mNDNref(0),
      mNDNonline(false),
      mInputClosed(true),
      mInput(this) {
}

nsNDNTransport::~nsNDNTransport() {
}

nsresult 
nsNDNTransport::Init(const char *ndnName) {
  // the current implementation only allows one ccn name
  int res;

  // create ccn connection
  mNDN = ccn_create();
  res = ccn_connect(mNDN, NULL);
  if (res < 0) {
    ccn_destroy(&mNDN);
    return NS_ERROR_CCND_UNAVAIL;
  }

  // create name buffer
  mNDNname = ccn_charbuf_create();
  mNDNname->length = 0;
  res = ccn_name_from_uri(mNDNname, ndnName);
  if (res < 0) {
    ccn_destroy(&mNDN);
    ccn_charbuf_destroy(&mNDNname);
    return NS_ERROR_CCND_INVALID_NAME;
  }

  // initialize ccn fetch
  mNDNfetch = ccn_fetch_new(mNDN);

  // initialize interest template (mNDNtmpl)
  NDN_MakeTemplate(0);

  // initialize ccn stream
  // XXX size of buffer is hard coded here, which must be wrong
  // copied from ccnwget
  // maxBufs = 4
  // assumeFixed = 0
  mNDNstream = ccn_fetch_open(mNDNfetch, mNDNname, ndnName, 
                              mNDNtmpl, 4, CCN_V_HIGHEST, 0);

  mNDNonline = true;
}

NS_IMETHODIMP
nsNDNTransport::OpenInputStream(PRUint32 flags,
                                PRUint32 segsize,
                                PRUint32 segcount,
                                nsIInputStream **result) {
  nsresult rv;
  nsCOMPtr<nsIAsyncInputStream> pipeIn;

  if (!(flags & OPEN_UNBUFFERED) || (flags & OPEN_BLOCKING)) {
    bool openBlocking = (flags & OPEN_BLOCKING);

    net_ResolveSegmentParams(segsize, segcount);
    nsIMemory *segalloc = net_GetSegmentAlloc(segsize);

    // create a pipe
    nsCOMPtr<nsIAsyncOutputStream> pipeOut;
    rv = NS_NewPipe2(getter_AddRefs(pipeIn), getter_AddRefs(pipeOut),
                     !openBlocking, true, segsize, segcount, segalloc);
    if (NS_FAILED(rv)) return rv;
    //    rv = NS_AsyncCopy(&mInput, pipeOut, ???


    *result = pipeIn;

  } else {
    *result = &mInput;
  }

  mInputClosed = false;

  // Leave out SocketService's event handling
  NS_ADDREF(*result);
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
  if (NS_SUCCEEDED(reason))
    reason = NS_BASE_STREAM_CLOSED;

  mInput.CloseWithStatus(reason);
  mInputClosed = true;
  return NS_OK;
}

NS_IMETHODIMP
nsNDNTransport::SetEventSink(nsITransportEventSink *sink,
                             nsIEventTarget *target) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

void 
nsNDNTransport::NDN_MakeTemplate(int allow_stale) {
  mNDNtmpl = ccn_charbuf_create();
  ccn_charbuf_append_tt(mNDNtmpl, CCN_DTAG_Interest, CCN_DTAG);
  ccn_charbuf_append_tt(mNDNtmpl, CCN_DTAG_Name, CCN_DTAG);
  ccn_charbuf_append_closer(mNDNtmpl); /* </Name> */
  // XXX - use pubid if possible
  ccn_charbuf_append_tt(mNDNtmpl, CCN_DTAG_MaxSuffixComponents, CCN_DTAG);
  ccnb_append_number(mNDNtmpl, 1);
  ccn_charbuf_append_closer(mNDNtmpl); /* </MaxSuffixComponents> */
  if (allow_stale) {
    ccn_charbuf_append_tt(mNDNtmpl, CCN_DTAG_AnswerOriginKind, CCN_DTAG);
    ccnb_append_number(mNDNtmpl, CCN_AOK_DEFAULT | CCN_AOK_STALE);
    ccn_charbuf_append_closer(mNDNtmpl); /* </AnswerOriginKind> */
  }
  ccn_charbuf_append_closer(mNDNtmpl); /* </Interest> */
}

void
nsNDNTransport::NDN_Close() {
  // XXX what if mNDNstream has not been initialized yet?
  mNDNstream = ccn_fetch_close(mNDNstream);
  mNDNfetch = ccn_fetch_destroy(mNDNfetch);
  ccn_destroy(&mNDN);
  ccn_charbuf_destroy(&mNDNname);
  ccn_charbuf_destroy(&mNDNtmpl);

  // TODO put the 'mInputClosed' into the right place
  mInputClosed = true;
}

struct ccn_fetch_stream*
nsNDNTransport::GetNDN_Locked() {
  // mNDN is not available to the streams while it's not oneline
  if (!mNDNonline)
    return nsnull;

  if (mNDNstream)
    mNDNref++;

  return mNDNstream;
}

void
nsNDNTransport::ReleaseNDN_Locked(struct ccn_fetch_stream *ccnfs) {
  NS_ASSERTION(mNDNstream == ccnfs, "wrong ndn");

  if (--mNDNref == 0) {
    // close ndn here
    NDN_Close();
  }
}

