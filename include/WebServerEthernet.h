#ifndef WebServerEthernet_h
#define WebServerEthernet_h

#include <LwIP.h>
#include <STM32Ethernet.h>
#include <AsyncWebServer_STM32.h>
#include "AsyncWebSocket_STM32.h"
extern void WebServerInit();
extern void webServerUpdateLoop();
#endif