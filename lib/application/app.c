#include "app.h"
#include "cfbd_define.h"
#include "ah_no.h"

#include <stddef.h>


static CFBDApplication _app;

static inline void _private_init_app(){
    CFBDBootTuple* pBootTuple = CFBD_AppBootMaker();
    _app.bootargs = pBootTuple ? pBootTuple->args : NULL;
    _app.selfBootFunc = pBootTuple ? pBootTuple->boot_func : NULL;
    _app.tick_provider = CFBD_AppClockProvider();
    if(!_app.tick_provider){
        CFBD_AH_NO();
    }
}

CFBDApplication* getApp(CFBD_Bool request_autoboot){
    _private_init_app();

    if(request_autoboot){
        _app.selfBootFunc(_app.bootargs);
    }

    return &_app;
}