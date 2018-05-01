#ifndef COLLISIONOBJECT_SPHERE_H
#define COLLISIONOBJECT_SPHERE_H

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace CGL;
using namespace std;

struct Sphere : public CollisionObject {
public:
  Sphere(const Vector3D &origin, double radius, double friction)
      : friction(friction), CollisionObject(origin, radius) {}

  void render(GLShader &shader);
  void collide(PointMass &pm);

  Vector3D normal();
  Vector3D velocity(double delta_t) {
    return (position - last_position) / delta_t;
  }

private:
  double friction;
};

#endif /* COLLISIONOBJECT_SPHERE_H */
