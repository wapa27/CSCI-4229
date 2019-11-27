#include "simulator.h"
/* 
 * Flight Simulator - Warren Payne
*/

/* references
    * https://cs.lmu.edu/~ray/notes/flightsimulator/
    * 
    * stole this guy's plane.txt and propellor.txt
    * for my display lists (hard to find display lists
    * out there):
    * https://github.com/DarknessItself/FlightSim/tree/c6030e2d423a3c8255cfeb21c37dba3cba15572f/kojo/FinalProject/FinalProject
*/

/*
    Want this done by Monday night
   * Steps:
   * 1. maybe toggle fog?
   * 2. get more advanced texturing set up (for mountains, sky, and sea) -- done
   * 3. get lighting squared away (we may be very close)    -- has an issue (horizon light source is
   *        an unknown origin)
   * 4. implement stall and barrel roll (stretch goals) -- nahhhh (figuring out how to
   *        create a crash would be too much for me at this point) -- done
   * 5. color plane - done
*/


/* Vertex, Triuangle, Quad are all related to the
   heightmap creation */
typedef struct Vertex
{
  float x, y, z;

  float vX, vY;
} Vertex;
// Triangles built from 3 vertices
typedef struct Triangle
{
  Vertex a, b, c;

  float color;
}Triangle;
// Quads built from 2 Triangles
typedef struct Quad
{
  Triangle a, b;
} Quad;

std::vector<Quad> HPQuads;

/* the png used is 1081 x 1081,
    created from http://terrain.party/
    This height map I pulled from SawTooth
    Range in Idaho */
stbi_uc HeightMap[1081][1081];

// got a lot of help in building the mountains
// done from a quad/triangle meshing algorithm
float Norm(Vertex *aV)
{
  return (aV->y + 20.f)/20.f;
}

void DrawV(Vertex *aV)
{
  glTexCoord2f(aV->vX, aV->vY);
  glVertex3d(aV->x, aV->y, aV->z);
}

void DrawT(Triangle *aT)
{
  glBegin(GL_TRIANGLES);
  DrawV(&aT->a);
  DrawV(&aT->b);
  DrawV(&aT->c);
  glEnd();
}

void DrawQ(Quad *aQ)
{
  glBindTexture(GL_TEXTURE_2D,map[0]);
  glEnable(GL_TEXTURE_2D);
  DrawT(&aQ->a);
  DrawT(&aQ->b);
  glDisable(GL_TEXTURE_2D);
}

Quad BuildQuad(Vertex *aA, Vertex *aB, Vertex *aC, Vertex *aD)
{
  Quad returnQuad;
  returnQuad.a.a = *aA;
  returnQuad.a.b = *aB;
  returnQuad.a.c = *aC;
  returnQuad.a.color = (Norm(aA)  + Norm(aB) + Norm(aC)) / 3.0f;

  returnQuad.b.a = *aA;
  returnQuad.b.b = *aC;
  returnQuad.b.c = *aD;
  returnQuad.b.color = (Norm(aA)  + Norm(aC) + Norm(aD)) / 3.0f;

  return returnQuad;
}



/* -----------  Plane  ------------- */



void drawProps() {
	// propeller-left
	glPushMatrix();
		// Position it in front of plane
		glTranslatef(-0.35, -0.1, -0.05);
		// make propeller face correct direction
		glRotatef(-90, 0.0f, 1.0f, 0.0f);
		// prop rotation
		glRotatef(propRotation*360, 1.0f, 0.0f, 0.0f);
		glTranslatef(0, 0.15f, -0.35f);
		// build th display list
		glCallList(props);
	glPopMatrix();

	// propeller-right
	glPushMatrix();
		glTranslatef(+0.35, -0.1, -0.05);
		glRotatef(-90, 0.0f, 1.0f, 0.0f);
		glRotatef(propRotation*360, 1.0f, 0.0f, 0.0f);
		glTranslatef(0, 0.15f, -0.35f);
		glCallList(props);
	glPopMatrix();
}

void idle(){

    // regarding the propellors
    if(propRotation >= 1.0) {
		propRotation = 0;
	} else {
		propRotation += 0.05;
	}
    // check if global "up" or "down" so we can resume going up or down
    if(up){
        planePosition[1] += upSpeed;
    }
    else if(down){
        planePosition[1] -= downSpeed;
    }
	turnSpeed = 0.0;
    // rotation speed of turning is double the Roll ratio
	turnSpeed += ratioOfRoll * 2;

	// angle of the turn should correspond to turn speed
	turnAngle += turnSpeed;

	// Reset turn angle if goes over 360
	if(turnAngle > 360) {
		turnAngle = 0;
	}

	glutPostRedisplay();
}

void movePlane() {
		// Move da plane
		glTranslatef(planePosition[0], planePosition[1], planePosition[2]);
		// Yaw
		glRotatef(-turnAngle, 0.0f, 1.0f, 0.0f);
		// Roll
		glRotatef(-sideTilt, 0.0f, 0.0f, 1.0f);
        // Pitch
        glRotatef(-upTilt, 1.0f, 0.0f, 0.0f);
}


/* should probably clean up this mess of a function */
void mousePosition(int x, int y) {
	int left = 0;
	int right = 0;
    // distance from center where center = center of viewport
	int	distanceFromCenter = 0;

    /* we want our roll and pitch to be calculated
     with respect to the center of the viewport */

    // get viewport
    int m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    /* set max x and y influence on tilt
       relative to the viewport size */
    maxMouseTiltx = (m_viewport[0]+m_viewport[2])/2;
    maxMouseTilty = (m_viewport[1]+m_viewport[3])/2;
    // keep track of the center of the viewport on y axis
    float trueCenter = m_viewport[3]/2;

    // is the plane going left, right, or neither?
	if(x == (int)maxMouseTiltx) {
		left = 0;
		right = 0;
	} else if(x > (int)maxMouseTiltx && x < windowWidth) {
		left = 0;
		right = 1;
	} else if(x < (int)maxMouseTiltx && x > 0) {
		left = 1;
		right = 0;
	}

    // calculate tilt and movement of plane
	if(left) {
		// calculate how far left the mouse is
		distanceFromCenter = (int)maxMouseTiltx - x;
		ratioOfRoll = -distanceFromCenter/maxMouseTiltx;
	}
	else if(right) {
		// calculate how far right the mouse is
		distanceFromCenter = x - (int)maxMouseTiltx;
		ratioOfRoll = distanceFromCenter/maxMouseTiltx;
	}
    // same for up and down
    if(y == (int)maxMouseTilty) {
        up = 0;
        down = 0;
    }
    else if(y < (int)maxMouseTilty && y > 0){
        up = 0;
        down = 1;
    }
    else if(y > (int)maxMouseTilty && y < int(maxMouseTilty*2)) {
        up = 1;
        down = 0;
    }
            
    if(up){
        distanceFromCenter = y-((int)maxMouseTilty);
        ratioOfPitch = distanceFromCenter/maxMouseTilty;
        // if y is within the viewport
        if(y >= (int)trueCenter && y <= (int)m_viewport[3]){
            // calculate up speed - it's finicky 
            upSpeed = 0.0005*(trueCenter-y);
        }
    }
    else if(down){
        distanceFromCenter = ((int)maxMouseTilty)-y;
        ratioOfPitch = -distanceFromCenter/maxMouseTilty;
        // make sure mouse is within viewport      
        if(y <= (int)trueCenter && y >= (int)m_viewport[1]){
            downSpeed = 0.0005*(y-trueCenter);
        }
    }
	// Calculate tilt to be translated, maximum 45 degrees sideways and 12.5 in pitch
	sideTilt = 45 * ratioOfRoll;
    upTilt = 12.5 * ratioOfPitch;
}


void CirrusToList(){
	int faceCount = -1;
	char firstChar;
	char *token;

    // for while loop
    int i = 0;
	int j = 0;
    

	// array to store
	char line[100];
    // set up list
	FILE * listFile;
	listFile = fopen("plane.txt", "rt");

	if (listFile != NULL)
	{
		// put into display list
		Cirrus = glGenLists(1);
	  	glNewList(Cirrus, GL_COMPILE);

		// read line by line, storing in line[]
		while(fgets(line, 100, listFile) != NULL)
		{
			firstChar = ' ';
			// if vertices
			if(sscanf(line, "v %f %f %f ", &CirrusVertices[i][0], &CirrusVertices[i][1], &CirrusVertices[i][2]) == 3) {
				// store those vertices
				// i for vertices
				i++;
			} else if(sscanf(line, "n %f %f %f ", &CirrusNormals[j][0], &CirrusNormals[j][1], &CirrusNormals[j][2]) == 3) {
				// j for normals
				j++;
			} else if(sscanf(line, "%c", &firstChar) == 1) { // Check if it is a face and tokenize it
				if(firstChar == 'f') { // face
                    // tokenize
					token = strtok(line, " ");
					token = strtok(NULL, " ");
                    glEnable(GL_LIGHT0);
					// Draw polygon for this face
					glBegin(GL_POLYGON);
						glLineWidth(1);
						while(token != NULL) {
							// Draw the normal and point
							glMaterialf(GL_FRONT, GL_SHININESS, 5.0f);

							// Color plane based on what object it is
							if(faceCount <= 3) { // fuselage
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
								glMaterialfv(GL_FRONT, GL_SPECULAR, lightBlue);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, lightBlue);
							} else if(faceCount <= 5) { // left windows
								glMaterialfv(GL_FRONT, GL_DIFFUSE, clear);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
							} else if(faceCount <= 6) {// front of wings
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
								glMaterialfv(GL_FRONT, GL_SPECULAR, lightBlue);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, lightBlue);
							} else if(faceCount <= 7) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, clear);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
							} else if(faceCount <= 10) { // emergency exit
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGrey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, lightGrey);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, lightGrey);
							} else if(faceCount <= 11) { //right windows
								glMaterialfv(GL_FRONT, GL_DIFFUSE, clear);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
							} else if(faceCount <= 13) { // rear exit
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGrey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, lightGrey);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, lightGrey);
							} else if(faceCount <= 25) { //rudder
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGrey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, lightGrey);
							} else if(faceCount <= 32) { // seems to be world lighting
								glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
								glMaterialfv(GL_FRONT, GL_AMBIENT, black);
								glMaterialfv(GL_FRONT, GL_SPECULAR, grey);
							} else {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                                glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
							}
                            // get normals/vertices and draw color
							glNormal3f(CirrusNormals[atoi(token)-1][0], CirrusNormals[atoi(token)-1][1], CirrusNormals[atoi(token)-1][2]);
							glVertex3f(CirrusVertices[atoi(token)-1][0], CirrusVertices[atoi(token)-1][1], CirrusVertices[atoi(token)-1][2]);
							// onto the next token
							token = strtok(NULL, " ");
						}
					glEnd();
                    glDisable(GL_LIGHT0);
				} else if (firstChar == 'g'){
					faceCount++;
				}
			}
		}
		// End the display list
		glEndList();
	}
	fclose (listFile);
}

void PropToList(){
    int faceCount = -1;
    char firstChar;
    char *token;

    // for the while loop
    int i = 0;
    int j = 0;
    char line[100];


    FILE *listFile;
    listFile = fopen("prop.txt", "rt");

    if (listFile != NULL){
        // make display list
        props = glGenLists(1);
        glNewList(props, GL_COMPILE);
        // iterage through heach line
        while(fgets(line, 100, listFile) != NULL){
            firstChar = ' ';
            // if vertices
            if(sscanf(line, "v %f %f %f ", &propVertices[i][0], &propVertices[i][1], &propVertices[i][2]) == 3){
                i++;
            }
            // if normals
            else if(sscanf(line, "n %f %f %f ", &propNormals[j][0], &propNormals[j][1], &propNormals[j][2]) == 3) {
                j++;
            }
            // if face
            else if(sscanf(line, "%c", &firstChar) == 1){
                if(firstChar == 'f'){
                    token = strtok(line, " ");
                    token = strtok(NULL, " ");
                    glBegin(GL_POLYGON);
                    glLineWidth(1);
                    while(token != NULL){
                        glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

                        if(faceCount <= 0){
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightBlue);
                        }
                        else if(faceCount <=1){
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, red);
                        }
                        else{
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, silver);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, silver);
                        }
                        // normals/ vertices draw colors
                        glNormal3f(propNormals[atoi(token)-1][0], propNormals[atoi(token)-1][1], propNormals[atoi(token)-1][2]);
                        glVertex3f(propVertices[atoi(token)-1][0], propVertices[atoi(token)-1][1], propVertices[atoi(token)-1][2]);
                        //  next token
                        token = strtok(NULL, " ");
                    }
                    glEnd();
                }
                else if (firstChar == 'g'){
                    faceCount ++;
                }
            }
        }
        glEndList();
    }
    fclose(listFile);
}


void drawPlane() {
	glPushMatrix();
		/* Move plane and propellers 
        depending on buttons pressed and 
        mouse position */
		movePlane();

		// Draw propellers
		drawProps();

		// make Cirrus face away from camera
		glRotatef(-90, 0.0f, 1.0f, 0.0f);

		// Draw the plane from display list
		glCallList(Cirrus);
	glPopMatrix();
}




/* ------------------ end plane stuff ------------------ */



void moon(){
    float tmpPosition[] = {300, 3, 0, .5};
    float Emission[]  = {.15,.15,.12,.5};
    float tmp [] = {155.0, 22.0, 1.0, .5};
   
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glEnable(GL_LIGHT0);
   //  Offset, scale and rotate
    glScaled(1,1,1);
    glTranslated(Fx,Fy,2200);

   //  White moon
    glColor3f(1,1,1);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    // fraw sphere
    glutSolidSphere(150.0,16,16);
    glEnable(GL_NORMALIZE);
    
    glLightfv(GL_LIGHT0, GL_EMISSION, tmp);
    glLightfv(GL_LIGHT0, GL_POSITION, tmpPosition);
    glPopMatrix();
    glDisable(GL_LIGHT0);
}

// updates the position of the plane and camera position
void positionScene() {
	// updates plane position (accounting for turn angle)
	planePosition[0] += sin(turnAngle * (PI/180.0f)) * planeSpeed;
	planePosition[2] -= cos(turnAngle * (PI/180.0f)) * planeSpeed;


	// cemera should follow planeposition
	cameraPosition[0] = planePosition[0] + sin(turnAngle * (PI/180.0f)) * -4;
	cameraPosition[1] = 1.2 + planePosition[1];
	cameraPosition[2] = planePosition[2] - cos(turnAngle * (PI/180.0f)) * -4;

	// Set where to look at (the plane)
	cameraPosition[3] = planePosition[0];
	cameraPosition[4] = planePosition[1];
	cameraPosition[5] = planePosition[2];
}

// builds moon and mountains baby
void Rockies(){
    // should be called MoonAndRockies()
    moon();
    // begin meshing algorithm
    for (auto& quad : HPQuads)
  {
    DrawQ(&quad);
  }
}


void init(void){
    /* set viewport dimensions */
    glutReshapeWindow(windowWidth, windowHeight);
    glutPositionWindow(0, 0);
    glEnable(GL_DEPTH_TEST);


    // Black Background
    glClearColor(0, 0, 0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // gluPerspective(fovy, aspect, near, far)
    gluPerspective(90, windowWidth/windowHeight, 0.1, 30000);

    glMatrixMode(GL_MODELVIEW);

    /* ended here nov 21------------------------------------------------------*/

    CirrusToList();

    PropToList();
}
// load rockies into a heightmap
void loadRockies(const char *filename) 
{

  int width,height,comp;
  stbi_uc *image = stbi_load(filename, &width, &height, &comp, STBI_grey);
  if (!image)
  {
      printf("Failed to load image in loadRockies");
  }
  memcpy(HeightMap, image, (1081 * 1081));
  stbi_image_free(image);
  return;
}
// example 25
void drawSkyBox(double D){

   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,sky[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,sky[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_2D);
}

// full implementation of meshing algorithm
void RockiesSetUp()
{
  // vector of Vertices
  std::vector<std::vector<Vertex>> set2d;
  
  // 12 because it's the most i can get away with without
  // messing up my performance
  const int tmp = 12;
  int picWidth  = 1081 / tmp;
  int picHeight = 1081 / tmp;
  set2d.resize(picHeight);

  for (auto &points : set2d)
  {
    points.resize(picWidth);
  }

  int correctSize = picWidth * picHeight;
  /* Iterate through each pixel in heightmap */
  float scalar = 3.0f;
  float z;
  Vertex v; 
  for (int i = tmp; i < 1081; i += tmp)
  {
    float x = (i - (1081 / 2))*scalar; //multipy by mesh scalar
    for (int j = tmp; j < 1081; j += tmp)
    {
      v.x = x;
      // moving plane down the y axis
      v.y = (HeightMap[i][j]/255.0f * 300) - 300.0f;
      z = (j - (1081 / 2))*scalar;
      v.z = z;
      v.vX  = i / 1081.0f;
      v.vY  = j / 1081.0f;
      set2d[(i / tmp) - 1][(j / tmp) - 1] = v;
      --correctSize;
    }
  }
  // build quads
  for (int i = 1; i < picHeight; i += 1)
  {
    for (int j = 1; j < picWidth; j += 1)
    {
      Quad thisIteration = 
        BuildQuad(&set2d[i-1][j+0], &set2d[i+0][j+0], &set2d[i+0][j-1], &set2d[i-1][j-1]);
      HPQuads.push_back(thisIteration);
    }
  }
}

void normalKeys(unsigned char ch, int x, int y) {
	switch(ch) {
        case 27:
            exit(0);
            break;
		case 'w':
            if(planeSpeed < 0.6){
                planeSpeed += .05;
                printf("plane speed: %f \n", planeSpeed);
                break;
            }
            else printf("This is the vechicle's top speed!\n");
        case 's':
            if(planeSpeed > 0.15){
                planeSpeed -= 0.05;
            }
            else printf("Flight instructor won't allow this thing to go any slower! \n");
		default:
			break;
	}
}

void resize(int newWidth, int newHeight)
{

	// Update widthxheight of window
	windowWidth = newWidth;
	windowHeight = newHeight;

	glViewport (0, 0, windowWidth, windowHeight);

	// Change camera properties
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    // Modify the gluPerspective (fovy, aspect, near, far)
    gluPerspective(45, windowWidth/windowHeight, 0.1, 40000);

    // Back into modelview
	glMatrixMode(GL_MODELVIEW);
}

void display(void){
    // glClearColor(0,0.2,0.7,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    positionScene();

    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
                cameraPosition[3], cameraPosition[4], cameraPosition[5], 0, 1, 0);

    glPushMatrix();

    /* ----------- draw mountains and drawPlane ---------------- */
    Rockies();
    glPopMatrix();
    glPushMatrix();
    drawSkyBox(dim*1000.5);
    glPopMatrix();
    
    glPushMatrix();
    drawPlane();
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

int main(int argc, char *argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(850, 600);

    glutCreateWindow("Final Project: Warren's Simulator");

    init();

    // normal key functions
	glutKeyboardFunc(normalKeys);
    // mouse logic
    glutPassiveMotionFunc(mousePosition);
    glutDisplayFunc(display);
	glutReshapeFunc(resize);
    glutIdleFunc(idle);

    loadRockies("ColoradoHM.png");
    RockiesSetUp();
    sky[0] = LoadTexBMP("sky0.bmp");
    sky[1] = LoadTexBMP("sky1.bmp");
    map[0] = LoadTexBMP("Colorado.bmp");
    glutMainLoop();
    return 0;
}