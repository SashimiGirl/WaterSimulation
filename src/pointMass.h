#ifndef POINTMASS_H
#define POINTMASS_H
#include <nanogui/nanogui.h>
#include <glad/glad.h>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "CGL/vector3D.h"

using namespace CGL;
using namespace std;
using namespace nanogui;

// Forward declarations
class Halfedge;

struct PointMass {
  PointMass(Vector3D pos, double radius, int hash) :
  start_position(pos), position(pos), radius(radius), hash(hash),
  friction(0.2), last_position(pos) {}

  void collide(PointMass &pm);
  void render(GLShader &shader);
  Vector3D normal();
  // Vector3D velocity(double delta_t) {
  //   return (position - last_position) / delta_t;
  // }

  // static values
  Vector3D start_position;
  double radius;
  double friction;
  int hash;
  double mass;

  // dynamic values
  Vector3D position;
  Vector3D last_position;
  Vector3D tmp_position;
  Vector3D forces;
  Vector3D velocity;
  Vector3D last_velocity;
  Vector3D pressure;
  //for sph
  vector<PointMass*> neighbors;
  // mesh reference
  Halfedge *halfedge;
};

#endif /* POINTMASS_H */
