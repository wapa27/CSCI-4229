
/* 
 * Little neigborhood with Lighting - Warren Payne
*/

#include "CSCIx229.h"

int th = 0;  // Azimuth
int ph = 10; // elevation
int axes = 1;
int move = 1;
int proj_mode = 1; // projection mode
double dim = 5.0;  // size of world
double asp = 1;
int fov = 80; // field of view,
int light = 1;

// double fpx, fpy, fpz;    // first person x, y, and z
// double fpox, fpoy, fpoz; // first person object they are looking at; x, y, z of that object
// double yaw = 0;          // rotation around x axis
// double pitch = 0;        // rotation around y axis

// Light values
int distance = 5;  // Light distance
int inc       =  10;  // Ball increment
int smooth = 1;    // Smooth/Flat shading
int local = 0;     // Local Viewer Model
int emission = 0;  // Emission intensity (%)
int ambient = 30;  // Ambient intensity (%)
int diffuse = 100; // Diffuse intensity (%)
int specular = 0;  // Specular intensity (%)
int shininess = 0; // Shininess (power of two)
float shiny = 1;    // Shininess (value)
int zh = 90;       // Light azimuth
float ylight = 0;  // Elevation of light

static void Vertex(double th, double ph)
{
    double x = Sin(th) * Cos(ph);
    double y = Cos(th) * Cos(ph);
    double z = Sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x, y, z);
    glVertex3d(x, y, z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x, double y, double z, double r)
{
    int th, ph;
    float yellow[] = {1.0, 1.0, 0.0, 1.0};
    float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x, y, z);
    glScaled(r, r, r);
    //  White ball
    glColor3f(1, 1, 1);
    glMaterialf(GL_FRONT, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
    glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
    //  Bands of latitude
    for (ph = -90; ph < 90; ph += inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += 2 * inc)
        {
            Vertex(th, ph);
            Vertex(th, ph + inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glPopMatrix();
}

// To build cookie cutter homes
static void house(double x, double y, double z,
                  double dx, double dy, double dz,
                  double th)
{
    float white[] = {1, 1, 1, 1};
    float black[] = {0, 0, 0, 1};
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);

    glBegin(GL_QUADS);

    // always start with a foundation
    glColor3ub(255, 0, 0);
    glVertex3d(-1, -1, +1); //front left
    glVertex3d(-1, -1, -1); // back left
    glVertex3d(+1, -1, -1); // back right
    glVertex3d(+1, -1, +1); // front right

    // top
    glVertex3d(-1, 1, 1);
    glVertex3d(-1, 1, -1);
    glVertex3d(1, 1, -1);
    glVertex3d(1, 1, 1);

    // left
    glColor3ub(112, 70, 27);
    glNormal3f(-1,1,0);
    glVertex3d(-1, 1, 1);
    glVertex3d(-1, 1, -1);
    glVertex3d(-1, -1, -1);
    glVertex3d(-1, -1, 1);

    // right
    glNormal3f(1,1,0);
    glVertex3d(1, 1, 1);
    glVertex3d(1, 1, -1);
    glVertex3d(1, -1, -1);
    glVertex3d(1, -1, 1);

    // front
    glColor3ub(194, 114, 35);
    glNormal3f(0,0,1);
    glVertex3d(-1, -1, 1);
    glVertex3d(-1, 1, 1);
    glVertex3d(1, 1, 1);
    glVertex3d(1, -1, 1);

    // back
    glNormal3f(0,0,-1);
    glVertex3d(-1, -1, -1);
    glVertex3d(-1, 1, -1);
    glVertex3d(1, 1, -1);
    glVertex3d(1, -1, -1);

    glEnd();

    // let's do a roof

    glBegin(GL_TRIANGLES);
    // front
    glColor3ub(94, 58, 23);
    glNormal3f(0, 0, +1);
    glVertex3d(-1.2, 1, 1.2);
    glVertex3d(1.2, 1, 1.2);
    glVertex3d(0, 2.2, 0);

    // back
    glNormal3f(0, 0, -1);
    glVertex3d(-1.2, 1, -1.2);
    glVertex3d(1.2, 1, -1.2);
    glVertex3d(0, 2.2, 0);

    // left
    glNormal3f(-1, 1, 0);
    glVertex3d(-1.2, 1, 1.2);
    glVertex3d(-1.2, 1, -1.2);
    glVertex3d(0, 2.2, 0);

    // right
    glNormal3f(1, 1, 0);
    glVertex3d(1.2, 1, 1.2);
    glVertex3d(1.2, 1, -1.2);
    glVertex3d(0, 2.2, 0);
    glEnd();

    // need a door

    glBegin(GL_QUADS);
    glNormal3f(0,1,1);
    glVertex3d(-.9, -1, 1.01);
    glVertex3d(-.2, -1, 1.01);
    glVertex3d(-0.2, 0.2, 1.01);
    glVertex3d(-.9, 0.2, 1.01);

    // rear entrance for luxury estate
    glNormal3f(0,1,-1);
    glVertex3d(-.9, -1, -1.01);
    glVertex3d(-.2, -1, -1.01);
    glVertex3d(-0.2, 0.2, -1.01);
    glVertex3d(-.9, 0.2, -1.01);
    // need windows for fire code sake
    // front window
    glNormal3f(0,1,1);
    glColor3ub(100, 100, 100);
    glVertex3d(.25, .2, 1.01);
    glVertex3d(.25, -0.25, 1.01);
    glVertex3d(.8, -0.25, 1.01);
    glVertex3d(.8, .2, 1.01);

    // two left windows
    glNormal3f(-1,1,0);
    glVertex3d(-1.01, .2, -.25);
    glVertex3d(-1.01, -0.25, -.25);
    glVertex3d(-1.01, -0.25, -.8);
    glVertex3d(-1.01, .2, -.8);

    glVertex3d(-1.01, .2, .25);
    glVertex3d(-1.01, -0.25, .25);
    glVertex3d(-1.01, -0.25, .8);
    glVertex3d(-1.01, .2, .8);

    // two right windows
    glNormal3f(1,1,0);
    glVertex3d(1.01, .2, -.25);
    glVertex3d(1.01, -0.25, -.25);
    glVertex3d(1.01, -0.25, -.8);
    glVertex3d(1.01, .2, -.8);

    glVertex3d(1.01, .2, .25);
    glVertex3d(1.01, -0.25, .25);
    glVertex3d(1.01, -0.25, .8);
    glVertex3d(1.01, .2, .8);

    glEnd();
    glPopMatrix();
}

/* to build super cool A-Frame homes */
static void aFrame(double x, double y, double z,
                   double dx, double dy, double dz,
                   double th)
{
    float white[] = {1, 1, 1, 1};
    float black[] = {0, 0, 0, 1};
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);

     // foundation
    glBegin(GL_QUADS);
    glColor3ub(245, 209, 66);
    glVertex3d(-1, -1, +1); //front left
    glVertex3d(-1, -1, -1); // back left
    glVertex3d(+1, -1, -1); // back right
    glVertex3d(+1, -1, +1); // front right
    glEnd();

    // front
    
    glBegin(GL_TRIANGLES);
    glColor3ub(245, 209, 66);
    glNormal3f(0,0,1);
    glVertex3d(+2, -1, +1);
    glVertex3d(-2, -1, 1);
    glVertex3d(0, 2, 1);

    // back
    glNormal3f(0,0,-1);
    glVertex3d(+2, -1, -1);
    glVertex3d(-2, -1, -1);
    glVertex3d(0, 2, -1);

    glEnd();

//     // right side
    
    
    glBegin(GL_QUADS);
    glColor3ub(148, 29, 55);
    glNormal3f(-1,-1,0);
    glVertex3d(-2, -1, -1);
    glVertex3d(-2, -1, 1);
    glVertex3d(0, 2, 1);
    glVertex3d(0, 2, -1);

    // left side
    glNormal3f(1,1,0);
    glVertex3d(2, -1, -1);
    glVertex3d(2, -1, 1);
    glVertex3d(0, 2, 1);
    glVertex3d(0, 2, -1);

    // door
    glNormal3f(0,1,1);
    glVertex3d(-.3, -1, 1.01);
    glVertex3d(.3, -1, 1.01);
    glVertex3d(.3, 0.2, 1.01);
    glVertex3d(-.3, 0.2, 1.01);

    // front window
    glColor3ub(100, 100, 100);
    glNormal3f(0,1,1);
    glVertex3d(.3, .5, 1.01);
    glVertex3d(.3, 1, 1.01);
    glVertex3d(-.3, 1, 1.01);
    glVertex3d(-.3, .5, 1.01);

    // back top window
    glNormal3f(0,1,-1);
    glVertex3d(.3, .5, -1.01);
    glVertex3d(.3, 1, -1.01);
    glVertex3d(-.3, 1, -1.01);
    glVertex3d(-.3, .5, -1.01);

    // back bottom window
    glNormal3f(0,1,-1);
    glVertex3d(.2, 0.1, -1.01);
    glVertex3d(.2, -0.4, -1.01);
    glVertex3d(.9, -0.4, -1.01);
    glVertex3d(.9, 0.1, -1.01);

    // back bottom window #2
    glNormal3f(0,1,-1);
    glVertex3d(-.2, 0.1, -1.01);
    glVertex3d(-.2, -0.4, -1.01);
    glVertex3d(-.9, -0.4, -1.01);
    glVertex3d(-.9, 0.1, -1.01);

    glEnd();

    glPopMatrix();
}

// just a big square at ground level
static void plantGrass()
{
    // float white[] = {1, 1, 1, 1};
    // float black[] = {0, 0, 0, 1};
    // glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
    glPushMatrix();


    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glColor3ub(78, 245, 66);
    glVertex3d(-20, -1.01, 20);
    glVertex3d(-20, -1.01, -20);
    glVertex3d(20, -1.01, -20);
    glVertex3d(20, -1.01, 20);

    glEnd();
    glPopMatrix();
}

// to draw the roads
static void plotRoads()
{
    float white[] = {1, 1, 1, 1};
    float black[] = {0, 0, 0, 1};
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
    glPushMatrix();

    glBegin(GL_QUADS);
    glColor3ub(143, 139, 134);

    // East/West
    glNormal3f(0,1,0);
    glVertex3d(-20, -1.0095, -1);
    glVertex3d(-20, -1.0095, 1);
    glVertex3d(20, -1.0095, 1);
    glVertex3d(20, -1.0095, -1);

    // North/South
    glNormal3f(0,1,0);
    glVertex3d(-1, -1.0095, -20);
    glVertex3d(-1, -1.0095, 20);
    glVertex3d(1, -1.0095, 20);
    glVertex3d(1, -1.0095, -20);

    glEnd();

    // to draw dashed lines for lanes
    glLineStipple(1, 0x00FF);
    glEnable(GL_LINE_STIPPLE);
    glLineWidth(2);
    glBegin(GL_LINES);
    

    glColor3ub(255, 251, 0);

    // // draw lines
    glNormal3f(0,1,0);
    glVertex3d(-20, -1.001, 0);
    glVertex3d(20, -1.001, 0);
    glVertex3d(0, -1.001, 20);
    glVertex3d(0, -1.001, -20);

    glEnd();
    glPopMatrix();
}

void display()
{
    const double len = 4.0;
    // erase window and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // enable Z buffering
    glEnable(GL_DEPTH_TEST);
    // load identity matrix
    glLoadIdentity();

    if (proj_mode == 1)
    {
        // dim = 5.0;
        double Ex = -2 * dim * Sin(th) * Cos(ph);
        double Ey = +2 * dim * Sin(ph);
        double Ez = +2 * dim * Cos(th) * Cos(ph);
        gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
    }
    //  Orthogonal - set world orientation
    else
    {
        // dim = 5.0;
        glRotatef(ph, 1, 0, 0);
        glRotatef(th, 0, 1, 0);
    }
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

    if (light)
    {
        //  Translate intensity to color vectors
        float Ambient[] = {0.01 * ambient, 0.01 * ambient, 0.01 * ambient, 1.0};
        float Diffuse[] = {0.01 * diffuse, 0.01 * diffuse, 0.01 * diffuse, 1.0};
        float Specular[] = {0.01 * specular, 0.01 * specular, 0.01 * specular, 1.0};
        //  Light position
        float Position[] = {distance * 2 * Cos(zh), ylight, distance * 2 * Sin(zh), 1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1, 1, 1);
        ball(Position[0], Position[1], Position[2], 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
        glLightfv(GL_LIGHT0, GL_POSITION, Position);
    }

    else
        glDisable(GL_LIGHTING);

    // Time to draw some stuff

    // houses

    /* error in plotRoads() and aFrame().. probably because I haven't implemented lighting in these yet */
    aFrame(-4, 0, 4, 1, 1, 1, 180);
    house(4, 0, 4, 2, 1, 2, 180);

    aFrame(4, 0, -4, 1, 1, 2, 0);
    house(-4, 0, -4, 1, 1, 1, 0);


    // grass it up
    plantGrass();

    // roads
    plotRoads();

    /* end error */
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1); //white axes
    if (axes)
    {
        glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(len, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, len, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 0.0, len);
        glEnd();
        //  Label axes
        glRasterPos3d(len, 0.0, 0.0);
        Print("X");
        glRasterPos3d(0.0, len, 0.0);
        Print("Y");
        glRasterPos3d(0.0, 0.0, len);
        Print("Z");
    }
    glWindowPos2i(5, 5);
    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
          th, ph, dim, fov, proj_mode ? "Orthogonal" : "Perpective", light ? "On" : "Off");
    if (light)
    {
        glWindowPos2i(5, 45);
        Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f", smooth ? "Smooth" : "Flat", local ? "On" : "Off", distance, ylight);
        glWindowPos2i(5, 25);
        Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f", ambient, diffuse, specular, emission, shiny);
    }

    //  Render the scene and make it visible
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

void special(int key, int x, int y)
{

    // Elevation
    if (key == GLUT_KEY_UP)
    {
        ph += 5;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        ph -= 5;
    }
    // Azimith
    else if (key == GLUT_KEY_RIGHT)
    {
        th += 5;
    }
    else if (key == GLUT_KEY_LEFT)
    {
        th -= 5;
    }
    //  PageUp key - increase dim
    else if (key == GLUT_KEY_PAGE_DOWN)
        dim += 0.1;
    //  PageDown key - decrease dim
    else if (key == GLUT_KEY_PAGE_UP && dim > 1)
        dim -= 0.1;
    //  Smooth color model
    else if (key == GLUT_KEY_F1)
        smooth = 1 - smooth;
    //  Local Viewer
    else if (key == GLUT_KEY_F2)
        local = 1 - local;
    else if (key == GLUT_KEY_F3)
        distance = (distance == 1) ? 5 : 1;


    th %= 360;
    ph %= 360;
    Project(proj_mode ? fov : 0, asp, dim);
    glutPostRedisplay();
}

/*
* ex9
*/
void key(unsigned char ch, int x, int y)
{
    // EXC = exit
    if (ch == 27)
    {
        exit(0);
    }
    else if (ch == '0')
    {
        th = ph = 0;
    }
    // axes
    else if (ch == 'x' || ch == 'X')
        axes = 1 - axes;
    else if (ch == 'p' || ch == 'P')
    {
        proj_mode = 1 - proj_mode;
        // if(proj_mode == 1){
        //     dim = 7;
        //     fov = 140;
        // }
        // else{
        //     dim = 5;
        // }
    }
    //  Change field of view angle
    else if (ch == '-' && ch > 1)
    {
        fov--;
        // printf("%s, %d, %s", "the new fov is: ", fov, "\n");
    }
    else if (ch == '+' && ch < 179)
    {
        fov++;
        // printf("%s, %d, %s", "the new fov is: ", fov, "\n");
    }
    else if (ch == 'l' || ch == 'L')
    {
        light = 1 - light;
    }
    else if (ch == 'm' || ch == 'M')
        move = 1 - move;
    //  Move light
    else if (ch == '<')
        zh += 1;
    else if (ch == '>')
        zh -= 1;
    //  Change field of view angle
    else if (ch == '-' && ch > 1)
        fov--;
    else if (ch == '+' && ch < 179)
        fov++;
    //  Light elevation
    else if (ch == '[')
        ylight -= 0.1;
    else if (ch == ']')
        ylight += 0.1;
    //  Ambient level
    else if (ch == 'a' && ambient > 0)
        ambient -= 5;
    else if (ch == 'A' && ambient < 100)
        ambient += 5;
    //  Diffuse level
    else if (ch == 'd' && diffuse > 0)
        diffuse -= 5;
    else if (ch == 'D' && diffuse < 100)
        diffuse += 5;
    //  Specular level
    else if (ch == 's' && specular > 0)
        specular -= 5;
    else if (ch == 'S' && specular < 100)
        specular += 5;
    //  Emission level
    else if (ch == 'e' && emission > 0)
        emission -= 5;
    else if (ch == 'E' && emission < 100)
        emission += 5;
    //  Shininess level
    else if (ch == 'n' && shininess > -1)
        shininess -= 1;
    else if (ch == 'N' && shininess < 7)
        shininess += 1;
    //  Translate shininess power to value (-1 => 0)
    shiny = shininess < 0 ? 0 : pow(2.0, shininess);
    //  Reproject
    Project(proj_mode ? fov : 0, asp, dim);
    //  Animate if requested
// glutIdleFunc(move?idle:NULL);
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    // aspect ratio = x : y = width/height
    asp = (height > 0) ? (double)width / height : 1;
    glViewport(0, 0, width, height);

    Project(proj_mode ? fov : 0, asp, dim);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    zh = fmod(90 * t, 360.0);
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    // initialize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(580, 400);
    glutCreateWindow("Warren Payne - Homework 5");

    // call functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    ErrCheck("init");
    // main loop
    glutMainLoop();
    return 0;
}