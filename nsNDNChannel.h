#ifndef nsNDNChannel_h__
#define nsNDNChannel_h__

//#include "nsStringAPI.h"
#include "nsString.h"
#include "nsIChannel.h"
#include "nsIInputStream.h"
#include "nsIIOService.h"
#include "nsCOMPtr.h"
#include "nsAutoPtr.h"

#include "nsInputStreamPump.h"
#include "nsIProgressEventSink.h"
#include "nsIInterfaceRequestor.h"
#include "nsIStreamListener.h"

#define NS_NDNCHANNEL_CLASSNAME                 \
  "nsNDNChannel"

/* cb5380c1-8767-4004-8c86-55f38013dbb2 */
#define NS_NDNCHANNEL_CID                               \
  { 0xcb5380c1, 0x8767, 0x4004,                         \
    {0x8c, 0x86, 0x55, 0xf3, 0x80, 0x13, 0xdb, 0xb2} }

//

class nsNDNChannel : public nsIChannel,
                     private nsIStreamListener {
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICHANNEL
  NS_DECL_NSIREQUEST
  //  NS_DECL_NSINDNCHANNEL

  nsNDNChannel(nsIURI *aURI);
  virtual ~nsNDNChannel();

  nsresult Init();

  nsIURI *URI() {
    return mURI;
  }
  void SetURI(nsIURI *uri) {
    mURI = uri;
    mOriginalURI = uri;
  }
  nsIURI *OriginalURI() {
    return mOriginalURI;
  }

  bool IsPending() const {
    // TODO
    return mPump;
  }

private:
  NS_DECL_NSISTREAMLISTENER
  NS_DECL_NSIREQUESTOBSERVER

  void CallbacksChanged() {
    mProgressSink = nsnull;
    mQueriedProgressSink = false;
    //    OnCallbacksChanged();
  }

  // Called to setup mPump and call AsyncRead on it.
  nsresult BeginPumpingData();

  nsresult OpenContentStream(bool async, nsIInputStream **stream,
                             nsIChannel** channel);

private:
  nsRefPtr<nsInputStreamPump>         mPump;

  nsCOMPtr<nsIURI>                    mOriginalURI;
  nsCOMPtr<nsIURI>                    mURI;

  nsCOMPtr<nsIInterfaceRequestor>     mCallbacks;
  nsCOMPtr<nsIProgressEventSink>      mProgressSink;
  nsCOMPtr<nsISupports>               mSecurityInfo;
  nsCOMPtr<nsISupports>               mOwner;
  nsCString                           mContentType;
  nsCString                           mContentCharset;
  nsCOMPtr<nsIIOService>              mIOService;
  nsCOMPtr<nsIChannel>                mTransport;
  nsresult                            mStatus;
  nsCOMPtr<nsILoadGroup>              mLoadGroup;
  PRUint32                            mLoadFlags;
  bool                                mQueriedProgressSink;
  bool                                mWaitingOnAsyncRedirect;

protected:
  nsCOMPtr<nsIStreamListener>         mListener;
  nsCOMPtr<nsISupports>               mListenerContext;
};

#endif /* nsNDNChannel_h__ */
