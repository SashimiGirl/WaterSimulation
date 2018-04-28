/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Particles.cpp
 * Author: swl
 *
 * Created on April 15, 2016, 12:16 PM
 */

#include "Simulator.h"


Simulator::Simulator()
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
    this->sphere = SolidSphere(d/2, (unsigned int) 8, (unsigned int) 10);
    
}

void Simulator::gluCircle(GLdouble radius) const
{
    const float DEG2RAD = 3.14159/180;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    glColor3f(0.2, 0.5, 0.8);
    for (int i=0; i <= 360; i++) {
        float degInRad = i*DEG2RAD;
        glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
    }
    glEnd();
}

void Simulator::glSphere(double x, double y, double z)
{
    this->sphere.draw(x, y, z);
}

void Simulator::render()
{
    //std::cout << "we render now!";
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };
    glShadeModel (GL_SMOOTH);
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

    for(const MeshObj &m : meshes)
    {
      continue;
      //render meshes
    }
    
    for(const Particle &par : particles)
    {
      //glPushMatrix();
      //glTranslatef(par.p.x, par.p.y, par.p.z);
      //gluSphere(0.05, 10, 10);
      glSphere(par.p.x, par.p.y, par.p.z);
      //glPopMatrix();
    }

    glPopAttrib();
}
