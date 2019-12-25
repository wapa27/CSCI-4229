static void cone(double height, double radius){
    double x, z;
    double y = height;
    double coord;
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBegin(GL_TRIANGLES);
    for(float i = 0; i < 360; i+=.15){
        x = (radius*Cos(i));
        z = (radius*Sin(i));
        xd = radius*Cos(i+.15);
        zd = radius*Sin(i+.15);
        coord = i/(double)360;
        glNormal3d(x, 0, z);
        glTexCoord2f(Cos(i), 0.0); glVertex3f(x, 0, z);
        glNormal3d(xd, 0, zd);
        glTexCoord2f(Cos(i+.15), 0.0); glVertex3f(xd, 0, zd);
        glTexCoord2f(Cos(i)+(.15/2), 1.0); glVertex3f(0, height, 0);
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void cylinder(double height, double radius){
    double x, z;
    double y = height;
    const double coord;
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture[5]);

    glBegin(GL_QUAD_STRIP);
    for(float i = 0; i < 360; i+=.125){
        x = (radius*Cos(i));
        z = (radius*Sin(i));
        coord = i/(double)360

        glNormal3d(cos(i), 0, Sin(i));
        glTexCoord2f(-coord, 0.0); glVertex3d(x, 0, z);
        glTexCoord2f(-coord, 1.0); glVertex3d(x, y, z);
    }
    glEnd():
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void tree(double x, double y, double z,
                   double dx, double dy, double dz,
                   double height, double CylinderRadius,
                   double ConeRadius){

    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(dx,dy,dz);


    cylinder(CylinderHeight, CylinderRadius);
    glPopMatrix();

    glPushMatrix();
    glTranslated(x, (y+height*), z);
    glScaled(dx,dy,dz);

    cone(ConeHeight, ConeRadius);
    glPopMatrix();
}