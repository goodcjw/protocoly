#include "nsNDNError.h"
#include "nsNetError.h"

#include <ccn/fetch.h>

nsresult
ErrorAccordingToCCND(int errorCode) {
    nsresult rv = NS_ERROR_FAILURE;
    switch (errorCode) {
      case CCN_FETCH_READ_NONE:
        rv = NS_ERROR_CONNECTION_REFUSED;
        break;
      case CCN_FETCH_READ_TIMEOUT:
        rv = NS_ERROR_NET_TIMEOUT;
        break;
      default:
        rv = NS_ERROR_CCND_UNKNOWN_FAILURE;
        break;
    }
    return rv;
}
