
/* 
 * Little neigborhood with Lighting and Texture - Warren Payne
*/

#include "CSCIx229.h"

int th = 0;  // Azimuth
int ph = 10; // elevation
int axes = 1;
int move = 0;
int proj_mode = 1; // projection mode
double dim = 5.0;  // size of world
double asp = 1;
int fov = 80; // field of view,
int light = 1; // enable/disable lighting


/* derived from example 13 :
 * Vertex() and Ball()
 *
 */
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

// Texture names
unsigned int texture[10];

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

    // glMaterialf(which face?, single-valued material parameter of the face, value of material parameter)
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

    // Texture stuff
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    
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
    glEnd();

    // left
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glColor3ub(112, 70, 27);
    glNormal3f(-1,1,0);
    glTexCoord2f(1,1); glVertex3d(-1, 1, 1);
    glTexCoord2f(0,1); glVertex3d(-1, 1, -1);
    glTexCoord2f(0,0); glVertex3d(-1, -1, -1);
    glTexCoord2f(1,0); glVertex3d(-1, -1, 1);
    glEnd();
    // right
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(1,1,0);
    glTexCoord2f(1,1); glVertex3d(1, 1, 1);
    glTexCoord2f(0,1); glVertex3d(1, 1, -1);
    glTexCoord2f(0,0); glVertex3d(1, -1, -1);
    glTexCoord2f(1,0); glVertex3d(1, -1, 1);
    glEnd();

    // front
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3d(-1, -1, 1);
    glTexCoord2f(0,1); glVertex3d(-1, 1, 1);
    glTexCoord2f(1,1); glVertex3d(1, 1, 1);
    glTexCoord2f(1,0); glVertex3d(1, -1, 1);
    glEnd();

    // back
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3d(-1, -1, -1);
    glTexCoord2f(0,1); glVertex3d(-1, 1, -1);
    glTexCoord2f(1,1); glVertex3d(1, 1, -1);
    glTexCoord2f(1,0); glVertex3d(1, -1, -1);
    glEnd();

    // let's do a roof

    // front
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, +1);
    glTexCoord2f(0,0); glVertex3d(-1.2, 1, 1.2);
    glTexCoord2f(1,0); glVertex3d(1.2, 1, 1.2);
    glTexCoord2f(0.5,1); glVertex3d(0, 2.2, 0);
    glEnd();

    // back
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0,0); glVertex3d(-1.2, 1, -1.2);
    glTexCoord2f(1,0); glVertex3d(1.2, 1, -1.2);
    glTexCoord2f(0.5,1); glVertex3d(0, 2.2, 0);
    glEnd();

    // left
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(-1, 1, 0);
    glTexCoord2f(1,0); glVertex3d(-1.2, 1, 1.2);
    glTexCoord2f(0,0); glVertex3d(-1.2, 1, -1.2);
    glTexCoord2f(0.5,1); glVertex3d(0, 2.2, 0);
    glEnd();

    // right
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(1, 1, 0);
    glTexCoord2f(1,0); glVertex3d(1.2, 1, 1.2);
    glTexCoord2f(0,0); glVertex3d(1.2, 1, -1.2);
    glTexCoord2f(0.5,1); glVertex3d(0, 2.2, 0);
    glEnd();

    // need a door

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
    glColor3ub(105, 82, 0);
    glNormal3f(0,1,1);
    glTexCoord2f(0,0); glVertex3d(-.9, -1, 1.01);
    glTexCoord2f(1,0); glVertex3d(-.2, -1, 1.01);
    glTexCoord2f(1,1); glVertex3d(-0.2, 0.2, 1.01);
    glTexCoord2f(0,1); glVertex3d(-.9, 0.2, 1.01);
    glEnd();
    // rear entrance for luxury estate

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
    glColor3ub(105, 82, 0);
    glNormal3f(0,1,-1);
    glTexCoord2f(0,0); glVertex3d(-.9, -1, -1.01);
    glTexCoord2f(1,0); glVertex3d(-.2, -1, -1.01);
    glTexCoord2f(1,1); glVertex3d(-0.2, 0.2, -1.01);
    glTexCoord2f(0,1); glVertex3d(-.9, 0.2, -1.01);
    glEnd();

    // need windows for fire code sake
    // front window
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glNormal3f(0,1,1);
    glColor3ub(100, 100, 100);
    glTexCoord2f(0,1); glVertex3d(.25, .2, 1.01);
    glTexCoord2f(0,0); glVertex3d(.25, -0.25, 1.01);
    glTexCoord2f(1,0); glVertex3d(.8, -0.25, 1.01);
    glTexCoord2f(1,1); glVertex3d(.8, .2, 1.01);
    glEnd();

    // two left windows
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glNormal3f(-1,1,0);
    glTexCoord2f(1,1); glVertex3d(-1.01, .2, -.25);
    glTexCoord2f(1,0); glVertex3d(-1.01, -0.25, -.25);
    glTexCoord2f(0,0); glVertex3d(-1.01, -0.25, -.8);
    glTexCoord2f(0,1); glVertex3d(-1.01, .2, -.8);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,1); glVertex3d(-1.01, .2, .25);
    glTexCoord2f(0,0); glVertex3d(-1.01, -0.25, .25);
    glTexCoord2f(1,0); glVertex3d(-1.01, -0.25, .8);
    glTexCoord2f(1,1); glVertex3d(-1.01, .2, .8);
    glEnd();

    // two right windows
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glNormal3f(1,1,0);
    glTexCoord2f(1,1); glVertex3d(1.01, .2, -.25);
    glTexCoord2f(1,0); glVertex3d(1.01, -0.25, -.25);
    glTexCoord2f(0,0); glVertex3d(1.01, -0.25, -.8);
    glTexCoord2f(0,1); glVertex3d(1.01, .2, -.8);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glTexCoord2f(0,1); glVertex3d(1.01, .2, .25);
    glTexCoord2f(0,0); glVertex3d(1.01, -0.25, .25);
    glTexCoord2f(1,0); glVertex3d(1.01, -0.25, .8);
    glTexCoord2f(1,1); glVertex3d(1.01, .2, .8);
    glEnd();
    glPopMatrix();
}

/* build super cool A-Frame homes */
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

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
     // foundation
    glBegin(GL_QUADS);
    glColor3ub(194, 114, 35);
    glVertex3d(-1, -1, +1); //front left
    glVertex3d(-1, -1, -1); // back left
    glVertex3d(+1, -1, -1); // back right
    glVertex3d(+1, -1, +1); // front right
    glEnd();

    // front
    glColor3ub(204, 41, 0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_TRIANGLES);
    glColor3ub(194, 114, 35);
    glNormal3f(0,0,1);
    glTexCoord2f(1,0); glVertex3d(+2, -1, +1);
    glTexCoord2f(0,0); glVertex3d(-2, -1, 1);
    glTexCoord2f(0.5,1); glVertex3d(0, 2, 1);
    glEnd();


    // back
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,-1);
    glTexCoord2f(1,0); glVertex3d(+2, -1, -1);
    glTexCoord2f(0,0); glVertex3d(-2, -1, -1);
    glTexCoord2f(0.5,1); glVertex3d(0, 2, -1);
    glEnd();

    // right side
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glColor3ub(125, 90, 0);
    glNormal3f(-1,-1,0);
    glTexCoord2f(0,0); glVertex3d(-2, -1, -1);
    glTexCoord2f(1,0); glVertex3d(-2, -1, 1);
    glTexCoord2f(1,1); glVertex3d(0, 2, 1);
    glTexCoord2f(0,1); glVertex3d(0, 2, -1);
    glEnd();

    // left side
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glColor3ub(125, 90, 0);
    glNormal3f(1,1,0);
    glTexCoord2f(0,0); glVertex3d(2, -1, -1);
    glTexCoord2f(1,0); glVertex3d(2, -1, 1);
    glTexCoord2f(1,1); glVertex3d(0, 2, 1);
    glTexCoord2f(0,1); glVertex3d(0, 2, -1);
    glEnd();

    // door
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBegin(GL_QUADS);
    glColor3ub(105, 82, 0);
    glNormal3f(0,1,1);
    glTexCoord2f(0,0); glVertex3d(-.3, -1, 1.01);
    glTexCoord2f(1,0); glVertex3d(.3, -1, 1.01);
    glTexCoord2f(1,1); glVertex3d(.3, 0.2, 1.01);
    glTexCoord2f(0,1); glVertex3d(-.3, 0.2, 1.01);
    glEnd();
    // front window

    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0,1,1);
    glTexCoord2f(1,0); glVertex3d(.3, .5, 1.01);
    glTexCoord2f(1,1); glVertex3d(.3, 1, 1.01);
    glTexCoord2f(0,1); glVertex3d(-.3, 1, 1.01);
    glTexCoord2f(0,0); glVertex3d(-.3, .5, 1.01);
    glEnd();

    // back top window
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0,1,-1);
    glTexCoord2f(1,0); glVertex3d(.3, .5, -1.01);
    glTexCoord2f(1,1); glVertex3d(.3, 1, -1.01);
    glTexCoord2f(0,1); glVertex3d(-.3, 1, -1.01);
    glTexCoord2f(0,0); glVertex3d(-.3, .5, -1.01);
    glEnd();

    // back bottom window
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0,1,-1);
    glTexCoord2f(0,1); glVertex3d(.2, 0.1, -1.01);
    glTexCoord2f(0,0); glVertex3d(.2, -0.4, -1.01);
    glTexCoord2f(1,0); glVertex3d(.9, -0.4, -1.01);
    glTexCoord2f(1,1); glVertex3d(.9, 0.1, -1.01);
    glEnd();

    // back bottom window #2
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0,1,-1);
    glTexCoord2f(1,1); glVertex3d(-.2, 0.1, -1.01);
    glTexCoord2f(1,0); glVertex3d(-.2, -0.4, -1.01);
    glTexCoord2f(0,0); glVertex3d(-.9, -0.4, -1.01);
    glTexCoord2f(0,1); glVertex3d(-.9, 0.1, -1.01);
    glEnd();

    glPopMatrix();
}

// just a big square at ground level
static void plantGrass()
{
    glPushMatrix();
    // Texture stuff
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glColor3ub(78, 245, 66);
    glTexCoord2f(0,1); glVertex3d(-20, -1.01, 20);
    glTexCoord2f(0,0); glVertex3d(-20, -1.01, -20);
    glTexCoord2f(1,0); glVertex3d(20, -1.01, -20);
    glTexCoord2f(1,1); glVertex3d(20, -1.01, 20);
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

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // to draw dashed lines for lanes
    glLineStipple(1, 0x00FF);
    glEnable(GL_LINE_STIPPLE);
    glLineWidth(2);
    glBegin(GL_LINES);
    

    glColor3ub(255, 255, 0);

    // // draw lines
    glNormal3f(0,1,0);
    glVertex3d(-20, -1.001, 0);
    glVertex3d(20, -1.001, 0);
    glVertex3d(0, -1.001, 20);
    glVertex3d(0, -1.001, -20);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_QUADS);
    glColor3ub(55, 55, 55);

    // East&West
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3d(-20, -1.0095, -1);
    glTexCoord2f(0,1); glVertex3d(-20, -1.0095, 1);
    glTexCoord2f(1,1); glVertex3d(20, -1.0095, 1);
    glTexCoord2f(1,0); glVertex3d(20, -1.0095, -1);
    glEnd();

    // North&South
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glBegin(GL_QUADS);
    glColor3ub(55, 55, 55);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3d(-1, -1.0095, -20);
    glTexCoord2f(0,1); glVertex3d(-1, -1.0095, 20);
    glTexCoord2f(1,1); glVertex3d(1, -1.0095, 20);
    glTexCoord2f(1,0); glVertex3d(1, -1.0095, -20);
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
    // select flat or smooth shading
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);


    /* pulled from example 13 */
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

    aFrame(-4, 0, 4, 1, 1, 1, 180);
    house(4, 0, 4, 2, 1, 2, 180);

    aFrame(4, 0, -4, 1, 1, 2, 0);
    house(-4, 0, -4, 1, 1, 1, 0);

    // roads
    plotRoads();

    // grass it up
    plantGrass();

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
    if (key == GLUT_KEY_UP && ph < 180)
    {
        ph += 5;
    }
    else if (key == GLUT_KEY_DOWN && ph > 5)
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
    }
    else if (ch == 'l' || ch == 'L')
    {
        light = 1 - light;
    }
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
    // glutIdleFunc(move?idle:NULL);
    //  Reproject
    Project(proj_mode ? fov : 0, asp, dim);
    //  Animate if requested

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
    glutInitWindowSize(600, 520);
    glutCreateWindow("Warren Payne - Homework 6");

    // call functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    // loading textures

    // http://cdinfinity.co/reclaimed-wood-texture-seamless/
    texture[0] = LoadTexBMP("panel1.bmp");
    // http://cdinfinity.co/reclaimed-wood-texture-seamless/
    texture[1] = LoadTexBMP("panel2.bmp");
    // from https://www.mytinyworld.co.uk/dollshouse/miniaturedetail.asp?prodid=9835&prodtype=Dolls-Houses-Handmade-Red-Brick-With-Black-Mortar-Dolls-House-Cladding
    texture[2] = LoadTexBMP("brick.bmp");
    // http://cdinfinity.co/reclaimed-wood-texture-seamless/
    texture[3] = LoadTexBMP("panel3.bmp");
    // https://www.pngguru.com/free-transparent-background-png-clipart-bjkan
    texture[4] = LoadTexBMP("door2.bmp");
    // https://www.pngguru.com/free-transparent-background-png-clipart-bgjqr
    texture[5] = LoadTexBMP("door.bmp");
    // https://www.lowes.com/pd/Owens-Corning-Oakridge-32-8-sq-ft-Artisan-Twilight-Black-Algae-Resistant-Laminated-Architectural-Roof-Shingles/4513946
    texture[6] = LoadTexBMP("roof.bmp");
    // https://www.pellawestspringfield.com/products/styles/single-hung-windows
    texture[7] = LoadTexBMP("window.bmp");
    // https://www.textures.com/browse/grass/238
    texture[8] = LoadTexBMP("grass.bmp");
    // https://www.textures.com/browse/floors/31872
    texture[9] = LoadTexBMP("concrete.bmp");
    ErrCheck("init");
    // main loop
    glutMainLoop();
    return 0;
}