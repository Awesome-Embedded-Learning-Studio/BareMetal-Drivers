#pragma once

#define CFBD_LOG_AS_UART // AS UART
#define CFBD_LOG_BUFFER_SZ (128)

#if CFBD_LOG_BUFFER_SZ <= 0
#error "Making CFBD_LOG_BUFFER_SZ less then 1 is never good :("
#endif
