#ifndef COLLISIONOBJECT
#define COLLISIONOBJECT

#include <nanogui/nanogui.h>

#include "../clothMesh.h"

using namespace CGL;
using namespace std;
using namespace nanogui;

struct CollisionObject {
public:
  CollisionObject(double m) {
  	mass = m;
  }
  CollisionObject(Vector3D origin, double r) {
  	// Soft Rubber density: 0.11 grams/cm^3
  	density = 0.11;
  	mass = density * 3.14159265 * (4 / 3) * pow(r, 3);
  	start_position = origin;
  	last_position = origin;
  	position = origin;
  	radius = r;
  }
  virtual void render(GLShader &shader) = 0;
  virtual void collide(PointMass &pm) = 0;

  // static values
  double mass;
  double density;
  Vector3D start_position;
  double radius;

  // dynamic values
  Vector3D position;
  Vector3D last_position;
  Vector3D forces;

private:
  double friction;
};

#endif /* COLLISIONOBJECT */
