#ifndef simulator
#define simulator

#include "CSCIx229.h"
#define PI 3.14159265f
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <stdarg.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// size of world
double dim=3.0;

int windowWidth =  850;
int windowHeight = 600;

// Display List
float Cirrus = 0;

// Pointers to store the texture data

typedef float RGBA_Color[4];
RGBA_Color white = {.85, .85, .85, 0.1};
RGBA_Color grey = {0.05, 0.05, 0.05, 1.0};
RGBA_Color black = {0.0, 0.0, 0.0, 1.0};
RGBA_Color clear = {1.0, 1.0, 1.0, 0.0};
RGBA_Color blue = {0.0, 0.0, 1.0, 1.0};
RGBA_Color red = {1.0, 0.0, 0.0, 1.0};
RGBA_Color silver = {0.75, 0.75, 0.75, 1.0};
RGBA_Color lightGrey = {0.224, 0.225, 0.226, 0.58};
RGBA_Color lightBlue = {0.15, 0.404, 0.796, 1.0};
// Set up display list for propeller
float props = 0;

// put BMP's in these
int sky[2];
int map [2];
// Defines an x,y,z point
typedef float xyz[3];
xyz CirrusVertices[6763];
// This is an array of all normals for the plane
xyz CirrusNormals[6763];

// This is an array of all the vertices for the plane
xyz propVertices[6763];
// This is an array of all normals for the plane
xyz propNormals[6763];

// camera position
float cameraPosition[] = {0, 0, 0, 0, 0, 0};

// light position
/* --------------------- GIVING ME ISSUES! ----------------------- */
float lightPosition[] = {-2500.0, 3.0, 0.0, 1.0};

// Set initial plane position
float planePosition[] = {0, 20.0, 0.0};

// angle of our turn (yaw)
float turnAngle = 0.0;
float planeSpeed = 0.05;
// used to calculate rotation of props
float propRotation = 0.0f;

// tilt of plane (also down tilt) -- pitch
float upTilt = 0.0;
// side tilt (roll)
float sideTilt = 0.0;

// start at 0
float turnSpeed = 0.0;

/* need these globals since I want them to retain their
  value and be used at each loop cycle; otherwise, the plane
  would only go up or down if there is user input to incite
  mousePosition() */
int up = 0;
int down = 0;

/* used to calculate the speed along the y axis
   for up and down relative to the mouse location
*/
float upSpeed = 0;
float downSpeed = 0;

/* used in mousePosition() and idle() to calculate
   the the extent of the roll and pitch */
float ratioOfRoll = 0.0;
float ratioOfPitch = 0.0;

/* used to limit the extend of mouse influence on roll/pitch;
   i.e. I want to keep the extent limited to the viewport rather
   than the entire screen */
float maxMouseTiltx = 0.0;
float maxMouseTilty = 0.0;

// used later to experiment with light location
// used for GLtranslated
// could use array I suppose
float Fx = -2500;
float Fy = 3;   
float Fz = 0; 

#endif