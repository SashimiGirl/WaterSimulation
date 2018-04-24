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
 #include <iostream>
 #if defined(__APPLE_CC__)
 #else
 #include <math.h>
 #endif

 class Particles {
 public:
     Particles();
     void gluCircle(GLdouble radius) const;
     void render() const;
     
     void step() {
      for (Particle &par : particles) {
        par.p += glm::dvec3(0.01, 0.01, 0.01);
      }
     
     
     } // simulate one frame
 private:
     struct Particle
     {
         glm::dvec3 p;
         glm::dvec3 v;
     };

     std::vector<Particle> particles;
 };

 #endif /* PARTICLES_H */
