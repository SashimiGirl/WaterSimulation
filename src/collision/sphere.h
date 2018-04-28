#ifndef COLLISIONOBJECT_SPHERE_H
#define COLLISIONOBJECT_SPHERE_H

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace CGL;
using namespace std;

struct Sphere : public CollisionObject {
public:
  Sphere(const Vector3D &origin, double radius, double friction)
      : origin(origin), radius(radius), radius2(radius * radius),
        friction(friction), start_position(position), position(position),
        last_position(position) {}

  void render(GLShader &shader);
  void collide(PointMass &pm);

  Vector3D normal();
  Vector3D velocity(double delta_t) {
    return (position - last_position) / delta_t;
  }

  // static values
  Vector3D start_position;

  // dynamic values
  Vector3D position;
  Vector3D last_position;
  Vector3D forces;

private:
  Vector3D origin;
  double radius;
  double radius2;

  double friction;
};

#endif /* COLLISIONOBJECT_SPHERE_H */
