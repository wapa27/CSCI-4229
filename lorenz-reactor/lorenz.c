/*
* Warren Payne, Lorenz Reactor
*/

/*
 * Key Bindings:
 * Up Arrow         Rotate elevation angle +5 
 * Down Arrow       Rotate elevation angle -5 
 * Right Arrow      Rotate Azimuth angle +5 
 * Left Arrow       Rotate Azimuth angle -5
 * R/r              increase/decrease rho
 * S/s              increase/decrease sigma
 * B/b              increase/decrease beta
 * ESC              Exit
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include </GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

double coordinates[50000][3]; //glVertex3dv takes an array
int arbitrary_num = 50000;
int th, ph = 0; // Azimuth view of angle and elevation view of angle
double w = .04; //W variable
double dim = 2;
double sigma = 10;
double rho = 28;
double beta = 8 / 3;

// Print taken outright from ex6 from class
#define LEN 8192
void Print(const char *format, ...)
{
    char buf[LEN];
    char *ch = buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args, format);
    vsnprintf(buf, LEN, format, args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

// this method used to create an array of points for lorenz equations
// will use the array in display to project the attractor
void lorenz(void)
{
    // initial values
    double x, y, z, dx, dy, dz, dt;
    x = 1;
    y = z = 1;

    dt = .001;

    for (int i = 0; i < arbitrary_num; i++)
    {
        // lorenz equations
        dx = sigma * (y - x);
        dy = x * (rho - z) - y;
        dz = x * y - beta * z;

        // delta
        x += dx * dt;
        y += dy * dt;
        z += dz * dt;

        //fill array so we can use glVertex3dv
        coordinates[i][0] = x * w;
        coordinates[i][1] = y * w;
        coordinates[i][2] = z * w;
    }
}

void display()
{
    // fill array
    lorenz();
    // clear image
    glClear(GL_COLOR_BUFFER_BIT);
    // reset transformations
    glLoadIdentity();

    // initial view angle
    glRotated(ph, 1, 0, 0);
    glRotated(th, 0, 1, 0);
    int i;
    glBegin(GL_LINE_STRIP);
    // plot lorenz attractor
    for (i = 0; i < arbitrary_num; i++)
    {
        glColor3dv(coordinates[i]);
        glVertex3dv(coordinates[i]);
    }
    glEnd();

    // making axes (white)
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 1);
    glEnd();

    // labeling axes, thank you ex6!
    glRasterPos3d(1, 0, 0);
    Print("X");
    glRasterPos3d(0, 1, 0);
    Print("Y");
    glRasterPos3d(0, 0, 1);
    Print("Z");
    glWindowPos2i(5, 5);
    Print("View Angle=%d, %d", th, ph);

    glFlush();
    glutSwapBuffers();
}

void key(unsigned char ch, int x, int y)
{
    // Exit on ESC
    if (ch == 27)
        exit(0);

    // increase rho value
    else if (ch == 'R')
    {
        rho += 1;
        printf("New rho: %f\n", rho);
    }
    // decrease rho value
    else if (ch == 'r')
    {
        if (rho == 0)
        {
            printf("rho is already zero\n");
        }
        else
        {
            rho -= 1;
            printf("New rho: %f\n", rho);
        }
    }
    // increase beta value
    else if (ch == 'B')
    {
        beta += 1;
        printf("New beta: %f\n", beta);
    }
    // reduce beta value
    else if (ch == 'b')
    {
        if (beta == 0)
        {
            printf("beta is already zero\n");
        }
        else
        {
            beta -= 1;
            printf("New beta: %f\n", beta);
        }
    }
    // increase sigma value
    else if (ch == 'S')
    {
        sigma += 1;
        printf("New sigma: %f\n", beta);
    }
    // reduce sigma value
    else if (ch == 's')
    {
        if (sigma == 0)
        {
            printf("sigma is already zero\n");
        }
        else
        {
            beta -= 1;
            printf("New sigma: %f\n", beta);
        }
    }
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    // adjust elevation ph
    if (key == GLUT_KEY_UP)
    {
        ph += 5;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        ph += 5;
    }
    // adjust Azimuth
    else if (key == GLUT_KEY_RIGHT)
    {
        th += 5;
    }
    else if (key == GLUT_KEY_LEFT)
    {
        th += 5;
    }

    th %= 360;
    ph %= 360;
    glutPostRedisplay();
}

// reshape is largely taken from ex6 in class
void reshape(int width, int height)
{
    double w2h = (height > 0) ? (double)width / height : 1;
    //  Setting viewport
    glViewport(0, 0, width, height);
    //  specifying which of the viewing pipelines is to be modified (projection matrix)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //  Orthogonal projection box adjusted for the
    //  aspect ratio of the window
    glOrtho(-dim * w2h, +dim * w2h, -dim, +dim, -dim, +dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Lorenz Reactor, Warren Payne");

    // call program functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}