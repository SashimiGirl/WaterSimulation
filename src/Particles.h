/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Particles.h
 * Author: swl
 *
 * Created on April 15, 2016, 12:16 PM
 */

 #ifndef PARTICLES_H
 #define PARTICLES_H
 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include "glm/glm.hpp"
 #include <vector>
 #include <stdio.h>
 #include <stdlib.h>
 #if defined(__APPLE_CC__)
 #else
 #include <math.h>
 #endif

 class Particles {
 public:
 	struct Particle
     {
         glm::dvec3 p;
         glm::dvec3 v;
     };
     Particles();
     void gluCircle(GLdouble radius) const;
     void drawSphere(GLdouble r, int lats, int longs) const;
     void render() const;
     
     void step() {
      for (Particle &par : particles) {
        par.p += glm::dvec3(double(rand()) / RAND_MAX * 0.02,double(rand()) / RAND_MAX  * 0.02,double(rand()) / RAND_MAX  * 0.02) - glm::dvec3(0.01, 0.01, 0.01);
      }
     
     
     } // simulate one frame
 private:
     std::vector<Particle> particles;
 };

Particles::Particles()
{
    int nx = 10;
    int ny = 10;
    int nz = 10;
    float d = 0.1;
    for(int x=0; x<nx; x++)
    {
        for(int y=0; y<ny; y++)
        {
            for(int z=0; z<nz; z++)
            {
                Particle par;
                par.p = glm::dvec3((x+0.5-nx*0.5)*d, (y+0.5)*d-1.0, (z+0.5-nz*0.5)*d);
                particles.push_back(par);
            }
        }
    }
}

void Particles::gluCircle(GLdouble radius) const
{
    const float DEG2RAD = 3.14159/180;
    ofBeginShape(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    glColor3f(0.2, 0.5, 0.8);
    for (int i=0; i <= 360; i++) {
        float degInRad = i*DEG2RAD;
        glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
    }
    glEnd();
}

void Particles::drawSphere(GLdouble r, int lats, int longs) const {
    int i, j;
    for(i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);
    
        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
    
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
    
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
     }
}

void Particles::render() const
{
    //std::cout << "we render now!";
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };
    glShadeModel (GL_FLAT);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glColor3f(0.2, 0.5, 0.8);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glColor3f(0.9, 0.9, 0.9);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glColor3f(0.2, 0.5, 0.8);

    for(const Particle &par : particles)
    {
        glPushMatrix();
        glTranslatef(par.p.x, par.p.y, par.p.z);
        //drawSphere(0.005, 10, 10);
        gluCircle(0.05);
        glPopMatrix();
    }

    glPopAttrib();
}


 #endif /* PARTICLES_H */
