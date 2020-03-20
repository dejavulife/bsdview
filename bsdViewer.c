/*
 * Copyright (c) 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/touch.h>
#include <vita2d.h>

#include "bsdview.h"
extern int hellocpp(int x);

char str[MAXLEN]= {};
int key_exit = 0;
int key_L, key_R, key_start, key_select, key_up, key_down, key_left, key_right, key_A, key_E, key_O, key_X;
int lx, ly, rx, ry;
SceTouchData touch_old[SCE_TOUCH_PORT_MAX_NUM];
SceTouchData touch_press[SCE_TOUCH_PORT_MAX_NUM];
SceTouchData touch_release[SCE_TOUCH_PORT_MAX_NUM];
SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];

int touch_press_fx, touch_press_fy, touch_fx, touch_fy, touch_release_fx, touch_release_fy; 
int touch_press_bx, touch_press_by, touch_bx, touch_by, touch_release_bx, touch_release_by; 
int istouch_press_f = 0 , istouch_press_b = 0;
int need_update = 1;
int need_help = 1;
enum GAMESCREEN gameScreen = MENU;
char errorStr[MAXLEN];

SceCtrlData pad;
vita2d_pgf *pgf;

int font = 0;
time_t now = 0;


__attribute__((format(printf,1,2)))
int debug_log (char* format, ... ) {
    FILE *fp;
    fp = fopen("ux0:data/bsdview_debug.txt","a");
    if ( fp == NULL ) {
        //printf("open alife_debug.txt failed.\n");
        return 0;
    }
    char str_tmp[MAXLEN];
    int i=0;
    va_list vArgList;                           
    va_start (vArgList, format);                 
    i=vsnprintf(str_tmp, MAXLEN, format, vArgList);  
    va_end(vArgList);                            

    fprintf(fp,"%s", str_tmp);
    fclose(fp);
    return i;
}


int initAll() {
    /* to enable analog sampling */
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, 1);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
    pgf = vita2d_load_custom_pgf(FONTPGF);
    if (pgf == NULL) { 
        pgf = vita2d_load_default_pgf(); 
        font = 1;
    }

    gameScreen = MENU;

    memset(&pad, 0, sizeof(pad));
    need_update=1 ;
    return 0;
}

int clearKeys() {
    key_L = 0; 
    key_R = 0; 
    key_start = 0; 
    key_select = 0; 
    key_up = 0; 
    key_down = 0; 
    key_left = 0;
    key_right = 0; 
    key_A = 0; 
    key_E = 0; 
    key_O = 0; 
    key_X = 0;
    return 0;
}

int totalKeypress() {
    return 1;
    
}


int getinput() {
    clearKeys();
    memcpy(touch_old, touch, sizeof(touch_old)); 
    sceCtrlPeekBufferPositive(0, &pad, 1);
    sceTouchPeek(0, &touch[SCE_TOUCH_PORT_FRONT], 1);
    sceTouchPeek(1, &touch[SCE_TOUCH_PORT_BACK], 1);
    if ( istouch_press_f == 0) {
        if ( touch[SCE_TOUCH_PORT_FRONT].reportNum > 0 ) {
            memcpy(touch_press, touch, sizeof(touch_press));
            touch_press_fx = touch_press[SCE_TOUCH_PORT_FRONT].report[0].x;
            touch_press_fy = touch_press[SCE_TOUCH_PORT_FRONT].report[0].y;            
            istouch_press_f = 1;
        }
    } else if ( istouch_press_f == 1 ) {
        if ( touch[SCE_TOUCH_PORT_FRONT].reportNum <= 0 ) {
            memcpy(touch_release, touch_old, sizeof(touch_release));
            touch_release_fx = touch_release[SCE_TOUCH_PORT_FRONT].report[0].x;
            touch_release_fy = touch_release[SCE_TOUCH_PORT_FRONT].report[0].y;      
            istouch_press_f = 0;
        }
    }
    if ( istouch_press_b == 0) {
        if ( touch[SCE_TOUCH_PORT_BACK].reportNum > 0 ) {
            memcpy(touch_press, touch, sizeof(touch_press));
            touch_press_bx = touch_press[SCE_TOUCH_PORT_BACK].report[0].x;
            touch_press_by = touch_press[SCE_TOUCH_PORT_BACK].report[0].y;            
            istouch_press_b = 1;
        }
    } else if ( istouch_press_b == 1 ) {
        if ( touch[SCE_TOUCH_PORT_BACK].reportNum <= 0 ) {
            memcpy(touch_release, touch_old, sizeof(touch_release));
            touch_release_bx = touch_release[SCE_TOUCH_PORT_BACK].report[0].x;
            touch_release_by = touch_release[SCE_TOUCH_PORT_BACK].report[0].y;      
            istouch_press_b = 0;
        }
    }
    int i = 0;
    for(i = 0; i < SCE_TOUCH_MAX_REPORT; i++) {
        if ( i < touch[SCE_TOUCH_PORT_FRONT].reportNum ) {
            touch_fx = touch[SCE_TOUCH_PORT_FRONT].report[i].x;
            touch_fy = touch[SCE_TOUCH_PORT_FRONT].report[i].y;
        }
        if ( i < touch[SCE_TOUCH_PORT_BACK].reportNum ) {
            touch_bx = touch[SCE_TOUCH_PORT_BACK].report[i].x;
            touch_by = touch[SCE_TOUCH_PORT_BACK].report[i].y;
        }
    }
    if (pad.buttons & SCE_CTRL_START) key_start = 1;
    if (pad.buttons & SCE_CTRL_SELECT) key_select = 1;
    if (pad.buttons & SCE_CTRL_UP)  key_up = 1;
    if (pad.buttons & SCE_CTRL_DOWN)  key_down = 1;
    if (pad.buttons & SCE_CTRL_LEFT)  key_left = 1;
    if (pad.buttons & SCE_CTRL_RIGHT)  key_right = 1;
    if (pad.buttons & SCE_CTRL_TRIANGLE) key_A = 1;
    if (pad.buttons & SCE_CTRL_SQUARE) key_E = 1;
    if (pad.buttons & SCE_CTRL_CIRCLE)  key_O = 1;
    if (pad.buttons & SCE_CTRL_CROSS) key_X = 1;
    lx = pad.lx ;
    ly = pad.ly ;
    rx = pad.rx;
    ry = pad.ry;
    if (pad.buttons & SCE_CTRL_LTRIGGER) key_L = 1;
    if (pad.buttons & SCE_CTRL_RTRIGGER) key_R = 1;
    return 0;
}

int mainLogic() {
    if ( key_L == 1 && key_start == 1 ) {
        key_exit = 1;
    }
    if ( key_R == 1 ) {
        hellocpp(errorStr);
    }
    return 0;
}



int innerLogic() {
    int c = 0;
    c = totalKeypress() ;
    if ( c > 0 ) need_update = 1;
    switch(gameScreen) {
        case MENU:
            mainLogic();
            break;
    }
    return 0;
}

int showDebugInfo() {
    int key_step = 20, y = 520;
    int i= 1;
    if (strlen(errorStr) > 0) vita2d_pgf_draw_text(pgf, 20, 30,RGBA8(255,0,0,255),1.0f,errorStr);
    //left stick 
    sprintf(str, "[%03d,%03d]", lx, ly);
    vita2d_pgf_draw_text(pgf, key_step * i++, y -20, RGBA8(0,255,0,255), 1.0f, str);
    //key_L 
    sprintf(str, "L");
    if (key_L) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_left 
    sprintf(str, "<");
    if (key_left) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_up 
    sprintf(str, "^");
    if (key_up) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_down 
    sprintf(str, "v");
    if (key_down) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_right 
    sprintf(str, ">");
    if (key_right) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_E 
    sprintf(str, "E");
    if (key_E) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_A 
    sprintf(str, "A");
    if (key_A) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_X 
    sprintf(str, "X");
    if (key_X) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_O 
    sprintf(str, "O");
    if (key_O) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_R 
    sprintf(str, "R");
    if (key_R) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_select 
    sprintf(str, "s");
    if (key_select) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //key_start 
    sprintf(str, "S");
    if (key_start) 
    {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(255,0,0,255), 1.0f, str);
    } else {
        vita2d_pgf_draw_text(pgf, key_step * i++, y, RGBA8(0,255,0,255), 1.0f, str);
    }
    //right stick 
    sprintf(str, "[%03d,%03d]", rx, ry);
    vita2d_pgf_draw_text(pgf, key_step * i++, y - 20, RGBA8(0,255,0,255), 1.0f, str);

    sprintf(str, "FRONT:P[%04d,%04d]T[%04d,%04d]R[%04d,%04d]", touch_press_fx,touch_press_fy,touch_fx,touch_fy, touch_release_fx, touch_release_fy);
    vita2d_pgf_draw_text(pgf, 10, y - 60, RGBA8(0,255,0,255), 1.0f, str);
    sprintf(str, "BACK:P[%04d,%04d]T[%04d,%04d]R[%04d,%04d]", touch_press_bx,touch_press_by,touch_bx,touch_by, touch_release_bx, touch_release_by);
    vita2d_pgf_draw_text(pgf, 10, y - 40, RGBA8(0,0,255,255), 1.0f, str);
    return 0;
}

int mainview() {
 // cleaning screen.
    vita2d_start_drawing();
    vita2d_clear_screen();
    showDebugInfo();
    vita2d_end_drawing();
    vita2d_swap_buffers();
    return 0;
}



int doGraphic() {
    need_update = 0;
    switch(gameScreen) {
        case MENU:
            mainview();
            break;
    }
    return 0;
}


int graphicDestroy() {
    /*
     * vita2d_fini() waits until the GPU has finished rendering,
     * then we can free the assets freely.
     */
    vita2d_fini();
    //vita2d_free_texture(image);
    //if (bg!= NULL) vita2d_free_texture(bg);
    //vita2d_free_texture(image4);
    vita2d_free_pgf(pgf);
    return 0;

}

int main()
{
    initAll();
    while (1) {
        getinput();    
        // innerLogic update things, and if graphic needed to update, need_update is set to 1.
        innerLogic();    
        if (!need_update) continue;
        doGraphic(); 
        if (key_exit)
            break;
    }
    graphicDestroy();
    sceKernelExitProcess(0);
    return 0;
}
