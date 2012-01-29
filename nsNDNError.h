#ifndef nsNDNError_h__
#define nsNDNError_h__

#include "nsError.h"

#define NS_ERROR_MODULE_NDN 217

#define NS_ERROR_CCND_UNAVAIL                           \
  NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_NDN, 1)
#define NS_ERROR_CCND_INVALID_NAME                      \
  NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_NDN, 2)
#define NS_ERROR_CCND_STREAM_UNAVAIL                    \
  NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_NDN, 3)
#define NS_ERROR_CCND_UNKNOWN_FAILURE                   \
  NS_ERROR_GENERATE_FAILURE(NS_ERROR_MODULE_NDN, 4)

nsresult ErrorAccordingToCCND(int errorCode);

#endif // nsNDNError_h__
