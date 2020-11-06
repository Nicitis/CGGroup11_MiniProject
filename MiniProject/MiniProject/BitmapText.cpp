#include "BitmapText.h"

// #include <windows.h>                      // Header File For Windows
// #include <stdio.h>                        // Header File For Standard Input/Output    ( ADD )
// #include <stdarg.h>                       // Header File For Variable Argument Routines   ( ADD )
// #include <gl\gl.h>                        // Header File For The OpenGL32 Library
// #include <gl\glu.h>                       // Header File For The GLu32 Library

HDC hDC;                     // Private GDI Device Context
HGLRC hRC;                   // Permanent Rendering Context
HWND hWnd;                   // Holds Our Window Handle
HINSTANCE hInstance;                // Holds The Instance Of The Application

GLuint  base;                           // Base Display List For The Font Set
GLfloat cnt1;                           // 1st Counter Used To Move Text & For Coloring
GLfloat cnt2;                           // 2nd Counter Used To Move Text & For Coloring

GLvoid BuildFontWithEnum(FONT fontEnum, int fontSize)
{
    switch (fontEnum)
    {
    case ARIAL:
        BuildFont("Arial", fontSize);
        break;
    case COURIERNEW:
        BuildFont("Courier New", fontSize);
        break;
    case CALIBRI:
        BuildFont("Calibri", fontSize);
        break;
    case TIMESNEWROMAN:
        BuildFont("Times New Roman", fontSize);
        break;
    case BOOKANTIQUA:
        BuildFont("Book Antiqua", fontSize);
        break;
    case TREBUCHETMS:
        BuildFont("Book Antiqua", fontSize);
        break;
    }
}

/** 새로운 폰트를 생성하는 함수입니다.
 *  char* fontName: 폰트 이름
 *  int fontSize: 폰트 크기
 */
GLvoid BuildFont(const char* fontName, int fontSize)
{
    HFONT   font;                       // Windows Font ID
    HFONT   oldfont;                    // Used For Good House Keeping

    base = glGenLists(96);                  // Storage For 96 Characters ( NEW )

    font = CreateFont(-fontSize, // Height Of Fonts
        0,              // Width Of Font
        0,              // Angle Of Escapement
        0,              // Orientation Angle
        FW_BOLD,        // Font Weight
        FALSE,          // Italic
        FALSE,          // Underline
        FALSE,          // Strikeout (취소선)
        ANSI_CHARSET, // Character Set Identifier
        OUT_TT_PRECIS,  // Output Precision
        CLIP_DEFAULT_PRECIS,        // Clipping Precision
        ANTIALIASED_QUALITY,        // Output Quality
        FF_DONTCARE | DEFAULT_PITCH,  // Family And Pitch
        fontName);         // Font Name

    oldfont = (HFONT)SelectObject(hDC, font);       // Selects The Font We Want
    wglUseFontBitmaps(hDC, 32, 96, base);           // Builds 96 Characters Starting At Character 32
    SelectObject(hDC, oldfont);             // Selects The Font We Want
    DeleteObject(font);                 // Delete The Font
}

GLvoid KillFont(GLvoid)                     // Delete The Font List
{
    glDeleteLists(base, 96);                // Delete All 96 Characters
}

GLvoid glPrint(const char* fmt, ...)                // Custom GL "Print" Routine
{
    char        text[256];              // Holds Our String
    va_list     ap;                 // Pointer To List Of Arguments

    if (fmt == NULL)                    // If There's No Text
        return;                     // Do Nothing

    va_start(ap, fmt);                  // Parses The String For Variables
        vsprintf(text, fmt, ap);                // And Converts Symbols To Actual Numbers
    va_end(ap);                     // Results Are Stored In Text

    glPushAttrib(GL_LIST_BIT);              // Pushes The Display List Bits     ( NEW )
    glListBase(base - 32);                  // Sets The Base Character to 32    ( NEW )

    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // Draws The Display List Text  ( NEW )
    glPopAttrib();                      // Pops The Display List Bits   ( NEW )
}

int InitFont(GLvoid)                      // All Setup For OpenGL Goes Here
{
    glShadeModel(GL_SMOOTH);                // Enable Smooth Shading
    // glClearColor(0.0f, 0.0f, 0.0f, 0.5f);           // Black Background
    // glClearDepth(1.0f);                 // Depth Buffer Setup
    // glEnable(GL_DEPTH_TEST);                // Enables Depth Testing
    // glDepthFunc(GL_LEQUAL);                 // The Type Of Depth Testing To Do
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations

    hDC = wglGetCurrentDC();            // 현재 openGL 윈도우의 hDC를 가져온다.

    BuildFontWithEnum(ARIAL, DEFAULTFONTSIZE);       // Build The Font

    return TRUE;                        // Initialization Went OK
}

/*
GLvoid KillGLWindow(GLvoid)                         // Properly Kill The Window
{
    if (fullscreen)
    {
        ChangeDisplaySetting(NULL, 0);

    }
}
*/