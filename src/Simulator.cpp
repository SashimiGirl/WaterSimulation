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


Simulator::Simulator(GLuint id)
{
    int nx = 50;
    int ny = 50;
    int nz = 50;
    float d = 0.1;
    for(int x=0; x<nx; x++)
    {
        for(int y=0; y<ny; y++)
        {
            for(int z=0; z<nz; z++)
            {
                Obj par;
                par.p = glm::dvec3((x+0.5-nx*0.5)*d, (y+0.5)*d-1.0, (z+0.5-nz*0.5)*d);
                par.v = glm::dvec3(0.0, 0.0, 0.0);
                par.f = glm::dvec3(0.0, 0.0, 0.0);
                par.cm = glm::dvec3(0.0, 0.0, 0.0);
                par.density = 1.0;
                par.type = Simulator::Obj::PARTICLE;
                objects.push_back(par);
            }
        }
    }
    this->pid = id;
    this->sphere = SolidSphere(d/2);
    
}

void Simulator::glSphere(double x, double y, double z)
{
    this->sphere.draw(x, y, z, pid);
}

void Simulator::render()
{
    //std::cout << "we render now!";
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };
    
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT, GL_DIFFUSE);
    //glColor3f(0.2, 0.5, 0.8);
    //glColorMaterial(GL_FRONT, GL_SPECULAR);
    //glColor3f(0.9, 0.9, 0.9);
    //glColorMaterial(GL_FRONT, GL_AMBIENT);
    //glColor3f(0.2, 0.5, 0.8);
    GLuint loc = glGetUniformLocation(pid, "lPosition");
    glUniform3fv(loc, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
    loc = glGetUniformLocation(pid, "lIntensity");
    glUniform3fv(loc, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    

    for(const Obj &o : objects)
    {
      //glPushMatrix();
      //glTranslatef(par.p.x, par.p.y, par.p.z);
      //gluSphere(0.05, 10, 10);
      if (o.type == Simulator::Obj::PARTICLE) {
        glSphere(o.p.x, o.p.y, o.p.z);
      }
      //glPopMatrix();
    }

    //glPopAttrib();
}

void Simulator::step()
{
       
  for (Obj &o : objects) {
    o.p += glm::dvec3(double(rand()) / RAND_MAX * 0.01,double(rand()) / RAND_MAX  * 0.01,double(rand()) / RAND_MAX  * 0.01) - glm::dvec3(0.005, 0.005, 0.005);
  }
}

void Simulator::load(std::string path)
{
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, 0);
  if(!scene) {
    printf("Unable to load mesh: %s\n", importer.GetErrorString());
  }

  for(int j = 0; j < scene->mNumMeshes; j += 1) {
    this->meshes.push_back(new MeshObject(scene->mMeshes[j]));
  }
}

















