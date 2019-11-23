
/* 
 * Flight Simulator - Warren Payne
*/

/* references
    * https://cs.lmu.edu/~ray/notes/flightsimulator/
*/

/*
    Want this done by Monday night
   * Steps:
   * 1. get movement commands down (up, down, left, right, azimuth, AND escape, toggles, etc..);
   *    let's get mouse movement done for vertical movement too
   * 2. get more advanced texturing set up (for mountains, sky, and sea)
   * 3. get lighting squared away (we may be very close)
   * 4. implement stall and barrel roll (stretch goals)
*/

#include "CSCIx229.h"
#define PI 3.14159265f
#define GRID_SIZE 1000.0f
#define NUM_MOUNTAINS 75
#include <time.h>
int windowWidth =  850;
int windowHeight = 600;

// Set up light colors
float diffuse[] = {1.0, 1.0, 1.0, 1.0};
float ambient[] = {1.0, 1.0, 1.0, 1.0};
float specular[] = {1.0, 1.0, 1.0, 1.0};

// Set global ambient
float globalAmbient[] = {0.05, 0.05, 0.05, 1.0};

// Display List
float thePlane = 0;

int imageWidthSea, imageHeightSea, imageWidthSky, imageHeightSky, imageWidthMountain, imageHeightMountain;
// Pointers to store the texture data
GLubyte *imageDataSea;
GLubyte *imageDataSky;
GLubyte *imageDataMountain;


typedef float color4[4];
color4 yellow = {1.0, 1.0, 0.0, 1.0};
color4 black = {0.0, 0.0, 0.0, 1.0};
color4 lightPurple = {0.87, 0.58, 0.98, 1.0};
color4 blue = {0.0, 0.0, 1.0, 1.0};
color4 red = {1.0, 0.0, 0.0, 1.0};
color4 green = {0.0, 1.0, 0.0, 1.0};
color4 white = {1.0, 1.0, 1.0, 1.0};
color4 grey = {0.05, 0.05, 0.05, 1.0};
color4 seaBlue = {0.0, 0.3, 0.8, 1.0};
color4 orange = {1.0, 0.5, 0.0, 1.0};
color4 clear = {1.0, 1.0, 1.0, 0.0};

// Defines an x,y,z point
typedef float point3[3];

point3 planeVertices[6763];
// This is an array of all normals for the plane
point3 planeNormals[6763];

// This is an array of all the vertices for the plane
point3 propVertices[6763];
// This is an array of all normals for the plane
point3 propNormals[6763];

// Keep track of current camera position and set the default
float cameraPosition[] = {0, 0, 0, 0, 0, 0};

// Set light position
float lightPosition[] = {0.0, 60.0, 0.0, 1.0};

// Set initial plane position
float planePosition[] = {0, 2.0, 10.0};

float turnAngle = 0.0;
float planeSpeed = 0.05;

// Array of cones for mountains
GLUquadricObj* quadricCone[NUM_MOUNTAINS];
int randHeightList[NUM_MOUNTAINS];
int baseWidthList[NUM_MOUNTAINS];
int randXList[NUM_MOUNTAINS];
int randZList[NUM_MOUNTAINS];

// texure stuff
float seaTextureID;
float skyTextureID;
float mountainTextureID;


float propInterp = 0.0f;

float upTilt = 0.0;
// Get the amount to tilt the plane
float sideTilt = 0.0;

// Set the turnspeed to 0
float turnSpeed = 0.0;

// Global mouse position of x
float mouseX = 0.0;

// Gets the ratio of the tilt from 0 - 1 of the maximum allowed tilt on the plane
float ratioOfRoll = 0.0;
float ratioOfPitch = 0.0;

// Maximum distance from center mouse can move (this is also middle of screen)
float maxMouseTiltx = 0.0;
float maxMouseTilty = 0.0;


// This is the the roll interp when doing a roll trick
float rollAmount = 0.0;

// Roll height to interp too
float rollHeight = 0.0;

int isFullScreen = 0;
// Wire rendering on by default
int isWireRendering = 1;
// Sea ad sky enabled
int isSeaAndSky = 0;

// If roll is enabled
int rollEnabled = 0;
// Crazy roll
int crazyRollEnabled = 0;
// Mountain textures on or off
int mountainTextureEnabled = 0;

// Toggles for directions key pressed and not pressed
int upPressed = 0;
int downPressed = 0;
int forwardPressed = 0;
int backwardPressed = 0;

// Set up quadric objects
GLUquadricObj* quadricCylinder;
GLUquadricObj* quadricDisk;

// Set up display list for propeller
float theProp = 0;

    int up = 0;
    int down = 0;
    int counter = 0;

/* -----------  Plane  ------------- */



void drawProps() {
	// Draw first propeller (left)
	glPushMatrix();
		// Position it in front of plane
		glTranslatef(-0.35, -0.1, -0.05);
		// Rotate so it is facing away
		glRotatef(-90, 0.0f, 1.0f, 0.0f);
		// Rotate propeller
		glRotatef(propInterp*360, 1.0f, 0.0f, 0.0f);
		// Translate to origin
		glTranslatef(0, 0.15f, -0.35f);

		// Draw propeller
		glCallList(theProp);
	glPopMatrix();

	// Draw second propeller (right)
	glPushMatrix();
		// Position it in front of plane
		glTranslatef(+0.35, -0.1, -0.05);
		// Rotate so it is facing away
		glRotatef(-90, 0.0f, 1.0f, 0.0f);
		// Rotate propeller
		glRotatef(propInterp*360, 1.0f, 0.0f, 0.0f);
		// Translate to origin
		glTranslatef(0, 0.15f, -0.35f);

		// Draw propeller
		glCallList(theProp);
	glPopMatrix();
}

void idle(){

    // regarding the propellors
    if(propInterp >= 1.0) {
		propInterp = 0;
	} else {
		propInterp += 0.05;
	}

    if(up){
        planePosition[1] -= .05;
    }
    else if(down){
        planePosition[1] += .05;
    }

	// // Plane trick interp
	// if(rollEnabled || crazyRollEnabled) {
	// 	if(rollAmount >= 360) {
	// 		// Resets all roll variables if hits 360
	// 		rollEnabled = 0;
	// 		crazyRollEnabled = 0;
	// 		rollAmount = 0;
	// 		rollHeight = 0;
	// 	} else {
	// 		// Increases the height to start the roll
	// 		if(rollHeight <= 1.0) {
	// 			rollHeight += 0.1;
	// 		} else {
	// 			// Increases the roll angle
	// 			rollAmount += 4;
	// 		}
	// 	}
	// }

	// Make sure turn speed starts at 0
	turnSpeed = 0.0;

	// Calculate the rotation speed of turning to be double the ratio of
	// the tilt
	turnSpeed += ratioOfRoll * 2;

	// Increase the angle of turning by the turn speed
	turnAngle += turnSpeed;

	// Reset turn angle if goes over 360
	if(turnAngle > 360) {
		turnAngle = 0;
	}

	// Force a redraw in OpenGL
	glutPostRedisplay();
}

void moveAllPlane() {
		// Move the plane to planes position
		glTranslatef(planePosition[0], planePosition[1], planePosition[2]);

		// Check if we should rotate a certain way depending on where the plane is moving

		// Always check for the planes tilt and rotation
		// Yaw
		glRotatef(-turnAngle, 0.0f, 1.0f, 0.0f);
		// Roll
		glRotatef(-sideTilt, 0.0f, 0.0f, 1.0f);

        glRotatef(-upTilt, 1.0f, 0.0f, 0.0f);


		// 		// Plane goes up
		// if(upPressed) {
		// 	// Update camera position and plane position
		// 	planePosition[1] += 0.05;

		// 	// Add tilt to plane
		// 	glRotatef(8, 1.0f, 0.0f, 0.0f);
		// }

		// // Plane goes down
		// if(downPressed) {
		// 	// Update camera position and plane position
		// 	planePosition[1] -= 0.05;

		// 	// Add tilt to plane
		// 	glRotatef(-8, 1.0f, 0.0f, 0.0f);
		// }

		// // Plane goes faster
		// if(forwardPressed) {
		// 	// Make plane speed faster
		// 	planeSpeed += 0.005;

		// 	// Add tilt to plane
		// 	glRotatef(-5, 1.0f, 0.0f, 0.0f);
		// }

		// // Plane goes slower
		// if(backwardPressed) {
		// 	// Make plane speed slower
		// 	// Limit how slow you can go
		// 	if(planeSpeed >= 0.05) {
		// 		planeSpeed -= 0.005;
		// 	}

		// 	// Add tilt to plane
		// 	glRotatef(5, 1.0f, 0.0f, 0.0f);
		// }
}

void drawPlane() {

	// Draw plane
	glPushMatrix();
		// Move plane and propellers depending on buttons pressed and
		// mouse position
		moveAllPlane();

		// Draw propellers
		drawProps();

		// Rotate the ship so it is facing away
		glRotatef(-90, 0.0f, 1.0f, 0.0f);

		// Draw the plane from display list
		glCallList(thePlane);
	glPopMatrix();
}

void mousePosition(int x, int y) {
	// Where the mouse is

	int left = 0;
	int right = 0;
	int center = 0;
	int	distanceFromCenter = 0;

	/* Mouse movement will effect the yaw/roll/pitch
       regardless of where it is on screen */
	// maxMouseTiltx = windowWidth/2.0;
    // maxMouseTilty = windowHeight/2.0;


    /* we want our roll and pitch to be calculated
     with respect to the center of the viewport */
    int m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );
    maxMouseTiltx = (m_viewport[0]+m_viewport[2])/2;
    maxMouseTilty = (m_viewport[1]+m_viewport[3])/2;
  



    
    // printf("x: %d, y: %d \n", x, y);
	// Check if going to left or right or center
	if(x == (int)maxMouseTiltx) {
        // printf("x: %d, maxMouseTiltx: %d\n", x, (int)maxMouseTiltx);
		left = 0;
		right = 0;
	} else if(x > (int)maxMouseTiltx && x < windowWidth) {
		center = 0;
		left = 0;
		right = 1;
	} else if(x < (int)maxMouseTiltx && x > 0) {
		center = 0;
		left = 1;
		right = 0;
	}

    // Calculate how far to move and tilt the plane
	if(left) {
		// Calculate percentage to the left and move
		distanceFromCenter = (int)maxMouseTiltx - x;
		// Calculate ratio
		ratioOfRoll = -distanceFromCenter/maxMouseTiltx;
	}
	else if(right) {
		// Calculate percentage to right and move
		distanceFromCenter = x - (int)maxMouseTiltx;
		// Calculate ratio
		ratioOfRoll = distanceFromCenter/maxMouseTiltx;
	}
    if(y == (int)maxMouseTilty) {
        up = 0;
        down = 0;
    }
    else if(y < (int)maxMouseTilty && y > 0){
        up = 0;
        down = 1;
    }
    else if(y > (int)maxMouseTilty) {
        up = 1;
        down = 0;
        
    }


	
    if(up){
   
        distanceFromCenter = y-((int)maxMouseTilty);
        ratioOfPitch = distanceFromCenter/maxMouseTilty;
        // planePosition[1] -= 0.05;
    }
    else if(down){
        distanceFromCenter = ((int)maxMouseTilty)-y;
        ratioOfPitch = -distanceFromCenter/maxMouseTilty;
        // planePosition[1] += 0.05;
    }
	// Translate the plane to the left or right depending on ratio, with max tilt being 45 degrees
	sideTilt = 45 * ratioOfRoll;
    upTilt = 10 * ratioOfPitch;
}


void setUpPlane(){
    int i = 0;
    int j = 0;
    int objectCount = -1;
    int isFace = 0;
    char firstChar;
    char *token;


    FILE *fileStream;

    char string[100];

    fileStream = fopen("plane.txt", "rt");

    if (fileStream != NULL){
        // OpenGL display list
        thePlane = glGenLists(1);
        glNewList(thePlane, GL_COMPILE);

        while(fgets(string, 100, fileStream) != NULL){
            firstChar = ' ';
            // Store plan vertices as we read in file
            if(sscanf(string, "v %f %f %f ", &propVertices[i][0], &propVertices[i][1], &propVertices[i][2]) == 3){
                i++;
            }
            // Store normals
            else if(sscanf(string, "n %f %f %f ", &propNormals[j][0], &propNormals[j][1], &propNormals[j][2]) == 3) {
                j++;
            }
            // in the case that it's a face
            else if(sscanf(string, "%c", &firstChar) == 1){
                if(firstChar == 'f'){
                    token = strtok(string, " ");
                    token = strtok(NULL, " ");

                    glBegin(GL_POLYGON);
                    glLineWidth(1);
                    while(token != NULL){
                        glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

                        if(objectCount <= 0){
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, orange);

                        }
                        else if(objectCount <=1){
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, red);
                        }
                        else{
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, yellow);
                        }
                        // Get normal and draw color
                        glNormal3f(propNormals[atoi(token)-1][0], propNormals[atoi(token)-1][1], propNormals[atoi(token)-1][2]);
                        glVertex3f(propVertices[atoi(token)-1][0], propVertices[atoi(token)-1][1], propVertices[atoi(token)-1][2]);
                        // Get next token
                        token = strtok(NULL, " ");
                    }
                    glEnd();
                }
                else if (firstChar == 'g'){
                    objectCount ++;
                }
            }
        }
        glEndList();
    }
    else{printf("No such file");}
    fclose(fileStream);
}

void setUpProp(){
    int i = 0;
    int j = 0;
    int objectCount = -1;
    int isFace = 0;
    char firstChar;
    char *token;


    FILE *fileStream;

    char string[100];

    fileStream = fopen("prop.txt", "rt");

    if (fileStream != NULL){
        // OpenGL display list
        theProp = glGenLists(1);
        glNewList(theProp, GL_COMPILE);

        while(fgets(string, 100, fileStream) != NULL){
            firstChar = ' ';
            // Store plan vertices as we read in file
            if(sscanf(string, "v %f %f %f ", &propVertices[i][0], &propVertices[i][1], &propVertices[i][2]) == 3){
                i++;
            }
            // Store normals
            else if(sscanf(string, "n %f %f %f ", &propNormals[j][0], &propNormals[j][1], &propNormals[j][2]) == 3) {
                j++;
            }
            // in the case that it's a face
            else if(sscanf(string, "%c", &firstChar) == 1){
                if(firstChar == 'f'){
                    token = strtok(string, " ");
                    token = strtok(NULL, " ");

                    glBegin(GL_POLYGON);
                    glLineWidth(1);
                    while(token != NULL){
                        glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

                        if(objectCount <= 0){
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, orange);

                        }
                        else if(objectCount <=1){
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, red);
                        }
                        else{
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, yellow);
                        }
                        // Get normal and draw color
                        glNormal3f(propNormals[atoi(token)-1][0], propNormals[atoi(token)-1][1], propNormals[atoi(token)-1][2]);
                        glVertex3f(propVertices[atoi(token)-1][0], propVertices[atoi(token)-1][1], propVertices[atoi(token)-1][2]);
                        // Get next token
                        token = strtok(NULL, " ");
                    }
                    glEnd();
                }
                else if (firstChar == 'g'){
                    objectCount ++;
                }
            }
        }
        glEndList();
    }
    fclose(fileStream);
}












/* ------------------ end plane stuff ------------------ */





void loadSea(){
    // image file
    FILE *fileID;

    int maxValue;
    int totalPixels;
    char tempChar;
    int i = 0;
    char headerLine[100];

    float RGBScaling;
    int red, green, blue;

    fileID = fopen("sea02.txt", "r");
    fscanf(fileID, "%[^\n] ", headerLine);

    if(headerLine[0]!= 'P'){
        exit(0);
    }
    printf("Loading textures...");

    fscanf(fileID, "%c", &tempChar);

    while(tempChar == '#'){
        fscanf(fileID, "%[^\n] ", headerLine);
        // read in the first char of the next line
        fscanf(fileID, "%c", &tempChar);
    }

    ungetc(tempChar, fileID);
    fscanf(fileID, "%d %d %d", &imageWidthSea, &imageHeightSea, &maxValue);
    totalPixels = imageWidthSea * imageHeightSea;

    imageDataSea = (GLubyte*)malloc(3* sizeof(float) * totalPixels);
    RGBScaling = 255.0 / maxValue;

    if (maxValue == 255)
	{
		for(i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID,"%d %d %d",&red, &green, &blue );
            // printf("%d %d %d \n", red, green, blue);

			// store the red, green and blue data of the current pixel in the data array
			imageDataSea[3*totalPixels - 3*i - 3] = red;
			imageDataSea[3*totalPixels - 3*i - 2] = green;
			imageDataSea[3*totalPixels - 3*i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for(i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID,"%d %d %d",&red, &green, &blue );

			// store the red, green and blue data of the current pixel in the data array
			imageDataSea[3*totalPixels - 3*i - 3] = red   * RGBScaling;
			imageDataSea[3*totalPixels - 3*i - 2] = green * RGBScaling;
			imageDataSea[3*totalPixels - 3*i - 1] = blue  * RGBScaling;
		}
	}

	// close the image file
	fclose(fileID);

}

void loadSky(){
    FILE *fileID;

    int maxValue;
    int totalPixels;
    char tempChar;
    int i = 0;
    char headerLine[100];

    float RGBScaling;
    int red, green, blue;

    fileID = fopen("sky.txt", "r");
    // fscanf(fileReadFrom, format, load into char buffer)
    fscanf(fileID, "%[^\n] ", headerLine);

    // Then we're in the wrong file
    if(headerLine[0]!= 'P'){
        exit(0);
    }
    printf("Loading textures...");

    fscanf(fileID, "%c", &tempChar);

    // reading for comment lines
    while(tempChar == '#'){
        fscanf(fileID, "%[^\n] ", headerLine);
        // read in the first char of the next line
        fscanf(fileID, "%c", &tempChar);
    }

    // last one wasn't a comment, cram it back into the input stream and let's do this!
    ungetc(tempChar, fileID);
    fscanf(fileID, "%d %d %d", &imageWidthSea, &imageHeightSea, &maxValue);
    totalPixels = imageWidthSea * imageHeightSea;

    imageDataSea = (GLubyte*)malloc(3* sizeof(float) * totalPixels);
    RGBScaling = 255.0 / maxValue;

    if (maxValue == 255)    // if 255, then we don't need to scale the image data values to be 0-255
	{
		for(i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID,"%d %d %d",&red, &green, &blue );
            // printf("%d %d %d \n", red, green, blue);

			// store the red, green and blue data of the current pixel in the data array
			imageDataSea[3*totalPixels - 3*i - 3] = red;
			imageDataSea[3*totalPixels - 3*i - 2] = green;
			imageDataSea[3*totalPixels - 3*i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for(i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID,"%d %d %d",&red, &green, &blue );

			// store the red, green and blue data of the current pixel in the data array
			imageDataSea[3*totalPixels - 3*i - 3] = red   * RGBScaling;
			imageDataSea[3*totalPixels - 3*i - 2] = green * RGBScaling;
			imageDataSea[3*totalPixels - 3*i - 1] = blue  * RGBScaling;
		}
	}

	// close the image file
	fclose(fileID);
}

void setUpLights(){
    glEnable(GL_LIGHTING);
    // GL_LIGHT0 = first of OpenGL's 8 light sources
    glEnable(GL_LIGHT0);
    // Enable smooth shading
    glShadeModel(GL_SMOOTH);
    // Enable normalization
    glEnable(GL_NORMALIZE);

    // Light color setup
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Global ambient
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);


    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
}



void setUpMountains(){
    srand(time(0));

    for (int i = 0; i < NUM_MOUNTAINS; i++){
        quadricCone[i] = gluNewQuadric();
        randHeightList[i] = (rand()%25)+2;
        // Generate a random base width
		baseWidthList[i] = (rand()%12)+2;

		// Generate a random x
		randXList[i] = (rand()%(300))-150;
		// Generate a random y
		randZList[i] = (rand()%(300))-150;
    }
}


void positionScene() {
	// Update plane position and account for the turn angle and plane speed
	planePosition[0] += sin(turnAngle * (PI/180.0f)) * planeSpeed;
	planePosition[2] -= cos(turnAngle * (PI/180.0f)) * planeSpeed;


	// Set up the camera position to trail behinde the plane
	// Based off the plane position
	cameraPosition[0] = planePosition[0] + sin(turnAngle * (PI/180.0f)) * -4;
	cameraPosition[1] = 1.2 + planePosition[1];
	cameraPosition[2] = planePosition[2] - cos(turnAngle * (PI/180.0f)) * -4;

	// Set where to look at (the plane)
	cameraPosition[3] = planePosition[0];
	cameraPosition[4] = planePosition[1];
	cameraPosition[5] = planePosition[2];
}

void setUpScene(void){
    glutReshapeWindow(windowWidth, windowHeight);
    glutPositionWindow(0, 0);
    glEnable(GL_DEPTH_TEST);


    // // set black background
    glClearColor(0, 0, 0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // gluPerspective(fovy, aspect, near, far)
    gluPerspective(90, windowWidth/windowHeight, 0.1, 40000);

    setUpLights();

    // Change into model-view mode so taht we can chagne object position
    glMatrixMode(GL_MODELVIEW);


    /* ended here ------------------------------------------------------*/

    setUpPlane();

    setUpProp();

    setUpMountains();

}

void drawGrid(){

    glPushMatrix();

    glRotatef(-45, 0.0f, 1.0f, 0.0f);
    glLineWidth(1);

    glTranslatef(GRID_SIZE/2, 0.0, -GRID_SIZE/2);

    for(int i = 0; i < GRID_SIZE; i++){ 
        // offset column drawn
        glTranslatef(-GRID_SIZE, 0.0, 0.0f);
        glTranslatef(0.0,0.0, 1.0);

        for(int j = 0; j < GRID_SIZE; j++){
            glTranslatef(1.0,0.0,0.0);
            // draw grd
            glBegin(GL_QUADS);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
            glMaterialfv(GL_FRONT, GL_AMBIENT, white);

            glNormal3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);

            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 1.0f);

            glNormal3f(1.0f, 0.0f, 1.0f);
            glVertex3f(1.0f, 0.0f, 1.0f);

            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glEnd();
        }
    }
    glPopMatrix();

    glPushMatrix();
		glRotatef(-45, 0.0f, 1.0f, 0.0f);
		// Move slightly above so it draws above grid lines
		glTranslatef(0.0, 0.05, 0.0);

		// Set line width to 5
		glLineWidth(5);

		// Draw the X direction
		glBegin(GL_LINES);
			// Color red
			glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
			glMaterialfv(GL_FRONT, GL_AMBIENT, red);
			glNormal3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glNormal3f(2.0f, 0.0f, 0.0f);
			glVertex3f(2.0f, 0.0f, 0.0f);
		glEnd();

		// Y direction
		glBegin(GL_LINES);
			// Color green
			glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
			glMaterialfv(GL_FRONT, GL_AMBIENT, green);
			glNormal3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glNormal3f(0.0f, 2.0f, 0.0f);
			glVertex3f(0.0f, 2.0f, 0.0f);
		glEnd();

		// Z direction
		glBegin(GL_LINES);
			// Color blue
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
			glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
			glNormal3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glNormal3f(0.0f, 0.0f, 2.0f);
			glVertex3f(0.0f, 0.0f, 2.0f);
		glEnd();

		// Draw circle in middle
		glMaterialfv(GL_FRONT, GL_DIFFUSE, grey);
		glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glutSolidSphere(0.2, 20, 20);
		glLineWidth(1);
	glPopMatrix();
}



void setUpTextures(){
    // texture name
    glGenTextures(1, &seaTextureID);

    // Bind the texture to it's id
    glBindTexture(GL_TEXTURE_2D, seaTextureID);
    // Set up texture with various settings to wrapping
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Build the mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidthSea, imageHeightSea, GL_RGB, GL_UNSIGNED_BYTE, imageDataSea);

	// Bind the for the sky
	glGenTextures(1, &skyTextureID);

	// Bind texture to id
	glBindTexture(GL_TEXTURE_2D, skyTextureID);

	// Set up texture with various settings to wrapping
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Build the mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidthSky, imageHeightSky, GL_RGB, GL_UNSIGNED_BYTE, imageDataSky);

	// Bind the for the mountain
	glGenTextures(1, &mountainTextureID);

	// Bind texture to id
	glBindTexture(GL_TEXTURE_2D, mountainTextureID);

	// Set up texture with various settings to wrapping
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Build the mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidthMountain, imageHeightMountain, GL_RGB, GL_UNSIGNED_BYTE, imageDataMountain);
}



void drawMountains() {
	int i = 0;

		// Set up texture for mountains
		glEnable(GL_TEXTURE_2D);
	
	// Draw all mountains
	for(i=0; i<NUM_MOUNTAINS;i++) {

		// Set up normals
		glShadeModel(GL_SMOOTH);

		gluQuadricDrawStyle(quadricCone[i], GLU_SMOOTH);

		// Set up textures
		gluQuadricTexture(quadricCone[i], GL_TRUE);

		// Set up the quadric normals
		gluQuadricNormals(quadricCone[i], GLU_SMOOTH);

		// Draw textures for mountain if enabled
		if(mountainTextureEnabled) {
			glBindTexture(GL_TEXTURE_2D, mountainTextureID);
			gluQuadricTexture(quadricCone[i], mountainTextureID);
		}

		// Draw cone for mountain
		glPushMatrix();
			glTranslatef(randXList[i], 0.0f, randZList[i]);
			glRotatef(-90, 1.0f, 0.0f, 0.0f);
			// Set line width
			glLineWidth(1);
			// Set the colors
			if(mountainTextureEnabled) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
			} else {
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
			}
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey);
			// Make it not as shiny as plane
			glMaterialf(GL_FRONT, GL_SHININESS, 200.0f);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blue);
			// Set the size (obj, inner, outer, height, slices, stacks)
			gluCylinder(quadricCone[i], baseWidthList[i], 0, randHeightList[i], 20, 20);
		glPopMatrix();
	}

	if(mountainTextureEnabled) {
		glDisable(GL_TEXTURE_2D);
	}

	// Reset color to blue
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
}




void drawSkyAndSea(){
    // Set up disk and cylinder
	quadricCylinder = gluNewQuadric();
	quadricDisk = gluNewQuadric();

	// Set up normals
	glShadeModel(GL_SMOOTH);

	gluQuadricDrawStyle(quadricDisk, GLU_SMOOTH);
	gluQuadricDrawStyle(quadricCylinder, GLU_SMOOTH);

	// Set up textures
	gluQuadricTexture(quadricCylinder, GL_TRUE);
	gluQuadricTexture(quadricDisk, GL_TRUE);

	// Set up the quadric normals
	gluQuadricNormals(quadricDisk, GLU_SMOOTH);
	gluQuadricNormals(quadricCylinder, GLU_SMOOTH);

	// Bind the texture to the quadric

	// Set up texture for disk base (sea)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTextureID);
	gluQuadricTexture(quadricCylinder, skyTextureID);

	// Draw cylinder
	glPushMatrix();
		// Set line width
		glLineWidth(1);
		// Rotate it to correct position
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		// Set the colors
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, orange);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey);
		// Set the size (obj, inner, outer, height, slices, stacks)
		gluCylinder(quadricCylinder, 200, 200, 100, 100, 100);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	// Set up texture for disk base (sea)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, seaTextureID);
	gluQuadricTexture(quadricDisk, seaTextureID);

	glPushMatrix();

		glLineWidth(1);
		// Move it to correct position
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		// Set up colors
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seaBlue);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey);
		// Set up the size (obj, inner, outer, slices, stacks)
		gluDisk(quadricDisk, 0, 201, 100, 100);
	glPopMatrix();

	// Disable the texture
	glDisable(GL_TEXTURE_2D);

	// Disable the fog after drawing the disk base

	drawMountains();
}

void normalKeys(unsigned char key, int x, int y) {
	switch(key) {
		case 's':
			// Set full screen to opposite
			isSeaAndSky = !isSeaAndSky;
			break;
		case 'r':
			// Rotate for normal roll
			rollEnabled = !rollEnabled;
			// Reset the roll amount each time
			rollAmount = 0.0f;
			break;
		case 'c':
			// Rotate for a crazy roll
			crazyRollEnabled = !crazyRollEnabled;
			// Reset the roll amount each time
			rollAmount = 0.0f;
			break;
		case 't':
			// Turn mountain textures on or off
			mountainTextureEnabled = !mountainTextureEnabled;
			break;
		// Quit the program gracefully
		case 'q':
			exit(0);
			break;
		default:
			break;
	}
}
void specialKeys(int key, int x, int y) {
		// Move up
		if(key == GLUT_KEY_F1) {
			// Set the key to be pressed
			upPressed = 1;
		}
		// Move down
		if(key == GLUT_KEY_F2) {
			// Set the key to be pressed
			downPressed = 1;
		}
		// Move forward
		if(key == GLUT_KEY_F3) {
			// Set the key to be pressed
			forwardPressed = 1;
		}
		// Move down
		if(key == GLUT_KEY_F4) {
			// Set the key to be pressed
			backwardPressed = 1;
		}
}



void myResize(int newWidth, int newHeight)
{

	// Update width of window
	windowWidth = newWidth;
	// Update height of window
	windowHeight = newHeight;

	// Update the viewport to still be all of the window
	glViewport (0, 0, windowWidth, windowHeight);

	// Change camera properties
    glMatrixMode(GL_PROJECTION);

	// Load identity matrix
    glLoadIdentity();

    // Modify the gluPerspective (fovy, aspect, near, far)
    gluPerspective(45, windowWidth/windowHeight, 0.1, 40000);

    // Back into modelview
	glMatrixMode(GL_MODELVIEW);
}


















void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    positionScene();
    // checkPitch();
    // Update plane position and account for the turn angle and plane speed
	planePosition[0] += sin(turnAngle * (PI/180.0f)) * planeSpeed;
	planePosition[2] -= cos(turnAngle * (PI/180.0f)) * planeSpeed;


	// Set up the camera position to trail behinde the plane
	// Based off the plane position
	cameraPosition[0] = planePosition[0] + sin(turnAngle * (PI/180.0f)) * -4;
	cameraPosition[1] = 1.2 + planePosition[1];
	cameraPosition[2] = planePosition[2] - cos(turnAngle * (PI/180.0f)) * -4;

	// Set where to look at (the plane)
	cameraPosition[3] = planePosition[0];
	cameraPosition[4] = planePosition[1];
	cameraPosition[5] = planePosition[2];

    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], cameraPosition[3], cameraPosition[4], cameraPosition[5], 0, 1, 0);

    // Set light position to whatever the lightPosition is
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPushMatrix();

    /* ----------- implement seaAndSky and drawPlane ---------------- */

    // drawGrid();

    drawSkyAndSea();
    glPopMatrix();
    
    glPushMatrix();
    drawPlane();

    glPopMatrix();
    glutSwapBuffers();

}

int main(int argc, char *argv[]){

    loadSea();
    loadSky();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(850, 600);
    glutCreateWindow("Warren Simulator");

    setUpScene();
    setUpTextures();
    glutIdleFunc(idle);

    // This handles keyboard input for normal keys
	glutKeyboardFunc(normalKeys);
    // This handles keyboard input for special keys
	glutSpecialFunc(specialKeys);

    glutPassiveMotionFunc(mousePosition);

    glutDisplayFunc(display);
    // ErrCheck("init");
    // Register the resize function
	glutReshapeFunc(myResize);
    
    glutMainLoop();
    return 0;
}