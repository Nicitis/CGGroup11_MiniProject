/* This program illustrates the use of the glut library for
interfacing with a window system */

#include <glut.h>
#include <stdlib.h>
#include "BitmapText.h"

#define DRAW_NULL 0
#define DRAW_LINE 1
#define DRAW_RECTANGLE 2
#define DRAW_TRIANGLE  3
#define DRAW_POINTS 4
#define DRAW_TEXT 5
//Adding New 20203044
#define PEN 6
#define ERASER 7
#define SELECT 8
#define DRAW_CIRCLE 9

#define MAX_KEY 256             // Max Text Length
#define FONTDELTASIZE (int)5;   // Delta of font size for setting the font size

void mouse(int, int, int, int);
void key(unsigned char, int, int);
void display(void);
void drawSquare(int, int);
void myReshape(GLsizei, GLsizei);

void myinit(void);

void screen_box(int, int, int);
void right_menu(int);
void middle_menu(int);
void color_menu(int);
void pixel_menu(int);
void fill_menu(int);
void font_menu(int);
void font_size_menu(int);
int pick(int, int);

//Adding New 20203044
void motionFunc(int, int);
bool isInTheRect(int, int, int, int, int, int);
void draw_menu(int, int, int, int);
void menuHighlight(void);
void swap(int*, int*);
void copy(int, int);
void paste(int, int);
void draw_select_box(int, int, int, int);
void draw_circle(int, int, int, int);

/* globals */

GLsizei wh = 500, ww = 500; /* initial window size */
GLfloat size = 3.0;   /* half side length of square */

//Adding New 20203044
GLfloat lineWidth = 1.0; // Line Size
int rubberband = 0;
int xp[2], yp[2];

int draw_mode = 0; /* drawing mode */
int rx, ry; /*raster position*/
int key_count = 0; /* a counter for text string */

// Adding a addtionally 20203044
GLfloat r = 0.0, g = 0.0, b = 0.0, a = 1.0;  /* drawing color */ 
int fill = 0; /* fill flag */

char key_buffer[MAX_KEY];           /* Text buffer */
FONT font = ARIAL;                  /* Font Storage */
FONT built_font = ARIAL;                  /* Font Storage(Current built font) */
int font_size = DEFAULTFONTSIZE;    /* Font Size */
int built_font_size = DEFAULTFONTSIZE;    /* Font Size(Current built font size) */

int is_selected = FALSE;
int hold_left_btn = FALSE;
GLubyte* pixels = NULL; /* Copied pixels */
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
    
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        where = pick(x, y);
        glColor3f(r, g, b);
        if (where != 0)
        {
            count = 0;
            draw_mode = where;

            if (is_selected) // De-select the area.
            {
                is_selected = FALSE;
                draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
            }
        }
        else switch (draw_mode)
        {
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
                    glLogicOp(GL_XOR);

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
                    glLogicOp(GL_XOR);

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
                drawSquare(x, y);
                count++;
                break;
            case(DRAW_TEXT):
                if (strlen(key_buffer) > 0)
                {
                    glRasterPos2i(rx, ry);
                    glPrint(key_buffer);
                }

                rx = x;
                ry = wh - y;
                glRasterPos2i(rx, ry);
                count = 0;

                // Clear key buffer.
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
            case(SELECT):
                if (!is_selected)
                {
                    xp[0] = x;
                    yp[0] = y;
                    hold_left_btn = TRUE;
                }
                break;
            case(DRAW_CIRCLE):
                if (count == 0)
                {
                    if (!isInTheRect(x, wh - y, 0, wh - wh / 10, ww, 0))
                        break;
                    xp[0] = x;
                    yp[0] = y;
                    xp[1] = x;
                    yp[1] = y;

                    glEnable(GL_COLOR_LOGIC_OP);
                    glLogicOp(GL_XOR);

                    if (!(r == 1.0 && g == 1.0 && b == 1.0))
                        glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);
                    
                    draw_circle(xp[0], wh - yp[0], 0, fill);

                    glDisable(GL_COLOR_LOGIC_OP);

                    count++;
                    rubberband = 1;
                }
                break;
        }

        glPopAttrib();
        glFlush();
    }
    
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        where = pick(x, y);

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
            case(SELECT):
                if (where != 0)
                    return;

                if (x != xp[0] && y != yp[0] && !is_selected)
                {
                    is_selected = TRUE;
                    xp[1] = x;
                    yp[1] = y;

                    if (xp[0] > xp[1])
                        swap(&xp[0], &xp[1]);
                    if (yp[0] > yp[1])
                        swap(&yp[0], &yp[1]);

                    draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);

                }
                else if (is_selected)
                {
                    is_selected = FALSE;
                    draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
                }
                hold_left_btn = FALSE;
                break;
            case (DRAW_CIRCLE):
                if (where == DRAW_NULL && count == 1)
                {
                    glColor3f(r, g, b);
                    int r = (int)sqrtf((float)((x - xp[0]) * (x - xp[0]) + (y - yp[0]) * (y - yp[0])));
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
}

void fill_menu(int id)
{
    if (id == 1) fill = 1; 
    else fill = 0;
}

/* change the font */
void font_menu(int id)
{
    // id: 1 ~ 6
    // FONT = arial, courier new, calibri, times new roman, book antiqua, trebuchet ms
    font = (FONT)id;

    // Erase previous writting texts
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
        glRasterPos2i(rx, ry);
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_XOR);
        glPrint(key_buffer);
    }

    built_font = font;
    built_font_size = font_size;
    BuildFontWithEnum(font, font_size);

    // re-draw texts
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
        glRasterPos2i(rx, ry);
        glPrint(key_buffer);
        glFlush();
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

/* Adjust font size */
void font_size_menu(int id)
{
    // Erase previous writting text
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
        glRasterPos2i(rx, ry);
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_XOR);
        glPrint(key_buffer);
    }

    if (id == 1)
    {
        font_size += 2 * FONTDELTASIZE;
    }
    else if (id == 2)
    {
        font_size += FONTDELTASIZE;
    }
    else if (id == 3)
    {
        font_size -= FONTDELTASIZE;
    }
    else
    {
        font_size -= 2 * FONTDELTASIZE;
    }

    if (font_size <= 0)
    {
        font_size = 5;
    }

    built_font = font;
    built_font_size = font_size;
    BuildFontWithEnum(font, font_size);

    // re-draw text
    if (draw_mode == DRAW_TEXT && key_count > 0)
    {
        glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
        glRasterPos2i(rx, ry);
        glPrint(key_buffer);
        glFlush();
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

// key callback function for entering text.
void key(unsigned char k, int xx, int yy)
{
    // Copy and Paste Tool
    if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
    {
        k += 96;
        if (k == 'c' && draw_mode == SELECT && is_selected)
        {
            is_selected = FALSE;
            draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
            copy(xx, yy);
            printf("COPY\n");
        }
        else if (k == 'v' && pixels != NULL)
        {
            if (is_selected)
            {
                draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
            }

            is_selected = TRUE;
            paste(xx, yy);
            xp[0] = xx;
            yp[0] = yy;
            xp[1] = xx + copy_width;
            yp[1] = yy + copy_height;

            draw_select_box(xp[0], yp[0], xp[1] - xp[0], yp[1] - yp[0]);
            printf("PASTE\n");
        }
        return;
    }

    // Text Tool
    if (draw_mode != DRAW_TEXT) return;
    if (k != 8 && k < 32) return; // exclude Control keys(000~031) except Backspace (008)

    if (built_font != font || built_font_size != font_size)
    {
        built_font = font;
        built_font_size = font_size;
        BuildFontWithEnum(font, font_size);
    }

    // Erase the text.
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
    glRasterPos2i(rx, ry);
    glPrint(key_buffer);

    if (k == 8 && key_count > 0)
    {
        // if the users enters the Backspace, delete the character.
        key_count--;
        key_buffer[key_count] = '\0';
    }
    if (k != 8 && key_count < MAX_KEY - 1)
    {
        // if the user enters normal keys, record it in the key buffer.
        key_buffer[key_count] = k;
        key_count++;
    }

    // Redraw the text
    glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
    if (!(r == 1.0 && g == 1.0 && b == 1.0))
        glColor3f(1.0 - r, 1.0 - g, 1.0 - b); // If color is white, replace color with white color
    glRasterPos2i(rx, ry);
    glPrint(key_buffer);
    
    glFlush();
    glDisable(GL_COLOR_LOGIC_OP);
}

void swap(int* p1, int* p2)
{
    int temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

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

void paste(int x, int y)
{
    int gl_x = x;
    int gl_y = wh - y - copy_height;

    // constrict the range of y 
    if (gl_y < 0) gl_y = 0;
    if (gl_y > wh - copy_height - ww / 10) gl_y = wh - copy_height - ww / 10;

    glRasterPos2i(gl_x, gl_y);
    glDrawPixels(copy_width, copy_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    glFlush();
}

void draw_select_box(int x, int y, int width, int height)
{
    if (width < 0)
    {
        x += width;
        width *= -1;
    }
    if (height < 0)
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
        int horizontal_dots = width / 5;
        int vertical_dots = height / 5;
        int len = 5;

        horizontal_dots -= horizontal_dots % 2;
        vertical_dots -= vertical_dots % 2;

        // Western dotted line
        for (i = 1; i <= vertical_dots; i++)
        {
            glVertex2i(init_x, init_y + i * len);
        }

        // Eastern dotted line
        
        for (i = 1; i <= vertical_dots; i++)
        {
            glVertex2i(init_x + width, init_y + i * len);
        }

        // Southern dotted line
        for (i = 1; i <= horizontal_dots; i++)
        {
            glVertex2i(init_x + i * len, init_y);
        }

        // Northern dotted line
        for (i = 1; i <= horizontal_dots; i++)
        {
            glVertex2i(init_x + i * len, init_y + height);
        }
    }
    glEnd();
    glFlush();
    glDisable(GL_COLOR_LOGIC_OP);

    glLineWidth(1.0);
}

// 브레스넘 알고리즘을 통한 원 그리기 함수
void draw_circle(int cx, int cy, int r, int fill)
{
    int x, y, p;
    x = 0; y = r;
    p = 3 - (r << 1); // 2 * r - 3
    if (fill)
        glBegin(GL_LINES);
    else
        glBegin(GL_POINTS);

    while (x <= y)
    {
        glVertex2i(cx + x, cy + y);
        glVertex2i(cx - x, cy + y);

        glVertex2i(cx + x, cy - y);
        glVertex2i(cx - x, cy - y);

        glVertex2i(cx + y, cy + x);
        glVertex2i(cx - y, cy + x);

        glVertex2i(cx + y, cy - x);
        glVertex2i(cx - y, cy - x);
        if (p <= 0) // draw a point at east.
        {
            p += (x << 2) + 6; // P(k+1) = P(k) + 4xk + 6
        }
        else
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
}

void draw_menu(int target_menu, int x, int y, int len)
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

    // Line Menu
    draw_menu(DRAW_LINE, 0, wh - ww / 10, ww / 10);

    // Rectangle Menu
    draw_menu(DRAW_RECTANGLE, ww / 10, wh - ww / 10, ww / 10);

    // Triangle Menu
    draw_menu(DRAW_TRIANGLE, ww/5,wh-ww/10, ww/10);

    // Point Menu
    draw_menu(DRAW_POINTS, 3 * ww / 10, wh - ww / 10, ww / 10);

    // Text Menu
    draw_menu(DRAW_TEXT, 2 * ww / 5, wh - ww / 10, ww / 10);

    // Pen Menu
    draw_menu(PEN, 5 * ww / 10, wh - ww / 10, ww / 10);

    // Eraser Menu
    draw_menu(ERASER, 6 * ww / 10, wh - ww / 10, ww / 10);

    // Select Menu
    draw_menu(SELECT, 7 * ww / 10, wh - ww / 10, ww / 10);

    // Circle Menu
    draw_menu(DRAW_CIRCLE, 8 * ww / 10, wh - ww / 10, ww / 10);

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

    // Text Menu
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

    //Pen Menu
    glRasterPos2i(ww / 2 + ww / 80, wh - ww / 16);
    glPrint("PEN");

    //Eraser Menu
    glRasterPos2i(3 * ww / 5 + ww / 200, wh - ww / 16);
    glPrint("Erase");

    // Select Menu
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(ww / 500.0);
    glBegin(GL_LINES);
    {
        int init_x = 7 * ww / 10 + ww / 40;
        int init_y = wh - ww / 10 + ww / 40;
        int i;

        // Western dotted line
        for (i = 1; i < 7; i++)
            glVertex2i(init_x, init_y + i * ww / 140);

        // Eastern dotted line
        for (i = 1; i < 7; i++)
            glVertex2i(init_x + ww / 20, init_y + i * ww / 140);

        // Southern dotted line
        for (i = 1; i < 7; i++)
            glVertex2i(init_x + i * ww / 140, init_y);

        // Northern dotted line
        for (i = 1; i < 7; i++)
            glVertex2i(init_x + i * ww / 140, init_y + ww / 20);
    }
    glEnd();

    // Circle Menu
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
                glLogicOp(GL_XOR); // XOR 연산
                glBegin(GL_LINES);

                if (!(r == 1.0 && g == 1.0 && b == 1.0))
                    glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);

                glVertex2i(xp[0], yp[0]); // 기존 선분 지우기
                glVertex2i(xp[1], yp[1]);

                glVertex2i(xp[0], yp[0]); // 새로운 RubberBand_line 그리기
                glVertex2i(x, y);
                glEnd();

                xp[1] = x; // RubberBand_line의 끝 점 유지하기
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
                glLogicOp(GL_XOR); // XOR연산

                if (!(r == 1.0 && g == 1.0 && b == 1.0))
                    glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);

                glBegin(GL_LINE_LOOP); // 이전의 RubberBand_rect 지우기
                glVertex2i(xp[1], yp[1]);
                glVertex2i(xp[1], yp[0]);
                glVertex2i(xp[0], yp[0]);
                glVertex2i(xp[0], yp[1]);
                glEnd();
            

                glBegin(GL_LINE_LOOP); // 새로운 RubberBand_rect 그리기
                glVertex2i(x, y);
                glVertex2i(x, yp[0]);
                glVertex2i(xp[0], yp[0]);
                glVertex2i(xp[0], y);
                glEnd();
                glFlush();

                xp[1] = x; // RubberBand_rect의 끝 점 유지
                yp[1] = y;

                rubberband = 1; // 새로운 vertex가 찍혔을때 기존의 RubberBand_rect 끊기
                glDisable(GL_COLOR_LOGIC_OP);
            }
            break;
	    case(ERASER):
		    glColor4f(0.8, 0.8, 0.8, a);
		    glLineWidth(lineWidth * 5);
	    case(PEN):
		    if (isInTheRect(x, y, 20, wh - ww / 10, ww - 0, 0)) {
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
                glLogicOp(GL_XOR);

                if (!(r == 1.0 && g == 1.0 && b == 1.0))
                    glColor4f(1.0 - r, 1.0 - g, 1.0 - b, a);

                int cur_x = xp[1];
                int cur_y = yp[1];
                int r = (int)sqrtf((float)((cur_x - xp[0]) * (cur_x - xp[0]) + (cur_y - yp[0]) * (cur_y - yp[0])));
                draw_circle(xp[0], wh - yp[0], r, fill);

                xp[1] = x;
                yp[1] = y;

                cur_x = x;
                cur_y = y;
                r = (int)sqrtf((float)((cur_x - xp[0]) * (cur_x - xp[0]) + (cur_y - yp[0]) * (cur_y - yp[0])));
                draw_circle(xp[0], wh - yp[0], r, fill);

                //glLogicOp(GL_XOR);
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

    // GLUT Setting
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("CGGroup11_MiniProject");
    glutDisplayFunc(display);

    // Glut Menu
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

    // Initialize
    myinit();
    InitFont();
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutMotionFunc(motionFunc);
    glutMainLoop();
    atexit(KillFont); // EXIT

    if (pixels != NULL)
        free(pixels);

    return 0;
}