#pragma once
#include<Windows.h>

#define delimiter "\n-------------------------------------------\n"
CONST CHAR g_sz_CLASS_NAME[] = "MyCalc";

CONST CHAR* g_sz_OPERATIONS[] = { "+", "-", "*", "/" };
CONST CHAR* g_sz_EDIT[] = { "<-", "C", "=" };
CONST CHAR* g_sz_BUTTON_FILENAMES[] = { "point","plus","minus","aster","slash","bsp","clr", "equal" };

//g_i_ - Global Integer
CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_INTERVAL = 1;
CONST INT g_i_BUTTON_SPACE = g_i_BUTTON_SIZE + g_i_INTERVAL;
//CONST INT g_i_BUTTON_SPACE_DOUBLE = (g_i_BUTTON_SIZE + g_i_INTERVAL) * 2;

CONST INT g_i_BUTTON_SIZE_DOUBLE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_DISPLAY_HEIGHT = 48;
CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL;

CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + 2 * g_i_START_X + 16;
CONST INT g_i_WINDOW_HEIGHT = (g_i_DISPLAY_HEIGHT + g_i_INTERVAL) + g_i_BUTTON_SPACE * 4 + 2 * g_i_START_Y + 24 + 16;

CONST INT g_SIZE = 256;

CONST COLORREF g_WINDOW_BACKGROUND[] = { RGB(0,0, 200),RGB(200,200,200) };
CONST COLORREF g_DISPLAY_BACKGROUND[] = { RGB(0,0, 100), RGB(100,100,100) };
CONST COLORREF g_DISPLAY_FOREGROUND[] = { RGB(255,0,000), RGB(0,255,0) };

CONST CHAR* g_sz_SKIN[] = { "square_blue", "metal_mistral" };

CONST CHAR* g_sz_FONT[] = {  "Digital-7 Mono", "Tristan DEMO", "Astronaut" };
