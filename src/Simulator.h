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

 #ifndef SIMULATOR_H
 #define SIMULATOR_H
 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include <vector>
 #include <stdio.h>
 #include <iostream>
 #include <stdlib.h>
 #include <assimp/Importer.hpp>
 #include <assimp/scene.h>
 #include <assimp/mesh.h>
 #include "glm/glm.hpp"
 #include "glm/gtc/type_ptr.hpp"
 #include "glm/gtc/matrix_transform.hpp"
 #include "SolidSphere.h"
 #include "MeshObject.h"
 #include "Mesh.h"
 #if defined(__APPLE_CC__)
 #else
 #include <math.h>
 #endif

 class Simulator {
 public:
     Simulator(GLuint id);
     void gluCircle(GLdouble radius) const;
     void glSphere(double x, double y, double z);
     void render();
     void step();
     void load(std::string path);

 private:
 /**
     struct Particle
     {
         glm::dvec3 p;
         glm::dvec3 v;
         glm::dvec3 f;
         double density;
         bool
     };**/
     
     struct Obj {
         enum OBJ_TYPE {
			    PARTICLE, MESHOBJECT
		     };
         glm::dvec3 p;
         glm::dvec3 v;
         glm::dvec3 f;
         glm::dvec3 cm;
         double density;
         short type;
     };
     
     const glm::dvec3 gravity = glm::dvec3(0.0, -9.81, 0.0);
     GLuint pid;
     std::vector<Mesh*> meshes;
     std::vector<Obj> objects;
     SolidSphere sphere = SolidSphere();
 };

 #endif
