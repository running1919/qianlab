#ifndef _WIFI_
#define _WIFI_

#define GS2011 1
#define RAK411 2

#ifndef WIFI_TYPE
#define WIFI_TYPE GS2011
#endif

#if (WIFI_TYPE == GS2011)
#include "./gs2011/API/GS_API.h"
#include "./gs2011/APP/GS_App.h"
#endif

#if (WIFI_TYPE == RAK411)
#endif

#endif /* #ifndef _WIFI_ */

