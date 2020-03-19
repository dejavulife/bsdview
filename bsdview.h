

#ifndef _BSDVIEW_H_
#define _BSDVIEW_H_
#include <time.h>
#include <psp2/types.h>
#include <psp2/kernel/threadmgr.h>

enum GAMESCREEN {
    MENU = 0,
    INFO ,
    PLAY ,
    JUMP ,
    SCANNING ,
    OPTION ,
};

int debug_log (char* format, ... );

#define TIMENOW (time(NULL))
#define MAXLEN 2048
#define SCREEN_WIDTH 960
#define SCREEN_HIGHT 544
#define SCREEN_HALF_WIDTH 480
#define SCREEN_HALF_HIGHT 272
#define FONTPGF "ux0:data/font.pgf"

#ifdef DEBUG
#   define DEBUG_LOG(fmt, args...) debug_log(fmt, ## args)
#else
#   define DEBUG_LOG(fmt, args...) 
#endif

#endif /*  _BSDVIEW_H_ */
