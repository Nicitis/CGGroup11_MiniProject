#ifndef __BITMAP_TEXT_H__
#define __BITMAP_TEXT_H__
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>        // Header File For Windows
#include <math.h>           // Header File For Windows Math Library
#include <stdio.h>          // Header File For Standard Input/Output
#include <stdarg.h>         // Header File For Variable Argument Routines
#include <gl\gl.h>          // Header File For The OpenGL32 Library
#include <gl\glu.h>         // Header File For The GLu32 Library

#define DEFAULTFONTSIZE 20

enum FONT
{
    ARIAL = 1,      // Arial = 1
    COURIERNEW,     // Courier New = 2
    CALIBRI,        // Calibri = 3
    TIMESNEWROMAN,  // Times New Roman = 4
    BOOKANTIQUA,    // Book Antiqua = 5
    TREBUCHETMS     // Trebuchet MS = 6
};

GLvoid BuildFontWithEnum(FONT fontEnum, int fontSize);
GLvoid BuildFont(const char* fontName, int fontSize);
GLvoid KillFont(GLvoid);
GLvoid glPrint(const char* fmt, ...);
int InitFont(GLvoid);
int DrawGLScene(GLvoid);

#endif