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
#include <unordered_map>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/norm.hpp"
#include "SolidSphere.h"
#include "MeshObject.h"
#include "Mesh.h"
#if defined(__APPLE_CC__)
#else
#include <math.h>
#endif

class Simulator {
public:
  struct PointMass {
    PointMass(glm::vec3 origin, float mass, float radius, int hash) :
      start_position(origin), position(origin), mass(mass), hash(hash), radius(radius),
      last_position(origin), tmp_position(origin) {}

    void render(GLuint shader);
    //void collide(Obj &o);

    // static values
    int hash;

    // dynamic values
    float radius;
    float lambda;
    float mass;
    glm::vec3 start_position;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 forces;
    glm::vec3 last_position;
    glm::vec3 tmp_position;
    std::vector<PointMass*> neighbors;
  };

  struct Obj {
  public:
    enum OBJ_TYPE {
    PRIMITIVE, MESHOBJECT
    };
    Obj() {}
    Obj(glm::vec3 origin, float mass, float friction, float elasticity) :
      mass(mass), start_position(origin), friction(friction),
      elasticity(elasticity), position(origin), type(PRIMITIVE) {}

    virtual void render(GLuint shader) = 0;
    virtual void collide(PointMass &p) = 0;
    virtual void collide(Obj &o) = 0;

    // static values
    glm::vec3 start_position;
    glm::vec3 cm;
    float mass;
    float friction;
    float elasticity;
    OBJ_TYPE type;

    //dynamic values
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 forces;
  };
  struct Box : public Obj {
    Box(glm::vec3 origin, float width, float depth, float friction, float elasticity) :
      Obj(origin, 0.0, friction, elasticity), width(width), depth(depth) {
      point[0] = origin;
      point[1] = origin + glm::vec3(-width / 2, 0, 0);
      point[2] = origin + glm::vec3( width / 2, 0, 0);
      point[3] = origin + glm::vec3( 0, 0, -depth / 2);
      point[4] = origin + glm::vec3( 0, 0,  depth / 2);
    }
      void render(GLuint shader);
      void collide(PointMass &p);
      void collide(Obj &o);
    float width;
    float depth;
    glm::vec3 point[5];
    glm::vec3 normal[5] = {glm::vec3(0, 1, 0),
                           glm::vec3(1, 0, 0),
                           glm::vec3(-1, 0, 0),
                           glm::vec3(0, 0, 1),
                           glm::vec3(0, 0, -1)};
  };

  struct WaterParameters {
    WaterParameters() {}
    WaterParameters(float density, int w, int h, int l) :
      density(density), num_length_points(l), num_width_points(w), num_height_points(h) {}
    ~WaterParameters() {}

    // liquid parameters
    float damping;
    float density;
    float ks;
    float viscosity;
    int num_length_points;
    int num_width_points;
    int num_height_points;
  };


  static void glSphere(float x, float y, float z, GLuint pid);

  Simulator(GLuint id, glm::vec3 gravity, WaterParameters *wp, float fps, float steps);
  void render();
  void step();
  void load(std::string path);
  void reset();
  void pause();
  void build_spatial_map();
  float SPkernel(glm::vec3 in, float var, float scalar);
  float Pkernel(glm::vec3 in, float var, float scalar, float scalar2);
  glm::vec3 dSPkernel(glm::vec3 in, float var, float scalar);
  float pointDensity(PointMass &p);
  glm::vec3 deltaP(PointMass& p);
  glm::vec3 gradC(PointMass* pk, PointMass* pi);
  void self_collide(PointMass &pm, std::vector<PointMass *> &candidates);
  void hash_collide(uint64_t hash, std::vector<PointMass *> &candidates);
  uint64_t hash_position(glm::vec3 pos);
  /**
  struct Particle
  {
     glm::dvec3 p;
     glm::dvec3 v;
     glm::dvec3 f;
     double density;
     bool
  };**/
  static SolidSphere sphere;

  const GLuint pid;
  const glm::vec3 gravity;

  bool paused;
  float fps;
  float steps;
  Box container;
  std::vector<Mesh*> meshes;
  std::vector<Obj> objects;
  std::vector<PointMass> particles;
  WaterParameters *wp;
  std::unordered_map<uint64_t, std::vector<PointMass *> *> map;
};

#endif
