/* This program illustrates the use of the glut library for
interfacing with a window system */

#include <glut.h>
#include <stdlib.h>
#include "BitmapText.h"

/* 그리기 모드 (windef에 POINTS가 중복되어 이름을 변경) */
#define DRAW_NULL 0
#define DRAW_LINE 1
#define DRAW_RECTANGLE 2
#define DRAW_TRIANGLE  3
#define DRAW_POINTS 4
#define DRAW_TEXT 5
#define PEN 6
#define ERASER 7
#define SELECT 8
#define DRAW_CIRCLE 9

#define MAX_KEY 256             /* 최대 텍스트 길이 */
#define FONTDELTASIZE (int)5;   /* 폰트를 조절할 때 간격이 되는 값 */

void mouse(int, int, int, int);     /* 마우스 콜백 함수 */
void key(unsigned char, int, int);  /* 키 콜백 함수 */
void display(void);                 /* 디스플레이 콜백 함수 */
void drawSquare(int, int);          /* 사각형 */
void myReshape(GLsizei, GLsizei);   /* 윈도우 이동 및 크기 조절 콜백 함수 */

void myinit(void);                  /* 초기화 함수 */

void screen_box(int, int, int);     /* 메뉴 버튼 상자를 그린다 */
void right_menu(int);               /* 우클릭 메뉴 */
void middle_menu(int);              /* 휠클릭 메뉴 */
void color_menu(int);               /* 색상 메뉴 */
void pixel_menu(int);               /* 픽셀 크기 및 선 두께 메뉴 */
void fill_menu(int);                /* 채움 메뉴 */
void font_menu(int);                /* 글자 폰트 메뉴 */
void font_size_menu(int);           /* 글자 크기 메뉴 */
int pick(int, int);                 /* 위치에 따른 상태를 구한다 */

void motionFunc(int, int);          /* 마우스 이동 콜백 함수 */
bool isInTheRect(int, int, int, int, int, int);
void drawMenuButton(int, int, int, int); /* 현재 메뉴 상태에 따라 메뉴 버튼 상자의 색을 설정하는 함수 */
void menuHighlight(void);
void swap(int*, int*);              /* 두 요소를 바꾸는 함수 */
void copy(int, int);                /* 영역 복사하기 */
void paste(int, int);               /* 영역 붙여넣기 */
void draw_select_box(int, int, int, int); /* 선택 영역 상자를 그린다 */
void draw_circle(int, int, int, int); /* 원을 그린다 */

/* globals */
GLsizei wh = 500, ww = 500; /* initial window size */
GLfloat size = 3.0;   /* half side length of square */

//Adding New 20203044
GLfloat lineWidth = 2.0; // Line Size
int rubberband = 0;
int xp[2], yp[2];

int draw_mode = 0; /* drawing mode */
int rx, ry; /*raster position*/
int key_count = 0; /* a counter for text string */

// Adding a addtionally 20203044
GLfloat r = 0.0, g = 0.0, b = 0.0, a = 1.0;  /* drawing color */ 
int fill = 0; /* fill flag */

char key_buffer[MAX_KEY];           /* 현재 입력하는 텍스트 버퍼 */
FONT font = ARIAL;                  /* 현재 폰트 */
FONT built_font = ARIAL;            /* 현재 빌드된 폰트 */
int font_size = DEFAULTFONTSIZE;    /* 현재 글자 크기 */
int built_font_size = DEFAULTFONTSIZE;    /* 현재 빌드된 글자 크기 */

int is_selected = FALSE; /* 현재 영역이 선택되었는지 여부 */
int hold_left_btn = FALSE;
GLubyte* pixels = NULL; /* 현재 복사된 픽셀 */
GLsizei copy_width = 0;
GLsizei copy_height = 0;


void drawSquare(int x, int y)
{
    y=wh-y;
    glColor3ub( (char) rand()%256, (char) rand()%256, (char) rand()%256); 
    glBegin(GL_POLYGON);
    {
        glVertex2f(x + size, y + size);
        glVertex2f(x - size, y + size);
        glVertex2f(x - size, y - size);
        glVertex2f(x + size, y - size);
    }
    glEnd();
}


/* rehaping routine called whenever window is resized
or moved */
void myReshape(GLsizei w, GLsizei h)
{
    /* adjust clipping box */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    glEnable(GL_LINE_SMOOTH); // 안티앨리어싱 가능하게 처리
    glEnable(GL_POLYGON_SMOOTH);

    /* adjust viewport and  clear */
    glViewport(0,0,w,h);
    glClearColor (0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    display();
    glFlush();

    /* set global size for use by drawing routine */
    ww = w;
    wh = h; 
}

void myinit(void)
{
    glViewport(0,0,ww,wh);

    /* Pick 2D clipping window to match size of X window 
    This choice avoids having to scale object coordinates
    each time window is resized */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    glOrtho(0.0, (GLdouble) ww , 0.0, (GLdouble) wh , -1.0, 1.0);

    /* set clear color to black and clear window */

    glClearColor (0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}



void mouse(int btn, int state, int x, int y)
{
    static int count;
    int where;
    
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // 마우스 왼쪽 버튼을 눌렀을 때
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        where = pick(x, y);
        glColor3f(r, g, b);
        if (where != 0)
        {
            count = 0;
            draw_mode = where;

            if (is_selected) // 영역 선택을 취소한다.
            {
                is_selected = FALSE;
                draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]); // 선택 영역 상자를 지운다.
            }
        }
        else switch (draw_mode)
        {
            glLineWidth(lineWidth);

            case(DRAW_LINE):
                if (count == 0)
                {
                    count++;
                    xp[0] = x;
                    yp[0] = wh - y;
                    xp[1] = x;
                    yp[1] = wh - y;
                    rubberband = 1;

                    glEnable(GL_COLOR_LOGIC_OP);
                    glLogicOp(GL_XOR); // XOR로 선분을 그린다.

                    glBegin(GL_LINE);
                    glVertex2i(xp[0], yp[0]);
                    glVertex2i(xp[0], yp[0]);
                    glEnd();

                    glFlush();
                    glDisable(GL_COLOR_LOGIC_OP);
                }
                break;
            case(DRAW_RECTANGLE):
                if (count == 0)
                {
                    count++;
                    xp[0] = x;
                    yp[0] = wh - y;
                    xp[1] = x;
                    yp[1] = wh - y;
                    rubberband = 1;

                    glEnable(GL_COLOR_LOGIC_OP);
                    glLogicOp(GL_XOR); // XOR로 직사각형을 그린다.

                    glBegin(GL_LINE_LOOP); // 이전의 RubberBand_rect 지우기
                    glVertex2i(xp[1], yp[1]);
                    glVertex2i(xp[1], yp[0]);
                    glVertex2i(xp[0], yp[0]);
                    glVertex2i(xp[0], yp[1]);
                    glEnd();

                    glFlush();
                    glDisable(GL_COLOR_LOGIC_OP);
                }
                break;
            case (DRAW_TRIANGLE):
                switch (count)
                {
                    case(0):
                        count++;
                        xp[0] = x;
                        yp[0] = y;
                        break;
                    case(1):
                        count++;
                        xp[1] = x;
                        yp[1] = y;
                        break;
                    case(2):
                        glLineWidth(lineWidth);

                        if (fill) glBegin(GL_POLYGON);
                        else glBegin(GL_LINE_LOOP);
                        glVertex2i(xp[0], wh - yp[0]);
                        glVertex2i(xp[1], wh - yp[1]);
                        glVertex2i(x, wh - y);
                        glEnd();
                        draw_mode = 0;
                        count = 0;
                }
                break;
            case(DRAW_POINTS):
                // glLineWidth(4.0);
                drawSquare(x, y);
                count++;
                break;
            case(DRAW_TEXT): // 텍스트 기능
                if (strlen(key_buffer) > 0) // 이미 입력한 값이 있으면 출력한다.
                {
                    glRasterPos2i(rx, ry);
                    glPrint(key_buffer);
                }

                rx = x;
                ry = wh - y;
                glRasterPos2i(rx, ry);
                count = 0;

                // 현재 입력된 키 버퍼에 값이 존재하면 키 버퍼를 초기화하여 입력할 준비를 한다.
                if (key_count > 0)
                {
                    int i;
                    for (i = 0; i < key_count; i++) {
                        key_buffer[i] = '\0';
                    }
                    key_count = 0;
                }
                break;
            case(PEN):
            case(ERASER):
                xp[0] = x;
                yp[0] = y;
                break;
            case(SELECT): // 영역 선택 기능 (복사, 붙여넣기 전용)
                if (!is_selected)
                {
                    xp[0] = x;
                    if (y < ww / 10) // 메뉴 상자 영역을 침범할 수 없게 y 영역을 제한한다.
                        yp[0] = ww / 10;
                    else
                        yp[0] = y;
                    hold_left_btn = TRUE;
                }
                break;
            case(DRAW_CIRCLE): // 원 그리기 기능
                if (count == 0)
                {
                    if (!isInTheRect(x, wh - y, 0, wh - ww / 10, ww, 0))
                        break;
                    xp[0] = x;
                    yp[0] = y;
                    xp[1] = x;
                    yp[1] = y;

                    glEnable(GL_COLOR_LOGIC_OP);
                    glLogicOp(GL_XOR); // XOR로 원을 그린다.

                    if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
                        glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);
                    
                    draw_circle(xp[0], wh - yp[0], 0, fill);

                    glDisable(GL_COLOR_LOGIC_OP);

                    count++;
                    rubberband = 1; // 러버밴드를 시작한다.
                }
                break;
        }

        glPopAttrib();
        glFlush();
    }
    
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) // 마우스 왼쪽 버튼을 뗐을 때
    {
        where = pick(x, y);
        glLineWidth(lineWidth);

        switch (draw_mode)
        {
            case(DRAW_LINE):
                if (where == DRAW_NULL && count == 1)
                {
                    glColor3f(r, g, b);
                    glBegin(GL_LINES);
                    glVertex2i(x, wh - y);
                    glVertex2i(xp[0], yp[0]);
                    glEnd();
                    draw_mode = 0;
                    count = 0;
                    rubberband = 0;
                }
                break;
            case(DRAW_RECTANGLE):
                if (where == DRAW_NULL && count == 1)
                {
                    glColor3f(r, g, b);
                    if (fill)
                    {
                        glBegin(GL_POLYGON);
                        glVertex2i(x, wh - y);
                        glVertex2i(x, yp[0]);
                        glVertex2i(xp[0], yp[0]);
                        glVertex2i(xp[0], wh - y);
                        glEnd();
                    }
                    
                    glBegin(GL_LINE_LOOP);
                    glVertex2i(x, wh - y);
                    glVertex2i(x, yp[0]);
                    glVertex2i(xp[0], yp[0]);
                    glVertex2i(xp[0], wh - y);
                    glEnd();
                    draw_mode = 0;
                    count = 0;
                    rubberband = 0;
                }
                break;
            case(PEN):
            case(ERASER):
                xp[1] = x;
                yp[1] = y;
                break;
            case(SELECT): // 영역 선택 기능 (복사, 붙여넣기 전용)
                if (where != 0)
                    return;

                if (y < ww / 10) // 메뉴판을 선택할 수 없도록 y 위치를 제한한다.
                    y = ww / 10;

                // 선택할 영역이 존재하고, 현재 선택 영역이 없으면 선택한다.
                if (x != xp[0] && y != yp[0] && !is_selected)
                {
                    is_selected = TRUE;
                    xp[1] = x;
                    yp[1] = y;

                    if (xp[0] > xp[1])
                        swap(&xp[0], &xp[1]);
                    if (yp[0] > yp[1])
                        swap(&yp[0], &yp[1]);

                    // 선택 영역에 선택 사각형을 그린다.
                    draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
                }
                else if (is_selected) // 이미 선택한 상태면 영역을 해제한다.
                {
                    is_selected = FALSE;
                    // 기존 선택 영역을 지운다.
                    draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
                }
                hold_left_btn = FALSE;
                break;
            case (DRAW_CIRCLE): // 원 그리기 기능
                if (where == DRAW_NULL && count == 1)
                {
                    glColor3f(r, g, b);
                    int r = (int)sqrtf((float)((x - xp[0]) * (x - xp[0]) + (y - yp[0]) * (y - yp[0])));
                    // 처음 지점과 마지막 지점 사이의 거리를 구함
                    draw_circle(xp[0], wh - yp[0], r, fill);

                    draw_mode = 0;
                    count = 0;
                    rubberband = 0;                    
                }
                break;
        }
    }
    menuHighlight();
}

int pick(int x, int y)
{
    y = wh - y;
    if (y < wh - ww / 10) return 0;
    else if (x < ww / 10) return DRAW_LINE; // Line tool
    else if (x < ww / 5) return DRAW_RECTANGLE; // Rect tool
    else if (x < 3 * ww / 10) return DRAW_TRIANGLE; // Triangle Tool
    else if (x < 2 * ww / 5) return DRAW_POINTS; // Point Tool
    else if (x < ww / 2) return DRAW_TEXT; // Text Tool
    else if (x < 6 * ww / 10) return PEN; // Pen Tool
    else if (x < 7 * ww / 10) return ERASER; // Eraser Tool
    else if (x < 8 * ww / 10) return SELECT; // Select tool
    else if (x < 9 * ww / 10) return DRAW_CIRCLE; // Select tool

    else return 0;
}

void screen_box(int x, int y, int s )
{
    glBegin(GL_QUADS);
    {
        glVertex2i(x, y);
        glVertex2i(x + s, y);
        glVertex2i(x + s, y + s);
        glVertex2i(x, y + s);
    }
    glEnd();
}

void right_menu(int id)
{
   if(id == 1) exit(0);
   else display();
}

void middle_menu(int id)
{

}

void color_menu(int id)
{
	if (id == 1) 
	{
		r = 1.0; g = 0.0; b = 0.0;
	}

	else if (id == 2)
	{
		r = 0.0; g = 1.0; b = 0.0; 
	}

	else if (id == 3)
	{ 
		r = 0.0; g = 0.0; b = 1.0; 
	}

	else if(id == 4) 
	{ 
		r = 0.0; g = 1.0; b = 1.0;
	}

	else if (id == 5) 
	{ 
		r = 1.0; g = 0.0; b = 1.0; 
	}

	else if (id == 6)
	{
		r = 1.0; g = 1.0; b = 0.0; 
	}

	else if (id == 7) 
	{ 
		r = 1.0; g = 1.0; b = 1.0;
	}

	else if (id == 8) 
	{ 
		r = 0.0; g = 0.0; b = 0.0; 
	}
}
void pixel_menu(int id)
{
    if (id == 1) size = 2 * size;
    else if (size > 1) size = size/2;
    else if (id == 3) lineWidth = 2 * lineWidth;
    else if (id == 4) lineWidth = lineWidth / 2;
}

void fill_menu(int id)
{
    if (id == 1) fill = 1; 
    else fill = 0;
}

/*-------------------------------------------------*
 * font_menu()                                     *
 *      글자 폰트를 설정한다.                      *
 *      id: 메뉴 번호                          *
 *-------------------------------------------------*/
void font_menu(int id)
{
    // id: 1 ~ 6
    // FONT = arial, courier new, calibri, times new roman, book antiqua, trebuchet ms
    font = (FONT)id;

    // 이전 텍스트를 지운다.
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f, 1.0f, 1.0f); // XOR시 백색이 사라지는 현상을 방지한다.
        if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
            glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
        glRasterPos2i(rx, ry);
        glEnable(GL_COLOR_LOGIC_OP); // 이전 텍스트를 XOR 연산으로 지운다.
        glLogicOp(GL_XOR);
        glPrint(key_buffer);
    }

    built_font = font; 
    built_font_size = font_size;
    BuildFontWithEnum(font, font_size); // 변경된 상태로 폰트를 새로 빌드한다.

    // 현재 입력 중인 텍스트를 새 폰트로 다시 그린다.
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f, 1.0f, 1.0f); // XOR시 백색이 사라지는 현상을 방지한다.
        if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
            glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
        glRasterPos2i(rx, ry);
        glPrint(key_buffer);
        glFlush();
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

/*-------------------------------------------------*
 * font_menu()                                     *
 *      글자 크기를 설정한다.                      *
 *      id: 메뉴 번호                              *
 *-------------------------------------------------*/
void font_size_menu(int id)
{
    // 이전 텍스트를 지운다.
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f, 1.0f, 1.0f); // XOR시 백색이 사라지는 현상을 방지한다.
        if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
            glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
        glRasterPos2i(rx, ry);
        glEnable(GL_COLOR_LOGIC_OP); // 이전 텍스트를 XOR 연산으로 지운다.
        glLogicOp(GL_XOR);
        glPrint(key_buffer);
    }

    if (id == 1) // 글자 크기를 10만큼 올린다.
    {
        font_size += 2 * FONTDELTASIZE;
    }
    else if (id == 2) // 글자 크기를 5만큼 올린다.
    {
        font_size += FONTDELTASIZE;
    }
    else if (id == 3) // 글자 크기를 5만큼 감소시킨다.
    {
        font_size -= FONTDELTASIZE;
    }
    else // 글자 크기를 10만큼 감소시킨다.
    {
        font_size -= 2 * FONTDELTASIZE;
    }

    if (font_size <= 0)
    {
        font_size = 5;
    }

    // 변경된 크기로 폰트를 새로 빌드한다.
    built_font = font;
    built_font_size = font_size;
    BuildFontWithEnum(font, font_size);

    // 현재 입력 중인 텍스트를 새 폰트로 다시 그린다.
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f, 1.0f, 1.0f); // XOR시 백색이 사라지는 현상을 방지한다.
        if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
            glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
        glRasterPos2i(rx, ry);
        glPrint(key_buffer); // 텍스트를 그린다.
        glFlush();
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

/*-------------------------------------------------*
 * key()                                           *
 *      텍스트를 입력하거나 복사/붙여넣기 기능을   *
 *      실행한다.                                  *
 *      k: 입력된 키 값                            *
 *      xx: 현재 마우스 위치의 x좌표               *
 *      yy: 현재 마우스 위치의 y좌표               *
 *-------------------------------------------------*/
void key(unsigned char k, int xx, int yy)
{
    // 복사 및 붙여넣기 기능을 실행한다.
    if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
    {
        k += 96; // 입력을 ASCII 코드로 바꾼다.
        if (k == 'c' && draw_mode == SELECT && is_selected) // 복사 기능
        {
            is_selected = FALSE;
            draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
            copy(xx, yy);
        }
        else if (k == 'v' && pixels != NULL) // 붙여넣기 기능
        {
            if (is_selected) // 기존 영역을 지운다.
            {
                draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
            }

            is_selected = TRUE;

            // 현재 마우스를 좌상단으로 해서 영역을 설정한다.
            if (xx < 0) // x 위치를 윈도우 안으로 제한한다.
                xp[0] = 0;
            else
                xp[0] = xx;

            if (yy < ww / 10) // 메뉴판을 선택할 수 없도록 y 위치를 제한한다.
                yp[0] = ww / 10;
            else if (yy + copy_height > wh)
                yp[0] = wh - copy_height;
            else
                yp[0] = yy;

            xp[1] = xx + copy_width;
            yp[1] = yp[0] + copy_height;

            paste(xp[0], yp[0]); // 선택 영역에 붙여넣는다.

            // 영역 박스를 그린다.
            draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]); 
        }
        return;
    }

    // 텍스트 기능을 실행한다.
    if (draw_mode != DRAW_TEXT) return;
    if (k != 8 && k < 32) return; // 백스페이스(008)가 아닌 제어 키(000~031)를 제외한다.

    if (built_font != font || built_font_size != font_size) // 폰트가 바뀌었을 경우 다시 설정한다.
    {
        built_font = font;
        built_font_size = font_size;
        BuildFontWithEnum(font, font_size);
    }

    // 이전 텍스트를 지운다.
    glEnable(GL_COLOR_LOGIC_OP); // 이전 텍스트를 XOR 연산으로 지운다.
    glLogicOp(GL_XOR);
    glColor3f(1.0f, 1.0f, 1.0f); // XOR시 백색이 사라지는 현상을 방지한다.
    if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
        glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
    glRasterPos2i(rx, ry);
    glPrint(key_buffer);

    if (k == 8 && key_count > 0) // 백스페이스 바를 누르면 글자를 지운다.
    {
        key_count--;
        key_buffer[key_count] = '\0';
    }
    else if (k != 8 && key_count < MAX_KEY - 1) // 다른 키가 입력되면 키 버퍼에 입력한다.
    {
        key_buffer[key_count] = k;
        key_count++;
    }

    // 현재 입력 중인 텍스트를 다시 그린다.
    glRasterPos2i(rx, ry);
    glPrint(key_buffer); // 텍스트를 그린다.
    
    glFlush();
    glDisable(GL_COLOR_LOGIC_OP);
}

void swap(int* p1, int* p2) // 두 수를 교체한다.
{
    int temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

/*-------------------------------------------------*
 * copy()                                          *
 *      선택 영역을 복사한다.                      *
 *      x, y: 복사할 영역의 좌상단 좌표            *
 *-------------------------------------------------*/
void copy(int x, int y)
{
    int bytes_per_pixel = 4;
    int size;
    
    if (pixels) free(pixels);

    copy_width = xp[1] - xp[0];
    copy_height = yp[1] - yp[0];
    size = copy_width * copy_height * bytes_per_pixel;
    pixels = (GLubyte*)malloc(size * sizeof(GLubyte));

    int gl_x = xp[0];
    int gl_y = wh - yp[0] - copy_height;
    
    glReadPixels(gl_x, gl_y, copy_width, copy_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

/*-------------------------------------------------*
 * paste()                                         *
 *      선택 지점에 복사된 내용을 붙여넣는다.      *
 *      x, y: 붙여넣을 영역의 좌상단 좌표          *
 *-------------------------------------------------*/
void paste(int x, int y)
{
    int gl_x = x;
    int gl_y = wh - y - copy_height;

    // 메뉴 버튼 상자를 침범할 수 없도록 y 값 범위를 제한한다.
    if (gl_y > wh - copy_height - ww / 10) gl_y = wh - copy_height - ww / 10;

    glRasterPos2i(gl_x, gl_y);
    glDrawPixels(copy_width, copy_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    glFlush();
}

/*-------------------------------------------------*
 * draw_select_box()                               *
 *      선택 영역 상자를 그린다.                   *
 *      x, y: 선택 영역의 좌상단 좌표              *
 *      width, height: 선택 영역의 크기            *
 *-------------------------------------------------*/
void draw_select_box(int x, int y, int width, int height)
{
    if (width < 0) // 영역의 좌우가 뒤집혔을 경우 이를 교정한다.
    {
        x += width;
        width *= -1;
    }
    if (height < 0) // 영역의 위아래가 뒤집혔을 경우 이를 교정한다.
    {
        y += height;
        height *= -1;
    }

    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(3.0);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glBegin(GL_LINES);
    {
        int init_x = x;
        int init_y = wh - y - height;
        int i;
        int horizontal_dots = width / 5; // 수평 점선에서 점의 수를 결정한다.
        int vertical_dots = height / 5; // 수직 점선에서 점의 수를 결정한다.
        int len = 5;

        horizontal_dots -= horizontal_dots % 2;
        vertical_dots -= vertical_dots % 2;

        for (i = 1; i <= vertical_dots; i++)   // 영역 좌측에 점선을 그린다.
        {
            glVertex2i(init_x, init_y + i * len);
        }

        for (i = 1; i <= vertical_dots; i++)   // 영역 우측에 점선을 그린다.
        {
            glVertex2i(init_x + width, init_y + i * len);
        }

        for (i = 1; i <= horizontal_dots; i++) // 영역 하단에 점선을 그린다.
        {
            glVertex2i(init_x + i * len, init_y);
        }

        for (i = 1; i <= horizontal_dots; i++) // 영역 상단에 점선을 그린다.
        {
            glVertex2i(init_x + i * len, init_y + height);
        }
    }
    glEnd();
    glFlush();
    glDisable(GL_COLOR_LOGIC_OP);

    glLineWidth(1.0);
}

/*-------------------------------------------------*
 * draw_circle()                                   *
 *      브레스넘 알고리즘을 통해 원을 그린다.      *
 *      cx, cy: 원의 중심점 좌표                   *
 *      r: 원의 반지름                             *
 *      fill: 원을 채울지 말지를 결정한다.         *
 *        0일 때 테두리만 그리고 1일 때 채운다.*
 *-------------------------------------------------*/
void draw_circle(int cx, int cy, int r, int fill)
{
    int x, y, p;
    x = 0; y = r;
    p = 3 - (r << 1); // 2 * r - 3
    if (fill)
        glBegin(GL_LINES);  // 채움 기능을 이용할 경우 수평으로 연속적으로 선을 그린다.
    else
        glBegin(GL_POINTS); // 채움 기능을 이용하지 않을 경우 점을 그린다.

    while (x <= y)
    {
        /* 브레스넘 알고리즘은 두 번째 8분면에만 적용되므로,  *
         * 결과 좌표를 대칭시켜서 모든 팔분면에 점을 그려준다.*/
        glVertex2i(cx + x, cy + y);
        glVertex2i(cx - x, cy + y);

        glVertex2i(cx + x, cy - y);
        glVertex2i(cx - x, cy - y);

        glVertex2i(cx + y, cy + x);
        glVertex2i(cx - y, cy + x);

        glVertex2i(cx + y, cy - x);
        glVertex2i(cx - y, cy - x);
        if (p <= 0) // 동쪽 지점으로 진행한다.
        {
            p += (x << 2) + 6; // P(k+1) = P(k) + 4xk + 6
        }
        else // 동남쪽 지점으로 진행한다.
        {
            p += ((x - y) << 2) + 10; // P(k+1) = P(k) + 4(xk - yk) + 10
            y--;
        }
        x++;
    }

    glEnd();
    glFlush();
}

void display(void)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glClearColor (0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	menuHighlight();
	glPopAttrib();

    is_selected = FALSE; // 화면을 지우고 새로 그릴 때 선택을 해제한다.
}

/*---------------------------------------------------*
 * drawMenuButton()                                  *
 *      대상 메뉴 버튼을 그린다.                     *
 *      선택된 메뉴는 노란색, 아니면 흰색으로 그린다.*
 *      target_menu: 그리고자 하는 메뉴의 그리기 모드*
 *      x, y, len: 메뉴의 좌표와 길이                *
 *---------------------------------------------------*/
void drawMenuButton(int target_menu, int x, int y, int len)
{
    if (draw_mode != target_menu)
        glColor3f(1.0, 1.0, 1.0);
    else
        glColor3f(1.0, 1.0, 0.0);

    screen_box(x, y, len);
}

void menuHighlight(void)
{
    glLineWidth(1);

    // 메뉴를 그린다.
    drawMenuButton(DRAW_LINE, 0, wh - ww / 10, ww / 10);                // 직선 메뉴
    drawMenuButton(DRAW_RECTANGLE, ww / 10, wh - ww / 10, ww / 10);     // 직사각형 메뉴
    drawMenuButton(DRAW_TRIANGLE, ww/5,wh-ww/10, ww/10);                // 삼각형 메뉴
    drawMenuButton(DRAW_POINTS, 3 * ww / 10, wh - ww / 10, ww / 10);    // 점 메뉴
    drawMenuButton(DRAW_TEXT, 2 * ww / 5, wh - ww / 10, ww / 10);       // 텍스트 메뉴
    drawMenuButton(PEN, 5 * ww / 10, wh - ww / 10, ww / 10);            // 펜 메뉴
    drawMenuButton(ERASER, 6 * ww / 10, wh - ww / 10, ww / 10);         // 지우개 메뉴
    drawMenuButton(SELECT, 7 * ww / 10, wh - ww / 10, ww / 10);         // 선택 메뉴
    drawMenuButton(DRAW_CIRCLE, 8 * ww / 10, wh - ww / 10, ww / 10);    // 원 메뉴

	glColor3f(0.0, 0.0, 0.0); //Menu Contents' Color

	screen_box(ww / 10 + ww / 40, wh - ww / 10 + ww / 40, ww / 20);
	glBegin(GL_LINES);
	glVertex2i(wh / 40, wh - ww / 20);
	glVertex2i(wh / 40 + ww / 20, wh - ww / 20);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2i(ww / 5 + ww / 40, wh - ww / 10 + ww / 40);
	glVertex2i(ww / 5 + ww / 20, wh - ww / 40);
	glVertex2i(ww / 5 + 3 * ww / 40, wh - ww / 10 + ww / 40);
	glEnd();
	glPointSize(3.0);
	glBegin(GL_POINTS);
	glVertex2i(3 * ww / 10 + ww / 20, wh - ww / 20);
	glEnd();

    // 텍스트 메뉴
    int f_size = ww / 30;
    if (built_font != ARIAL || built_font_size != f_size)
    {
        built_font = ARIAL;
        built_font_size = f_size;
        glColor3f(0.0, 0.0, 0.0);
        BuildFontWithEnum(built_font, built_font_size);
    }

    glRasterPos2i(2 * ww / 5 + ww / 120, wh - ww / 16);
    glPrint("TEXT");

    // 펜 메뉴
    glRasterPos2i(ww / 2 + ww / 80, wh - ww / 16);
    glPrint("PEN");

    // 지우개 메뉴
    glRasterPos2i(3 * ww / 5 + ww / 200, wh - ww / 16);
    glPrint("Erase");

    // 선택 메뉴
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(ww / 500.0);
    glBegin(GL_LINES);
    {
        int init_x = 7 * ww / 10 + ww / 40;
        int init_y = wh - ww / 10 + ww / 40;
        int i;

        for (i = 1; i < 7; i++) // 영역 좌측에 점선을 그린다.
            glVertex2i(init_x, init_y + i * ww / 140);

        for (i = 1; i < 7; i++) // 영역 우측에 점선을 그린다.
            glVertex2i(init_x + ww / 20, init_y + i * ww / 140);

        for (i = 1; i < 7; i++) // 영역 하단에 점선을 그린다.
            glVertex2i(init_x + i * ww / 140, init_y);

        for (i = 1; i < 7; i++) // 영역 상단에 점선을 그린다.
            glVertex2i(init_x + i * ww / 140, init_y + ww / 20);
    }
    glEnd();

    // 원 메뉴
    glColor3f(0.0, 0.0, 0.0);
    draw_circle(round(8.5 * ww / 10.0), wh - ww / 20, ww / 40, 0);

	glFlush();
}

//Adding New 20203044
void motionFunc(int x, int y)	
{
	glColor4f(r, g, b, a);
	glLineWidth(lineWidth);

	switch (draw_mode)
	{
        case(DRAW_LINE):
            if (rubberband)
            {
                y = wh - y;
                glEnable(GL_COLOR_LOGIC_OP);
                glLogicOp(GL_XOR); // 이전 직선을 XOR 연산으로 지운다.
                glBegin(GL_LINES);

                if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
                    glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);

                glVertex2i(xp[0], yp[0]); // 기존 선분 지우기
                glVertex2i(xp[1], yp[1]);

                glVertex2i(xp[0], yp[0]); // 새로운 선분 그리기
                glVertex2i(x, y);
                glEnd();

                xp[1] = x; // 선분의 끝 점 유지하기
                yp[1] = y;

                glFlush();
                glDisable(GL_COLOR_LOGIC_OP);
            }
            break;
        case(DRAW_RECTANGLE):
            if (rubberband)
            {
                y = wh - y;
                glEnable(GL_COLOR_LOGIC_OP);
                glLogicOp(GL_XOR); // 이전 직사각형을 XOR 연산으로 지운다.

                if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
                    glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);

                glBegin(GL_LINE_LOOP); // 이전의 직사각형 지우기
                glVertex2i(xp[1], yp[1]);
                glVertex2i(xp[1], yp[0]);
                glVertex2i(xp[0], yp[0]);
                glVertex2i(xp[0], yp[1]);
                glEnd();
            

                glBegin(GL_LINE_LOOP); // 새로운 직사각형 그리기
                glVertex2i(x, y);
                glVertex2i(x, yp[0]);
                glVertex2i(xp[0], yp[0]);
                glVertex2i(xp[0], y);
                glEnd();
                glFlush();

                xp[1] = x; // 직사각형의 끝 점 유지
                yp[1] = y;

                rubberband = 1; // 새로운 vertex가 찍혔을때 기존의 직사각형 끊기
                glDisable(GL_COLOR_LOGIC_OP);
            }
            break;
	    case(ERASER):
		    glColor4f(0.8, 0.8, 0.8, a);
		    glLineWidth(lineWidth * 5);
	    case(PEN):
		    if (isInTheRect(x, y, 0, wh, ww - 0, ww / 10)) { // 화면 내에 있고 메뉴 상자 위가 아닐 때
			    glBegin(GL_LINES);
			    glVertex2i(x, wh - y);
			    glVertex2i(xp[0], wh - yp[0]);
			    glEnd();
			    xp[0] = x;
			    yp[0] = y;
		    }
		    glFlush();
		    break;
        case(DRAW_CIRCLE):
            if (rubberband)
            {
                glEnable(GL_COLOR_LOGIC_OP);
                glLogicOp(GL_XOR); // 기존에 그려진 원을 XOR 연산으로 지운다.

                if (!(r == 1.0 && g == 1.0 && b == 1.0)) // 백색이 아니면 XOR 값이 본래 색이 되도록 한다.
                    glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);

                int cur_x = xp[1];
                int cur_y = yp[1];
                // 반지름을 구한다.
                int r = (int)sqrtf((float)((cur_x - xp[0]) * (cur_x - xp[0]) + (cur_y - yp[0]) * (cur_y - yp[0])));
                draw_circle(xp[0], wh - yp[0], r, fill); // 기존 원을 지운다.

                xp[1] = x;
                yp[1] = y;

                cur_x = x;
                cur_y = y;
                // 반지름을 구한다.
                r = (int)sqrtf((float)((cur_x - xp[0]) * (cur_x - xp[0]) + (cur_y - yp[0]) * (cur_y - yp[0])));
                draw_circle(xp[0], wh - yp[0], r, fill); // 새로운 원을 그린다.

                glDisable(GL_COLOR_LOGIC_OP);
            }
            break;
	}
}


bool isInTheRect(int x, int y, int left, int top, int right, int bottom) {
	if(left <= x && x < right && bottom <= y && y < top)
		return true;
	else 
		return false;
}


int main(int argc, char** argv)
{
    int c_menu, p_menu, f_menu, ft_menu, ft_size_menu;

    // GLUT 기본 설정
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("CGGroup11_MiniProject");
    glutDisplayFunc(display);

    // Glut 메뉴
    c_menu = glutCreateMenu(color_menu);
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Green", 2);
    glutAddMenuEntry("Blue", 3);
    glutAddMenuEntry("Cyan", 4);
    glutAddMenuEntry("Magenta", 5);
    glutAddMenuEntry("Yellow", 6);
    glutAddMenuEntry("White", 7);
    glutAddMenuEntry("Black", 8);

    p_menu = glutCreateMenu(pixel_menu);
    glutAddMenuEntry("increase pixel size", 1);
    glutAddMenuEntry("decrease pixel size", 2);
    glutAddMenuEntry("increase line width", 3);
    glutAddMenuEntry("decrease line width", 4);

    f_menu = glutCreateMenu(fill_menu);
    glutAddMenuEntry("fill on", 1);
    glutAddMenuEntry("fill off", 2);

    ft_menu = glutCreateMenu(font_menu); // Font Menu
    glutAddMenuEntry("Arial", 1);
    glutAddMenuEntry("Courier New", 2);
    glutAddMenuEntry("Calibri", 3);
    glutAddMenuEntry("Times New Roman", 4);
    glutAddMenuEntry("Book Antiqua", 5);
    glutAddMenuEntry("Trebuchet MS", 6);

    ft_size_menu = glutCreateMenu(font_size_menu); // Font Size Menu
    glutAddMenuEntry("Font Size Up (+10)", 1);
    glutAddMenuEntry("Font Size Up (+5)", 2);
    glutAddMenuEntry("Font Size Down (-5)", 3);
    glutAddMenuEntry("Font Size Down (-10)", 4);

    glutCreateMenu(right_menu);
    glutAddMenuEntry("quit", 1);
    glutAddMenuEntry("clear", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutCreateMenu(middle_menu);
    glutAddSubMenu("Colors", c_menu);
    glutAddSubMenu("Pixel Size", p_menu);
    glutAddSubMenu("Fill", f_menu);
    glutAddSubMenu("Font", ft_menu);
    glutAddSubMenu("Font Size", ft_size_menu);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    // 초기화하기
    myinit();
    InitFont();
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutMotionFunc(motionFunc);
    glutMainLoop();
    atexit(KillFont); // 종료시에 폰트를 제거한다.

    if (pixels != NULL)
        free(pixels);

    return 0;
}