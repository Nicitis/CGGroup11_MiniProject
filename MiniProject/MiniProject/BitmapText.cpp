#include "BitmapText.h"

HDC hDC;                     // Private GDI Device Context
HGLRC hRC;                   // Permanent Rendering Context
HWND hWnd;                   // Holds Our Window Handle
HINSTANCE hInstance;                // Holds The Instance Of The Application

GLuint  base;                           // Base Display List For The Font Set
GLfloat cnt1;                           // 1st Counter Used To Move Text & For Coloring
GLfloat cnt2;                           // 2nd Counter Used To Move Text & For Coloring

/*-------------------------------------------------*
 * BuildFontWithEnum()                             *
 *      조건에 맞는 폰트를 생성한다.               *
 *      fontEnum, fontSize: 새로 생성할 폰트 정보  *
 *        fontEnum은 폰트 종류, fontSize는 폰트    *
 *        크기를 의미한다.                         *
 *-------------------------------------------------*/
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

/*-------------------------------------------------*
 * BuildFont()                                     *
 *      조건에 맞게 새로운 폰트를 생성한다.        *
 *      fontName, fontSize: 새로 빌드할 폰트 정보  *
 *        fontName은 폰트의 이름, fontSize는 폰트  *
 *        크기를 의미한다.                         *
 *-------------------------------------------------*/
GLvoid BuildFont(const char* fontName, int fontSize)
{
    HFONT   font;                       // Windows Font ID
    HFONT   oldfont;                    // Used For Good House Keeping

    base = glGenLists(96);                  // Storage For 96 Characters

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

/*-------------------------------------------------*
 * KillFont()                                      *
 *      폰트를 위한 디스플레이 리스트를 삭제한다.  *
 *-------------------------------------------------*/
GLvoid KillFont(GLvoid)                     // Delete The Font List
{
    glDeleteLists(base, 96);                // Delete All 96 Characters
}

/*-------------------------------------------------*
 * glPrint()                                       *
 *      문자열을 비트맵 그래픽으로 출력한다.       *
 *      fmt: 가변길이 문자열, 출력할 문자열이다.   *
 *-------------------------------------------------*/
GLvoid glPrint(const char* fmt, ...)                // Custom GL "Print" Routine
{
    char        text[256];              // Holds Our String
    va_list     ap;                 // Pointer To List Of Arguments

    if (fmt == NULL)                    // If There's No Text
        return;                     // Do Nothing

    va_start(ap, fmt);                  // Parses The String For Variables
        vsprintf(text, fmt, ap);                // And Converts Symbols To Actual Numbers
    va_end(ap);                     // Results Are Stored In Text

    glPushAttrib(GL_LIST_BIT);              // Pushes The Display List Bits
    glListBase(base - 32);                  // Sets The Base Character to 32

    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // Draws The Display List Text
    glPopAttrib();                      // Pops The Display List Bits
}

/*-------------------------------------------------*
 * InitFont()                                      *
 *      폰트 생성에 필요한 정보를 초기화하고       *
 *      기본 폰트인 ARIAL을 생성한다.              *
 *-------------------------------------------------*/
int InitFont(GLvoid)                      // All Setup For OpenGL Goes Here
{
    glShadeModel(GL_SMOOTH);                // Enable Smooth Shading

    hDC = wglGetCurrentDC();            // 현재 openGL 윈도우의 hDC를 가져온다.

    BuildFontWithEnum(ARIAL, DEFAULTFONTSIZE);       // Build The Font

    return TRUE;                        // Initialization Went OK
}
