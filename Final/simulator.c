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

/* -----------  Plane  ------------- */

void drawProps()
{
    // propeller-left
    glPushMatrix();
    // Position it in front of plane
    glTranslatef(-0.35, -0.1, -0.05);
    // make propeller face correct direction
    glRotatef(-90, 0.0f, 1.0f, 0.0f);
    // prop rotation
    if (!crash)
    {
        glRotatef(propRotation * 360, 1.0f, 0.0f, 0.0f);
    }
    glTranslatef(0, 0.15f, -0.35f);
    // build th display list
    glCallList(props);
    glPopMatrix();

    // propeller-right
    glPushMatrix();
    glTranslatef(+0.35, -0.1, -0.05);
    glRotatef(-90, 0.0f, 1.0f, 0.0f);
    if (!crash)
    {
        glRotatef(propRotation * 360, 1.0f, 0.0f, 0.0f);
    }

    glTranslatef(0, 0.15f, -0.35f);
    glCallList(props);
    glPopMatrix();
}

void idle()
{
    // regarding the propellors
    if (propRotation >= 1.0 && !isLanding)
    {
        propRotation = 0;
    }
    else
    {
        if (!isLanding)
        {
            propRotation += 0.04;
        }
        else if (isLanding && propRotation > 0)
        {
            if (propRotation < 20.6)
            {
                propRotation += reducePropSpeed * .05;
                reducePropSpeed -= .0005;
            }
            else
                propRotation += reducePropSpeed * .05;
        }
    }

    if (!crash)
    {
        savePlanePosition[1] = planePosition[1];
    }
    // check if global "up" or "down" so we can resume going up or down
    if (up && !isLanding && !isDeparting)
    {
        planePosition[1] += upSpeed;
    }
    else if (down && !isLanding && !isDeparting)
    {
        planePosition[1] -= downSpeed;
    }
    turnSpeed = 0.0;
    // rotation speed of turning is double the Roll ratio
    turnSpeed += ratioOfRoll * 2;

    // angle of the turn should correspond to turn speed
    turnAngle += turnSpeed;

    // Reset turn angle if goes over 360
    if (turnAngle > 360)
    {
        turnAngle = 0;
    }
    if (!crash && !isLanding)
    {
        savePlaneTurnAngle = turnAngle;
    }

    glutPostRedisplay();
}

void movePlane()
{
    // Move da plane
    glTranslatef(planePosition[0], planePosition[1], planePosition[2]);
    // if(!crash){
    // Yaw
    glRotatef(-turnAngle, 0.0f, 1.0f, 0.0f);
    // Roll
    glRotatef(-sideTilt, 0.0f, 0.0f, 1.0f);
    // Pitch
    glRotatef(-upTilt, 1.0f, 0.0f, 0.0f);
    glScaled(2, 2, 2);
    // }
}

/* loading in display list */
void CirrusToList()
{
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    int faceCount = -1;
    char firstChar;
    char *token;

    // for while loop
    int i = 0;
    int j = 0;

    // to store each line
    char line[100];
    // open plane.txt
    FILE *listFile;

    listFile = fopen("plane.txt", "rt");

    if (listFile != NULL)
    {
        // create list
        Cirrus = glGenLists(1);
        glNewList(Cirrus, GL_COMPILE);

        // store each line in line[]
        while (fgets(line, 100, listFile) != NULL)
        {
            firstChar = ' ';
            // if vertices
            if (sscanf(line, "v %f %f %f ", &CirrusVertices[i][0], &CirrusVertices[i][1], &CirrusVertices[i][2]) == 3)
            {
                i++;
            }
            else if (sscanf(line, "n %f %f %f ", &CirrusNormals[j][0], &CirrusNormals[j][1], &CirrusNormals[j][2]) == 3)
            {
                // j for normals
                j++;
            }
            else if (sscanf(line, "%c", &firstChar) == 1)
            {
                if (firstChar == 'f')
                {
                    // tokenize face
                    token = strtok(line, " ");
                    token = strtok(NULL, " ");
                    // use polygon for face
                    glBegin(GL_POLYGON);
                    glLineWidth(1);
                    while (token != NULL)
                    {
                        // Draw the normal and point
                        glMaterialf(GL_FRONT, GL_SHININESS, 5.0f);

                        // Color plane based on what object it is
                        if (faceCount <= 3)
                        { // fuselage
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, lightBlue);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightBlue);
                        }
                        else if (faceCount <= 5)
                        { // left windows
                            glColor3b(lightBlue[0], lightBlue[1], lightBlue[2]);
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, clear);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
                        }
                        else if (faceCount <= 6)
                        { // front of wings
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, lightBlue);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightBlue);
                        }
                        else if (faceCount <= 7)
                        {
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, clear);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
                        }
                        else if (faceCount <= 10)
                        { // emergency exit
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGrey);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, lightGrey);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightGrey);
                        }
                        else if (faceCount <= 11)
                        { //right windows
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, clear);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
                        }
                        else if (faceCount <= 13)
                        { // rear exit
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGrey);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, lightGrey);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightGrey);
                        }
                        else if (faceCount <= 25)
                        { //rudder
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightGrey);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightGrey);
                        }
                        else if (faceCount <= 32)
                        { // seems to be world lighting
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, white);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, grey);
                        }
                        else
                        {
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
                        }
                        // get normals/vertices and draw color
                        glNormal3f(CirrusNormals[atoi(token) - 1][0], CirrusNormals[atoi(token) - 1][1], CirrusNormals[atoi(token) - 1][2]);
                        glVertex3f(CirrusVertices[atoi(token) - 1][0], CirrusVertices[atoi(token) - 1][1], CirrusVertices[atoi(token) - 1][2]);
                        // onto the next token
                        token = strtok(NULL, " ");
                    }
                    glEnd();
                }
                else if (firstChar == 'g')
                {
                    faceCount++;
                }
            }
        }
        // End the display list
        glEndList();
    }
    fclose(listFile);
    glDisable(GL_TEXTURE_2D);
}

void PropToList()
{
    int faceCount = -1;
    char firstChar;
    char *token;

    // for the while loop
    int i = 0;
    int j = 0;
    char line[100];

    FILE *listFile;
    listFile = fopen("prop.txt", "rt");

    if (listFile != NULL)
    {
        // make display list
        props = glGenLists(1);
        glNewList(props, GL_COMPILE);
        // iterage through heach line
        while (fgets(line, 100, listFile) != NULL)
        {
            firstChar = ' ';
            // if vertices
            if (sscanf(line, "v %f %f %f ", &propVertices[i][0], &propVertices[i][1], &propVertices[i][2]) == 3)
            {
                i++;
            }
            // if normals
            else if (sscanf(line, "n %f %f %f ", &propNormals[j][0], &propNormals[j][1], &propNormals[j][2]) == 3)
            {
                j++;
            }
            // if face
            else if (sscanf(line, "%c", &firstChar) == 1)
            {
                if (firstChar == 'f')
                {
                    token = strtok(line, " ");
                    token = strtok(NULL, " ");
                    // glEnable(GL_LIGHTING);
                    // glEnable(GL_LIGHT1);
                    // glEnable(GL_LIGHT1);
                    glBegin(GL_POLYGON);
                    glLineWidth(1);
                    while (token != NULL)
                    {
                        glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

                        if (faceCount <= 0)
                        {
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, lightBlue);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, lightBlue);
                        }
                        else if (faceCount <= 1)
                        {
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, red);
                        }
                        else
                        {
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, silver);
                            glMaterialfv(GL_FRONT, GL_AMBIENT, silver);
                        }
                        // normals/ vertices draw colors
                        glNormal3f(propNormals[atoi(token) - 1][0], propNormals[atoi(token) - 1][1], propNormals[atoi(token) - 1][2]);
                        glVertex3f(propVertices[atoi(token) - 1][0], propVertices[atoi(token) - 1][1], propVertices[atoi(token) - 1][2]);
                        //  next token
                        token = strtok(NULL, " ");
                    }
                    glEnd();
                }
                else if (firstChar == 'g')
                {
                    faceCount++;
                }
            }
        }
        glEndList();
    }
    fclose(listFile);
}

void drawPlane()
{
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

/* this function calculates the movement of the plane based on the location of the mouse */
void mousePosition(int x, int y)
{
    int left = 0;
    int right = 0;
    // distance from center where center = center of viewport
    int distanceFromCenter = 0;

    /* we want our roll and pitch to be calculated
    with respect to the center of the viewport */

    // get viewport
    int m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    /* set max x and y influence on tilt
    relative to the viewport size */
    maxMouseTiltx = (m_viewport[0] + m_viewport[2]) / 2;
    maxMouseTilty = (m_viewport[1] + m_viewport[3]) / 2;
    // keep track of the center of the viewport on y axis
    float trueCenter = m_viewport[3] / 2;

    if (x == (int)maxMouseTiltx)
    {
        left = 0;
        right = 0;
    }
    else if (x > (int)maxMouseTiltx && x < windowWidth)
    {
        left = 0;
        right = 1;
    }
    else if (x < (int)maxMouseTiltx && x > 0)
    {
        left = 1;
        right = 0;
    }

    // calculate tilt and movement of plane
    if (left)
    {
        // calculate how far left the mouse is
        distanceFromCenter = (int)maxMouseTiltx - x;
        ratioOfRoll = -distanceFromCenter / maxMouseTiltx;
    }
    else if (right)
    {
        // calculate how far right the mouse is
        distanceFromCenter = x - (int)maxMouseTiltx;
        ratioOfRoll = distanceFromCenter / maxMouseTiltx;
    }
    // same for up and down
    if (y == (int)maxMouseTilty)
    {
        up = 0;
        down = 0;
    }
    else if (y < (int)maxMouseTilty && y > 0)
    {
        up = 0;
        down = 1;
    }
    else if (y > (int)maxMouseTilty && y < (int)(maxMouseTilty * 2))
    {
        up = 1;
        down = 0;
    }

    if (up)
    {
        distanceFromCenter = y - ((int)maxMouseTilty);
        ratioOfPitch = distanceFromCenter / maxMouseTilty;
        // if y is within the viewport
        if (y >= (int)trueCenter && y <= (int)m_viewport[3])
        {
            // calculate up speed - it's finicky
            upSpeed = 0.0005 * (trueCenter - y);
        }
    }
    else if (down)
    {
        distanceFromCenter = ((int)maxMouseTilty) - y;
        ratioOfPitch = -distanceFromCenter / maxMouseTilty;
        // make sure mouse is within viewport
        if (y <= (int)trueCenter && y >= (int)m_viewport[1])
        {
            downSpeed = 0.0005 * (y - trueCenter);
        }
    }
    // Calculate tilt to be translated, maximum 45 degrees sideways and 12.5 in pitch
    sideTilt = 45 * ratioOfRoll;
    upTilt = 12.5 * ratioOfPitch;
    if (upTilt > 14)
    {
        upTilt = 14.0;
    }
    else if (upTilt < -14)
    {
        upTilt = -14;
    }
    if (!crash)
    {

        savePlaneUpTilt = upTilt;

        savePlaneSideTilt = sideTilt;
    }
}

static void missleAttack()
{
    bombPosition[0] += (saveXChange);
    bombPosition[1] += up ? (saveUp) : (-saveDown);
    bombPosition[2] += (saveZChange);
    glTranslatef(bombPosition[0], bombPosition[1], (bombPosition[2]));
    glRotatef(-saveTAngle, 0.0f, 1.0f, 0.0f);
    // // Roll
    glRotatef(-saveSTilt, 0.0f, 0.0f, 1.0f);
    // // Pitch
    glRotatef(-saveUTilt, 1.0f, 0.0f, 0.0f);
    glutPostRedisplay();
}

void moveMissle()
{
    if (!fireMissle)
    {

        glTranslatef(planePosition[0], planePosition[1] + .1, (planePosition[2]));

        // Yaw
        glRotatef(-turnAngle, 0.0f, 1.0f, 0.0f);
        // // Roll
        glRotatef(-sideTilt, 0.0f, 0.0f, 1.0f);
        // // Pitch
        glRotatef(-upTilt, 1.0f, 0.0f, 0.0f);
        // keep bombPosition updated
        for (int i = 0; i < 3; i++)
        {
            bombPosition[i] = planePosition[i];
        }
        saveUp = upSpeed;
        saveDown = downSpeed;
        if (planeSpeed > .15)
        {
            saveXChange = XChange * 1.5;
            saveZChange = ZChange * 1.5;
        }
        // else if (planeSpeed > .1)
        // {
        //     saveXChange = XChange * 2.0;
        //     saveZChange = ZChange * 2.0;
        // }
        else
        {
            saveXChange = XChange * 2.0;
            saveZChange = ZChange * 2.0;
        }
        saveTAngle = turnAngle;
        saveSTilt = sideTilt;
        saveUTilt = upTilt;
    }
    else
    {
        // if((saveUp)<(upSpeed))
        missleAttack();
    }
}

static void drawMissle()
{
    double x, y, z, i;
    int j;
    double radius = .2;
    double height = 4;

    glPushMatrix();

    moveMissle();
    glScaled(.2, .2, .2);
    glRotated(-90, 1.0, 0.0, 0.0);

    glBegin(GL_TRIANGLE_STRIP);
    glColor3ub(25, 25, 25);
    for (j = 0; j <= 360; j++)
    {
        x = radius * Cos(j);
        y = height;
        z = radius * Sin(j);

        glNormal3d(Cos(j), 0, Sin(j));

        glVertex3d(x, y, z);
        glVertex3d(x, -y, z);
    }
    glEnd();

    // top of missle
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, height, 0.0);
    for (i = 0.0; i < 360; i += .15)
    {
        glVertex3d(radius * Cos(i), height + .5, radius * Sin(i));
        glNormal3f(0, 0, 1);
    }
    glEnd();

    // bottom of missle
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, -height, 0.0);

    for (i = 0.0; i < 360; i += .15)
    {
        glVertex3d(radius * Cos(i), -height - .1, radius * Sin(i));
        glColor3ub(225.0, 0, 0);
    }

    glEnd();
    // draw stabalizers
    glColor3ub(255, 255, 255);
    glBegin(GL_TRIANGLES);
    glVertex3d(radius * cos(360), -height, radius * Sin(360));
    glVertex3d(radius * cos(360), 1 - height, radius * Sin(360));
    glVertex3d(radius * cos(360), -height, radius * Sin(360) + 1);

    glVertex3d(radius * cos(360), -height, radius * Sin(360));
    glVertex3d(radius * cos(360), 1 - height, radius * Sin(360));
    glVertex3d(radius * cos(360) + 1, -height, radius * Sin(360));

    glVertex3d(radius * cos(360), -height, radius * Sin(360));
    glVertex3d(radius * cos(360), 1 - height, radius * Sin(360));
    glVertex3d(radius * cos(360), -height, radius * Sin(360) - 1);

    glVertex3d(radius * cos(360), -height, radius * Sin(360));
    glVertex3d(radius * cos(360), 1 - height, radius * Sin(360));
    glVertex3d(radius * cos(360) - 1, -height, radius * Sin(360));
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

void moon()
{

    float Emission[] = {0.1, 0.1, 0.1, 0.1};
    // float tmp [] = {155.0, 22.0, 1.0, .5};

    // glEnable(GL_LIGHTING);
    glPushMatrix();
    // glEnable(GL_LIGHT1);
    //  Offset, scale and rotate
    glTranslated(1900, -1800, 1200);

    //  White moon
    glColor3f(1, 1, 1);
    glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
    glutSolidSphere(50.0, 16, 16);
    // glEnable(GL_NORMALIZE);

    glPopMatrix();
}

/* landing, crashing, takeoff */
void positionScene()
{
    // land crash
    if (((planePosition[1] < (-1999.1) && planePosition[1] > -2000 && upTilt > 11 && !isDeparting) || (crash)) && (planePosition[0] < 200 && planePosition[0] > -200 && planePosition[2] < 200 && planePosition[2] > -200))
    {
        crash = 1;
        planePosition[0] = savePlanePosition[0];
        planePosition[1] = savePlanePosition[1];
        planePosition[2] = savePlanePosition[2];
        upTilt = savePlaneUpTilt;
        // printf("plane upTilt is : %d\n", (int)savePlaneUpTilt);
        sideTilt = savePlaneSideTilt;
        turnAngle = savePlaneTurnAngle;
    }
    // if above water crash
    else if ((planePosition[1] < -2000.5 && planePosition[1] > -2002 && upTilt > 11) || (crash && (planePosition[0] > 200 || planePosition[0] < -200 || planePosition[2] > 200 || planePosition[2] < -200)))
    {
        crash = 1;
        planePosition[0] = savePlanePosition[0];
        planePosition[1] = savePlanePosition[1];
        planePosition[2] = savePlanePosition[2];
        upTilt = savePlaneUpTilt;
        sideTilt = savePlaneSideTilt;
        turnAngle = savePlaneTurnAngle;
    }
    // commence landing sequence
    else if ((planePosition[1] < -1998 && planePosition[1] > -2004 && upTilt <= 11 && !isDeparting) || isLanding)
    {
        if (planeSpeed > 0.0f)
        {
            isLanding = 1;
            upTilt = 0;
            sideTilt = 0;
            turnAngle = savePlaneTurnAngle;
            if (!(planePosition[0] > 200 || planePosition[0] < -200 || planePosition[2] > 200 || planePosition[2] < -200)) // make sure still on land
            {
                planePosition[1] = savePlanePosition[1];
                planePosition[0] += Sin(turnAngle) * planeSpeed;
                planePosition[2] -= Cos(turnAngle) * planeSpeed;
                planeSpeed -= 0.0005;
                // printf("planeSpeed: %fl\n", (float)planeSpeed);
                // cemera
                cameraPosition[0] = planePosition[0] + Sin(turnAngle) * -7;
                cameraPosition[1] = 1.2 + planePosition[1];
                cameraPosition[2] = planePosition[2] - Cos(turnAngle) * -7;

                cameraPosition[3] = planePosition[0];
                cameraPosition[4] = planePosition[1];
                cameraPosition[5] = planePosition[2];

                if (planeSpeed <= 0.0)
                {
                    planeSpeed = 0.0f;
                    isStopped = 1;
                    turnAngle = savePlaneTurnAngle;
                }
            }
            else
            {
                crash = 1;
            }
        }
        else
        {
            // turnSpeed = 0;
            upTilt = 0;
            sideTilt = 0;
            turnAngle = savePlaneTurnAngle;
        }
    }
    else if (isDeparting) // commence takeoff sequence
    {
        if (planeSpeed < 0.15f) // still grounded
        {
            upTilt = 0;
            sideTilt = 0;
            turnAngle = savePlaneTurnAngle;
            planePosition[1] = savePlanePosition[1];
            planePosition[0] += Sin(turnAngle) * planeSpeed;
            planePosition[2] -= Cos(turnAngle) * planeSpeed;
            planeSpeed += 0.0005;
            // printf("planeSpeed: %fl\n", (float)planeSpeed);
            // cemera
            cameraPosition[0] = planePosition[0] + Sin(turnAngle) * -7;
            cameraPosition[1] = 1.2 + planePosition[1];
            cameraPosition[2] = planePosition[2] - Cos(turnAngle) * -7;

            // keep location of plane
            cameraPosition[3] = planePosition[0];
            cameraPosition[4] = planePosition[1];
            cameraPosition[5] = planePosition[2];
        }
        else // takeoff up to +5 off ground
        {
            upTilt = 0;
            sideTilt = 0;
            turnAngle = savePlaneTurnAngle;

            planePosition[1] += .02;
            planePosition[0] += Sin(turnAngle) * planeSpeed;
            planePosition[2] -= Cos(turnAngle) * planeSpeed;
            // cemera
            cameraPosition[0] = planePosition[0] + Sin(turnAngle) * -7;
            cameraPosition[1] = 1.2 + planePosition[1];
            cameraPosition[2] = planePosition[2] - Cos(turnAngle) * -7;

            // keep location of plane
            cameraPosition[3] = planePosition[0];
            cameraPosition[4] = planePosition[1];
            cameraPosition[5] = planePosition[2];
            if (planePosition[1] > -1995)
            {
                isDeparting = 0;
            }
        }
    }
    else
    {
        float oldPositionX = planePosition[0];
        float oldPositionZ = planePosition[2];
        planePosition[0] += Sin(turnAngle) * planeSpeed;
        planePosition[2] -= Cos(turnAngle) * planeSpeed;
        savePlanePosition[0] = planePosition[0];
        savePlanePosition[2] = planePosition[2];
        // keep change for missle trajectory
        XChange = planePosition[0] - oldPositionX;
        ZChange = planePosition[2] - oldPositionZ;

        // cemera
        cameraPosition[0] = planePosition[0] + Sin(turnAngle) * -7;
        cameraPosition[1] = 1.2 + planePosition[1];
        cameraPosition[2] = planePosition[2] - Cos(turnAngle) * -7;

        // keep location of plane
        cameraPosition[3] = planePosition[0];
        cameraPosition[4] = planePosition[1];
        cameraPosition[5] = planePosition[2];
    }
}

// example 25
void drawSkyBox(double D)
{
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);

    //  Sides
    glBindTexture(GL_TEXTURE_2D, sky[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.00, 0);
    glVertex3f(-D, -D, -D);
    glTexCoord2f(0.25, 0);
    glVertex3f(+D, -D, -D);
    glTexCoord2f(0.25, 1);
    glVertex3f(+D, +D, -D);
    glTexCoord2f(0.00, 1);
    glVertex3f(-D, +D, -D);

    glTexCoord2f(0.25, 0);
    glVertex3f(+D, -D, -D);
    glTexCoord2f(0.50, 0);
    glVertex3f(+D, -D, +D);
    glTexCoord2f(0.50, 1);
    glVertex3f(+D, +D, +D);
    glTexCoord2f(0.25, 1);
    glVertex3f(+D, +D, -D);

    glTexCoord2f(0.50, 0);
    glVertex3f(+D, -D, +D);
    glTexCoord2f(0.75, 0);
    glVertex3f(-D, -D, +D);
    glTexCoord2f(0.75, 1);
    glVertex3f(-D, +D, +D);
    glTexCoord2f(0.50, 1);
    glVertex3f(+D, +D, +D);

    glTexCoord2f(0.75, 0);
    glVertex3f(-D, -D, +D);
    glTexCoord2f(1.00, 0);
    glVertex3f(-D, -D, -D);
    glTexCoord2f(1.00, 1);
    glVertex3f(-D, +D, -D);
    glTexCoord2f(0.75, 1);
    glVertex3f(-D, +D, +D);
    glEnd();

    //  Top and bottom
    glBindTexture(GL_TEXTURE_2D, sky[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0);
    glVertex3f(+D, +D, -D);
    glTexCoord2f(0.5, 0);
    glVertex3f(+D, +D, +D);
    glTexCoord2f(0.5, 1);
    glVertex3f(-D, +D, +D);
    glTexCoord2f(0.0, 1);
    glVertex3f(-D, +D, -D);

    // glTexCoord2f(1.0, 1);
    // glVertex3f(-D, -D, +D);
    // glTexCoord2f(0.5, 1);
    // glVertex3f(+D, -D, +D);
    // glTexCoord2f(0.5, 0);
    // glVertex3f(+D, -D, -D);
    // glTexCoord2f(1.0, 0);
    // glVertex3f(-D, -D, -D);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

static void house(double x, double y, double z,
                  double dx, double dy, double dz,
                  double th)
{

    glEnable(GL_LIGHTING);

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
    glNormal3f(-2, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3d(-1, 1, 1);
    glTexCoord2f(0, 1);
    glVertex3d(-1, 1, -1);
    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, -1);
    glTexCoord2f(1, 0);
    glVertex3d(-1, -1, 1);
    glEnd();
    // right
    glColor3ub(192, 55, 5);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3d(1, 1, 1);
    glTexCoord2f(0, 1);
    glVertex3d(1, 1, -1);
    glTexCoord2f(0, 0);
    glVertex3d(1, -1, -1);
    glTexCoord2f(1, 0);
    glVertex3d(1, -1, 1);
    glEnd();

    // front
    glColor3ub(192, 55, 5);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, 1);
    glTexCoord2f(0, 1);
    glVertex3d(-1, 1, 1);
    glTexCoord2f(1, 1);
    glVertex3d(1, 1, 1);
    glTexCoord2f(1, 0);
    glVertex3d(1, -1, 1);
    glEnd();

    // back
    glColor3ub(192, 55, 5);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, -1);
    glTexCoord2f(0, 1);
    glVertex3d(-1, 1, -1);
    glTexCoord2f(1, 1);
    glVertex3d(1, 1, -1);
    glTexCoord2f(1, 0);
    glVertex3d(1, -1, -1);
    glEnd();

    // roof

    // front

    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, +1);
    glTexCoord2f(0, 0);
    glVertex3d(-1.2, 1, 1.2);
    glTexCoord2f(1, 0);
    glVertex3d(1.2, 1, 1.2);
    glTexCoord2f(0.5, 1);
    glVertex3d(0, 2.2, 0);
    glEnd();

    // back

    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3d(-1.2, 1, -1.2);
    glTexCoord2f(1, 0);
    glVertex3d(1.2, 1, -1.2);
    glTexCoord2f(0.5, 1);
    glVertex3d(0, 2.2, 0);
    glEnd();

    // left
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(1, 0);
    glVertex3d(-1.2, 1, 1.2);
    glTexCoord2f(0, 0);
    glVertex3d(-1.2, 1, -1.2);
    glTexCoord2f(0.5, 1);
    glVertex3d(0, 2.2, 0);
    glEnd();

    // right
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_TRIANGLES);
    glNormal3f(1, 0, 0);
    glTexCoord2f(1, 0);
    glVertex3d(1.2, 1, 1.2);
    glTexCoord2f(0, 0);
    glVertex3d(1.2, 1, -1.2);
    glTexCoord2f(0.5, 1);
    glVertex3d(0, 2.2, 0);
    glEnd();

    // need a door

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
    glColor3ub(105, 82, 0);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3d(-.9, -1, 1.01);
    glTexCoord2f(1, 0);
    glVertex3d(-.2, -1, 1.01);
    glTexCoord2f(1, 1);
    glVertex3d(-0.2, 0.2, 1.01);
    glTexCoord2f(0, 1);
    glVertex3d(-.9, 0.2, 1.01);
    glEnd();
    // rear entrance for luxury estate

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
    glColor3ub(105, 82, 0);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3d(-.9, -1, -1.01);
    glTexCoord2f(1, 0);
    glVertex3d(-.2, -1, -1.01);
    glTexCoord2f(1, 1);
    glVertex3d(-0.2, 0.2, -1.01);
    glTexCoord2f(0, 1);
    glVertex3d(-.9, 0.2, -1.01);
    glEnd();

    // need windows for fire code sake
    // front window
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glColor3ub(100, 100, 100);
    glTexCoord2f(0, 1);
    glVertex3d(.25, .2, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(.25, -0.25, 1.01);
    glTexCoord2f(1, 0);
    glVertex3d(.8, -0.25, 1.01);
    glTexCoord2f(1, 1);
    glVertex3d(.8, .2, 1.01);
    glEnd();

    // two left windows
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3d(-1.01, .2, -.25);
    glTexCoord2f(1, 0);
    glVertex3d(-1.01, -0.25, -.25);
    glTexCoord2f(0, 0);
    glVertex3d(-1.01, -0.25, -.8);
    glTexCoord2f(0, 1);
    glVertex3d(-1.01, .2, -.8);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex3d(-1.01, .2, .25);
    glTexCoord2f(0, 0);
    glVertex3d(-1.01, -0.25, .25);
    glTexCoord2f(1, 0);
    glVertex3d(-1.01, -0.25, .8);
    glTexCoord2f(1, 1);
    glVertex3d(-1.01, .2, .8);
    glEnd();

    // two right windows
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3d(1.01, .2, -.25);
    glTexCoord2f(1, 0);
    glVertex3d(1.01, -0.25, -.25);
    glTexCoord2f(0, 0);
    glVertex3d(1.01, -0.25, -.8);
    glTexCoord2f(0, 1);
    glVertex3d(1.01, .2, -.8);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex3d(1.01, .2, .25);
    glTexCoord2f(0, 0);
    glVertex3d(1.01, -0.25, .25);
    glTexCoord2f(1, 0);
    glVertex3d(1.01, -0.25, .8);
    glTexCoord2f(1, 1);
    glVertex3d(1.01, .2, .8);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutPostRedisplay();
}

/* I foolishly hardcoded the locations of these
    a-frames rather than just scaling them */
static void aFrame(double x, double y, double z,
                   double dx, double dy, double dz,
                   double th)
{

    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);

    // glEnable(GL_LIGHT1);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // foundation
    glBegin(GL_QUADS);
    glColor3ub(194, 114, 35);
    glVertex3d(-5, -1999.1, +5); //front left
    glVertex3d(-5, -1999.1, -5); // back left
    glVertex3d(+5, -1999.1, -5); // back right
    glVertex3d(+5, -1999.1, +5); // front right
    glEnd();

    // front
    glColor3ub(204, 41, 0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_TRIANGLES);
    glColor3ub(194, 114, 35);
    glNormal3f(0, 0, 1);
    glTexCoord2f(1, 0);
    glVertex3d(+5, -1999.1, +5);
    glTexCoord2f(0, 0);
    glVertex3d(-5, -1999.1, 5);
    glTexCoord2f(0.5, 1);
    glVertex3d(0, -1992.1, 5);
    glEnd();

    // back
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, -1);
    glTexCoord2f(1, 0);
    // bottom right
    glVertex3d(+5, -1999.1, -5);
    glTexCoord2f(0, 0);
    glVertex3d(-5, -1999.1, -5);
    glTexCoord2f(0.5, 1);
    glVertex3d(0, -1992.1, -5);
    glEnd();

    // right side
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glColor3ub(125, 90, 0);
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3d(5, -1999.1, 5);
    glTexCoord2f(1, 0);
    glVertex3d(5, -1999.1, -5);
    glTexCoord2f(1, 1);
    glVertex3d(0, -1992.1, -5);
    glTexCoord2f(0, 1);
    glVertex3d(0, -1992.1, 5);
    glEnd();

    // left side
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glColor3ub(125, 90, 0);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3d(-5, -1999.1, 5);
    glTexCoord2f(1, 0);
    glVertex3d(-5, -1999.1, -5);
    glTexCoord2f(1, 1);
    glVertex3d(0, -1992.1, -5);
    glTexCoord2f(0, 1);
    glVertex3d(0, -1992.1, 5);
    glEnd();

    // door
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBegin(GL_QUADS);
    glColor3ub(105, 82, 0);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    // door bottom
    glVertex3d(-2.5, -1999.1, 5.01);
    glTexCoord2f(1, 0);
    glVertex3d(-.5, -1999.1, 5.01);
    glTexCoord2f(1, 1);
    // end door bottom
    glVertex3d(-.5, -1996.1, 5.01);
    glTexCoord2f(0, 1);
    glVertex3d(-2.5, -1996.1, 5.01);
    glEnd();
    // front window

    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0, 1, 1);
    glTexCoord2f(1, 0);
    glVertex3d(1.5, -1997, 5.1);
    glTexCoord2f(1, 1);
    glVertex3d(1.5, -1995.5, 5.1);
    glTexCoord2f(0, 1);
    glVertex3d(0, -1995.5, 5.1);
    glTexCoord2f(0, 0);
    glVertex3d(0, -1997, 5.1);
    glEnd();

    // back top window
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0, 1, -1);
    glTexCoord2f(1, 0);
    glVertex3d(.75, -1995, -5.1);
    glTexCoord2f(1, 1);
    glVertex3d(.75, -1993.5, -5.1);
    glTexCoord2f(0, 1);
    glVertex3d(-.75, -1993.5, -5.1);
    glTexCoord2f(0, 0);
    glVertex3d(-.75, -1995, -5.1);
    glEnd();

    // back bottom window
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0, 1, -1);
    glTexCoord2f(0, 1);
    glVertex3d(.5, -1996.5, -5.1);
    glTexCoord2f(0, 0);
    glVertex3d(.5, -1998, -5.1);
    glTexCoord2f(1, 0);
    glVertex3d(2, -1998, -5.1);
    glTexCoord2f(1, 1);
    glVertex3d(2, -1996.5, -5.1);
    glEnd();

    // back bottom window #2
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glBegin(GL_QUADS);
    glColor3ub(100, 100, 100);
    glNormal3f(0, 1, -1);
    glTexCoord2f(1, 1);
    glVertex3d(-.5, -1998, -5.1);
    glTexCoord2f(1, 0);
    glVertex3d(-.5, -1996.5, -5.1);
    glTexCoord2f(0, 0);
    glVertex3d(-2, -1996.5, -5.1);
    glTexCoord2f(0, 1);
    glVertex3d(-2, -1998, -5.1);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void hangar(double x, double y, double z,
            double dx, double dy, double dz,
            double th)
{

    double radius = 0.3, height = 0.2;
    double coord;
    glPushMatrix();
    glTranslated(x, y, z);
    glRotated(th, 1, 0, 0);
    glScaled(dx, dy, dz);

    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[2]);

    // basic half-cylinder
    double i;
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= 360; i += .15)
    {
        x = radius * Cos(i);
        y = height;
        z = radius * Sin(i);
        coord = (i / (float)360);

        glNormal3d(Cos(i), 0, Sin(i));

        glTexCoord2f(-coord, 0.0);
        glVertex3d(x, -y, z);
        glTexCoord2f(-coord, 1.0);
        glVertex3d(x, y, z);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glNormal3d(0, -1, 0);

    /// the back of the hangar
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5, 0.5);
    glVertex3d(0.0, -height, 0.0);

    for (i = 0.0; i <= 360; i += 10)
    {
        glTexCoord2f(0.5 * Cos(i) + 0.5, 0.5 * Sin(i) + 0.5);
        glVertex3d(radius * Cos(i), -height, radius * Sin(i));
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// very loosley based on https://glosassociates.com/portfolio/windsor-airport-maintenance-hangar/
//, int sidePanel, int roof, int driveway, int flag
static void hangarGenerator(double x, double y, double z,
                            double dx, double dy, double dz,
                            double th)
{
    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);

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
    // repeat the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glColor3ub(112, 70, 27);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(2, 2);
    glVertex3d(-1, 1, 1);
    glTexCoord2f(0, 2);
    glVertex3d(-1, 1, -1);
    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, -1);
    glTexCoord2f(2, 0);
    glVertex3d(-1, -1, 1);
    glEnd();

    // right
    glColor3ub(192, 55, 5);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glTexCoord2f(2, 2);
    glVertex3d(1, 1, 1);
    glTexCoord2f(0, 2);
    glVertex3d(1, 1, -1);
    glTexCoord2f(0, 0);
    glVertex3d(1, -1, -1);
    glTexCoord2f(2, 0);
    glVertex3d(1, -1, 1);
    glEnd();

    // front-left
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(1, .35);
    glVertex3d(-1, 1, 1);
    glTexCoord2f(1, 0);
    glVertex3d(-1, .65, 1);
    glTexCoord2f(0, 0);
    glVertex3d(0, .65, 1);
    glTexCoord2f(0, 0.35);
    glVertex3d(0, 1, 1);
    glEnd();

    // front-right
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(1, .35);
    glVertex3d(1, 1, 1);
    glTexCoord2f(1, 0);
    glVertex3d(1, .65, 1);
    glTexCoord2f(0, 0);
    glVertex3d(0, .65, 1);
    glTexCoord2f(0, .35);
    glVertex3d(0, 1, 1);
    glEnd();

    //front-top
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, 1);

    glTexCoord2f(0, 0);
    glVertex3d(-1, 1, 1);
    glTexCoord2f(1.0, .2);
    glVertex3d(0, 1.2, 1);
    glTexCoord2f(2.0, 0);
    glVertex3d(1, 1, 1);
    glEnd();

    // back
    glColor3ub(192, 55, 5);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);

    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, -1);
    glTexCoord2f(0, 2);
    glVertex3d(-1, 1, -1);
    glTexCoord2f(2, 2);
    glVertex3d(1, 1, -1);
    glTexCoord2f(2, 0);
    glVertex3d(1, -1, -1);
    glEnd();

    // back inside
    glColor3ub(192, 55, 5);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(0, 0);
    glVertex3d(-1, -1, -.9);
    glTexCoord2f(0, 2);
    glVertex3d(-1, 1, -.9);
    glTexCoord2f(2, 2);
    glVertex3d(1, 1, -.9);
    glTexCoord2f(2, 0);
    glVertex3d(1, -1, -.9);
    glEnd();

    //  back top
    glColor3ub(192, 55, 5);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, -1);

    glTexCoord2f(0, 0);
    glVertex3d(-1, 1, -1);
    glTexCoord2f(2, 0);
    glVertex3d(1, 1, -1);
    glTexCoord2f(1, .2);
    glVertex3d(0, 1.2, -1);
    glEnd();

    // let's do a roof

    // left
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);

    glTexCoord2f(1, 0);
    glVertex3d(-1, 1, 1);
    glTexCoord2f(0, 0);
    glVertex3d(-1, 1, -1);
    glTexCoord2f(0, 1);
    glVertex3d(0, 1.2, -1);
    glTexCoord2f(1, 1);
    glVertex3d(0, 1.2, 1);
    glEnd();

    // right
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);

    glTexCoord2f(1, 0);
    glVertex3d(1, 1, 1);
    glTexCoord2f(0, 0);
    glVertex3d(1, 1, -1);
    glTexCoord2f(0, 1);
    glVertex3d(0, 1.2, -1);
    glTexCoord2f(1, 1);
    glVertex3d(0, 1.2, 1);
    glEnd();

    // side-right-front
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(.5, 1.7);
    glVertex3d(1.45, 0.7, 1.01);
    glTexCoord2f(.5, 0);
    glVertex3d(1.45, -1, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(0.95, -1, 1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(0.95, 0.7, 1.01);
    glEnd();

    // side-right-outside
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);

    glTexCoord2f(2.0, 1.7);
    glVertex3d(1.45, 0.7, 1.01);
    glTexCoord2f(2.0, 0);
    glVertex3d(1.45, -1, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(1.45, -1, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(1.45, 0.7, -1.01);
    glEnd();

    // side-right-inside
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);

    glTexCoord2f(2.0, 1.7);
    glVertex3d(.95, 0.7, 1.01);
    glTexCoord2f(2.0, 0);
    glVertex3d(.95, -1, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(.95, -1, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(.95, 0.7, -1.01);
    glEnd();

    // side-right-back
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);

    glTexCoord2f(.5, 1.7);
    glVertex3d(1.45, 0.7, -1.01);
    glTexCoord2f(.5, 0);
    glVertex3d(1.45, -1, -1.01);
    glTexCoord2f(0, 0);
    glVertex3d(0.95, -1, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(0.95, 0.7, -1.01);
    glEnd();

    // side-right-top
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);

    glTexCoord2f(.5, 1.7);
    glVertex3d(1.45, 0.7, 1.01);
    glTexCoord2f(0.5, 0);
    glVertex3d(1.45, 0.7, -1.01);
    glTexCoord2f(0, 0);
    glVertex3d(0.95, 0.7, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(0.95, 0.7, 1.01);
    glEnd();

    /* HERE BEGINS LEFT SIDE */

    // side-left-front
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);

    glTexCoord2f(.5, 1.7);
    glVertex3d(-1.45, 0.7, 1.01);
    glTexCoord2f(.5, 0);
    glVertex3d(-1.45, -1, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(-0.95, -1, 1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(-0.95, 0.7, 1.01);
    glEnd();

    // side-left-outside
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);

    glTexCoord2f(2.0, 1.7);
    glVertex3d(-1.45, 0.7, 1.01);
    glTexCoord2f(2.0, 0);
    glVertex3d(-1.45, -1, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(-1.45, -1, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(-1.45, 0.7, -1.01);
    glEnd();

    // side-left-inside
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);

    glTexCoord2f(2.0, 1.7);
    glVertex3d(-.95, 0.7, 1.01);
    glTexCoord2f(2.0, 0);
    glVertex3d(-.95, -1, 1.01);
    glTexCoord2f(0, 0);
    glVertex3d(-.95, -1, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(-.95, 0.7, -1.01);
    glEnd();

    // side-left-back
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);

    glTexCoord2f(.5, 1.7);
    glVertex3d(-1.45, 0.7, -1.01);
    glTexCoord2f(.5, 0);
    glVertex3d(-1.45, -1, -1.01);
    glTexCoord2f(0, 0);
    glVertex3d(-0.95, -1, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(-0.95, 0.7, -1.01);
    glEnd();

    // side-left-top
    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);

    glTexCoord2f(.5, 1.7);
    glVertex3d(-1.45, 0.7, 1.01);
    glTexCoord2f(0.5, 0);
    glVertex3d(-1.45, 0.7, -1.01);
    glTexCoord2f(0, 0);
    glVertex3d(-0.95, 0.7, -1.01);
    glTexCoord2f(0, 1.7);
    glVertex3d(-0.95, 0.7, 1.01);
    glEnd();

    // driveway

    glColor3ub(215, 215, 215);
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(.5, 1.7);
    glVertex3d(-.95, -.61, 4.0);
    glTexCoord2f(0.5, 0);
    glVertex3d(-.95, -.61, -1.0);
    glTexCoord2f(0, 0);
    glVertex3d(0.95, -.61, -1.0);
    glTexCoord2f(0, 1.7);
    glVertex3d(0.95, -0.61, 4.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void tree(double x, double y, double z,
                 double dx, double dy, double dz,
                 double CylinderHeight, double CylinderRadius,
                 double ConeHeight, double ConeRadius)
{
    // tree base
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(dx, dy, dz);
    double xt, zt, xd, zd;
    // double y = height;
    double coord;
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    // just going to do a quad strip for the base,
    // no need for bottom or top
    glBegin(GL_QUAD_STRIP);
    for (float i = 0; i < 360; i += 4)
    {
        xt = (CylinderRadius * Cos(i));
        zt = (CylinderRadius * Sin(i));
        coord = -(i / (double)360);

        glNormal3d(Cos(i), 0, Sin(i));
        glTexCoord2f(coord, 0.0);
        glVertex3d(xt, 0, zt);
        glTexCoord2f(coord, 1.0);
        glVertex3d(xt, CylinderHeight, zt);
    }
    glEnd();
    glPopMatrix();

    // tree top
    glPushMatrix();
    glTranslated(x, y - (y * .003), z);
    glScaled(dx, dy, dz);
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glBegin(GL_TRIANGLES);
    for (float i = 0; i < 360; i += .15)
    {
        xt = (ConeRadius * Cos(i));
        zt = (ConeRadius * Sin(i));
        xd = ConeRadius * Cos(i + .15);
        zd = ConeRadius * Sin(i + .15);
        glNormal3d(xt, 0, zt);
        glTexCoord2f(Cos(i), 0.0);
        glVertex3f(xt, 0, zt);
        glNormal3d(xd, 0, zd);
        glTexCoord2f(Cos(i + .15), 0.0);
        glVertex3f(xd, 0, zd);
        glTexCoord2f(Cos(i) + (.075), 1.0);
        glVertex3f(0, ConeHeight, 0);
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void Vertex(double th, double ph)
{
    double x = Sin(th) * Cos(ph);
    double y = Cos(th) * Cos(ph);
    double z = Sin(ph);
    glNormal3d(x, y, z);
    glVertex3d(x, y, z);
}

/* responsible for drawing the flag at the end of the runway */
static void drawFlag(double x, double y, double z,
                     double dx, double dy, double dz,
                     double CylinderRadius, double CylinderHeight, double th)
{

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    int inc = 10;
    int th2, ph2;
    double x2, z2, y2;
    y2 = 1;
    glColor3ub(168, 170, 168);
    glPushMatrix();
    glEnable(GL_NORMALIZE);
    // poll
    glTranslated(x + th, y + 2, z + th);
    glRotated(180, 0, 0, 1);
    glScaled(10 * dx, CylinderHeight, 10 * dz);

    glBegin(GL_QUAD_STRIP);
    for (float i = 0; i < 360; i += 4)
    {
        x2 = (CylinderRadius * Cos(i));
        z2 = (CylinderRadius * Sin(i));
        // coord = i / (double)360;

        glNormal3d(Cos(i), 0, Sin(i));
        glVertex3d(x2, 0, z2);
        glVertex3d(x2, y2, z2);
    }
    glEnd();
    glPopMatrix();
    // top of poll
    glPushMatrix();
    glTranslated(x + th, y + 2, z + th);
    glRotated(90, 0, 1, 0);
    glScaled(dx / 1.6, dy / 1.6, dz / 1.6);
    for (ph2 = -90; ph2 < 90; ph2 += inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th2 = 0; th2 <= 360; th2 += 2 * inc)
        {
            Vertex(th2, ph2);
            Vertex(th2, ph2 + inc);
        }
        glEnd();
    }
    glPopMatrix();

    // front of flag
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    glTranslated(x + th, y - 11, z + th);
    glRotated(90, 0, 1, 0);
    glColor3ub(100, 100, 100);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glTexCoord3f(0, 1, 0);
    glVertex3d(CylinderRadius, CylinderHeight, 0);
    glTexCoord3f(1, 1, 0);
    glVertex3d(CylinderRadius - 13, CylinderHeight, 0);
    glTexCoord3f(1, 0, 0);
    glVertex3d(CylinderRadius - 13, CylinderHeight - 5, 0);
    glTexCoord3f(0, 0, 0);
    glVertex3d(CylinderRadius, CylinderHeight - 5, 0);

    glEnd();
    glPopMatrix();
    // back of flag
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    glTranslated(x + th, y - 11, z + th);
    glRotated(90, 0, 1, 0);
    glColor3ub(100, 100, 100);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord3f(0, 1, 0);
    glVertex3d(CylinderRadius, CylinderHeight, .1);
    glTexCoord3f(1, 1, 0);
    glVertex3d(CylinderRadius - 13, CylinderHeight, .1);
    glTexCoord3f(1, 0, 0);
    glVertex3d(CylinderRadius - 13, CylinderHeight - 5, .1);
    glTexCoord3f(0, 0, 0);
    glVertex3d(CylinderRadius, CylinderHeight - 5, .1);

    glEnd();
    glPopMatrix();

    // glEnable(GL_TEXTURE_2D);
    // glEnable(GL_COLOR_MATERIAL);

    //second poll and top
    glColor3ub(168, 170, 168);
    glPushMatrix();
    glEnable(GL_NORMALIZE);
    glTranslated(x + th, y + 2, z + th + 13);
    glRotated(180, 0, 0, 1);
    glScaled(10 * dx, CylinderHeight, 10 * dz);

    glBegin(GL_QUAD_STRIP);
    for (float i = 0; i < 360; i += 4)
    {
        x2 = (CylinderRadius * Cos(i));
        z2 = (CylinderRadius * Sin(i));

        glNormal3d(Cos(i), 0, Sin(i));
        glVertex3d(x2, 0, z2);
        glVertex3d(x2, y2, z2);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(x + th, y + 2, z + th + 13);
    glRotated(180, 0, 1, 0);
    glScaled(dx / 1.6, dy / 1.6, dz / 1.6);
    for (ph2 = -90; ph2 < 90; ph2 += inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th2 = 0; th2 <= 360; th2 += 2 * inc)
        {
            Vertex(th2, ph2);
            Vertex(th2, ph2 + inc);
        }
        glEnd();
    }
    glPopMatrix();

    glDisable(GL_NORMALIZE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
}

/* drawing the landmass and the surrounding ocean */
static void OceanAndIsland()
{
    float white[] = {1, 1, 1, 1};
    float black[] = {0, 0, 0, 1};
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // // to draw dashed lines for lanes
    // glLineStipple(1, 0x00FF);
    // glEnable(GL_LINE_STIPPLE);
    // glLineWidth(2);
    // glBegin(GL_LINES);

    glColor3ub(255, 255, 0);

    // Repeating texture x8
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBegin(GL_QUADS);

    // Ocean
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3d(-2000, -1999.5, -2000);
    glTexCoord2f(0, 8);
    glVertex3d(-2000, -1999.5, 2000);
    glTexCoord2f(8, 8);
    glVertex3d(2000, -1999.5, 2000);
    glTexCoord2f(8, 0);
    glVertex3d(2000, -1999.5, -2000);
    glEnd();

    // landmass
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3d(-200, -1999.25, -200);
    glTexCoord2f(0, 8);
    glVertex3d(-200, -1999.25, 200);
    glTexCoord2f(8, 8);
    glVertex3d(200, -1999.25, 200);
    glTexCoord2f(8, 0);
    glVertex3d(200, -1999.25, -200);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/* building the runway */
static void runWay()
{
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3d(-65, -1999.0, -4);
    glTexCoord2f(0, 4);
    glVertex3d(65, -1999.0, -4);
    glTexCoord2f(4, 4);
    glVertex3d(65, -1999.0, 4);
    glTexCoord2f(4, 0);
    glVertex3d(-65, -1999.0, 4);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void init(void)
{
    glutReshapeWindow(windowWidth, windowHeight);
    glutPositionWindow(0, 0);
    glEnable(GL_DEPTH_TEST);

    // Black Background
    glClearColor(0, 0, 0, 1.0);

    // set camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // gluPerspective(fovy, aspect, near, far)
    gluPerspective(90, windowWidth / windowHeight, 0.1, 30000);

    // for translation/rotation/etc
    glMatrixMode(GL_MODELVIEW);

    /* ended here nov 21------------------------------------------------------*/

    CirrusToList();

    PropToList();
}

void reshape(int newWidth, int newHeight)
{

    // Update widthxheight of window
    windowWidth = newWidth;
    windowHeight = newHeight;

    glViewport(0, 0, windowWidth, windowHeight);

    // Change camera properties
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    // Modify the gluPerspective (fovy, aspect, near, far)
    gluPerspective(45, windowWidth / windowHeight, 0.1, 4000);

    // Back into modelview
    glMatrixMode(GL_MODELVIEW);
}

void normalKeys(unsigned char ch, int x, int y)
{
    if (ch == 27)
    {
        exit(0);
    }
    if (ch == 'w' || ch == 'W')
    {
        if (planeSpeed < 0.3 && !isLanding)
        {
            planeSpeed += .05;
            // break;
        }
        else
            printf("This is the vechicle's top speed!\n");
    }
    if (ch == 's' || ch == 'S')
    {
        if (planeSpeed > 0.15 && !isLanding)
        {
            planeSpeed -= 0.05;
        }
        else if (planeSpeed > 0.05 && !isLanding)
        {
            planeSpeed -= 0.01;
        }
        else
            printf("Flight instructor won't allow this thing to go any slower! \n");
    }
    if ((ch == 'b' || ch == 'B') && !crash && !isLanding)
    {
        fireMissle = 1 - fireMissle;
    }
    if ((ch == 't' || ch == 'T') && isStopped)
    {
        isLanding = 0;
        isStopped = 0;
        isDeparting = 1;
    }
    glutPostRedisplay();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable Z-buffering
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    positionScene();

    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
              cameraPosition[3], cameraPosition[4], cameraPosition[5], 0, 1, 0);

    OceanAndIsland(); // make land mass and surrounding ocean
    drawSkyBox(dim);  // skybox, of course; might be worth finding better bmp's for this
    drawPlane();

    // if missle has been fired
    if (fireMissle)
    {
        float F = 2;
        float Diffuse[] = {0.5f * F, 0.5f * F, 0.5f * F, 1.0f};
        float positionf[4] = {0, 0, 0, 0};
        if (planeSpeed > .15)
        {
            // positionf[0] = {(float)(bombPosition[0] + (30 * saveXChange)), (float)(bombPosition[1]), (float)(30 * saveZChange + bombPosition[2]), 1.0f};
            positionf[0] = (float)(bombPosition[0] + (30 * saveXChange));
            positionf[1] = (float)(bombPosition[1]);
            positionf[2] = (float)(30 * saveZChange + bombPosition[2]);
            positionf[3] = 1.0f;
        }
        else
        {
            // positionf = {(float)(bombPosition[0] + (saveXChange)), (float)(bombPosition[1]), (float)(saveZChange + bombPosition[2]), 1.0f};
            positionf[0] = (float)(bombPosition[0] + (saveXChange));
            positionf[1] = (float)(bombPosition[1]);
            positionf[2] = (float)(saveZChange + bombPosition[2]);
            positionf[3] = 1.0f;
        }
        glEnable(GL_LIGHTING);
        // glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
        glEnable(GL_NORMALIZE);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, positionf);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHT0);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, .0001); // light should dimiish over time non-linearly
        drawMissle();
    }
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        drawMissle();
    }

    float Diffuse[] = {1, 1, 1, 1}; //high diffuse
    float white[] = {1, 1, 1, 1};
    //  Light direction

    float position2[4] = {1800, -1800, 1100, 1};
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT2, GL_DIFFUSE, Diffuse);
    glLightfv(GL_LIGHT2, GL_POSITION, position2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT2);

    moon();

    glDisable(GL_COLOR_MATERIAL);

    /* drawing some buildings, trees, and a runway */

    drawFlag(-55, -1990, -6.5, .3, .3, .3, .06, 13, 0);

    aFrame(-15, 0, 25, 1, 1, 1, 180);

    aFrame(15, 0, 25, 1, 1, 1, 180);
    house(0, -1996.2, 25, 2.5, 2.5, 2.5, 180);
    house(-30, -1996.2, 25, 2.5, 2.5, 2.5, 180);
    aFrame(40, 0, -25, 1, 1, 1, 0);
    house(-15, -1996.2, -25, 2.5, 2.5, 2.5, 180);

    aFrame(15, 0, -25, 1, 1, 1, 0);
    house(0, -1996.2, -25, 2.5, 2.5, 2.5, 0);

    runWay();

    hangar(25, -1999, -25, 15, 15, 15, 90);

    hangarGenerator(100, -1996, 0, 5, 5, 5, -90);
    tree(100, -1999, 14, 5, 5, 5, 3, .05, 3, .5);
    tree(100, -1999, -14, 5, 5, 5, 3, .05, 3, .5);

    hangarGenerator(65, -1996, -30, 3, 5, 3, 0);

    hangarGenerator(65, -1996, 30, 3, 5, 3, 180);

    // trees of various sizes
    tree(78, -1999, -96, 6, 9, 6, 3, .05, 3, .5);
    tree(169, -1999, 122, 5, 8, 5, 3, .05, 3, .5);
    tree(194, -1999, 77, 5, 5, 5, 3, .05, 3, .5);
    tree(89, -1999, 111, 7, 9, 7, 3, .05, 3, .5);
    tree(-114, -1999, -127, 5, 5, 5, 3, .05, 3, .5);

    glFlush();
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(850, 600);

    // load textures
    glutCreateWindow("Final Project: Warren's Simulator");
    sky[0] = LoadTexBMP("space2.bmp");
    sky[1] = LoadTexBMP("sky1.bmp");
    texture[0] = LoadTexBMP("panel1.bmp");
    texture[1] = LoadTexBMP("panel2.bmp");
    texture[2] = LoadTexBMP("newBrick.bmp");
    texture[3] = LoadTexBMP("aluminum.bmp");
    texture[4] = LoadTexBMP("door2.bmp");
    texture[5] = LoadTexBMP("door.bmp");
    texture[6] = LoadTexBMP("roof.bmp");
    texture[7] = LoadTexBMP("window.bmp");
    texture[7] = LoadTexBMP("ocean.bmp");
    texture[8] = LoadTexBMP("sand.bmp");
    texture[9] = LoadTexBMP("dirtRunway.bmp");
    texture[10] = LoadTexBMP("concrete.bmp");
    texture[11] = LoadTexBMP("pineTree.bmp");
    texture[12] = LoadTexBMP("bark.bmp");
    texture[13] = LoadTexBMP("raceFlag.bmp");
    // set viewport dimensions and displaylists
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    // normal key functions
    glutKeyboardFunc(normalKeys);
    // mouse logic
    glutPassiveMotionFunc(mousePosition);

    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}