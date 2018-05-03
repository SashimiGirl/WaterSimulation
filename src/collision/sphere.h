#ifndef COLLISIONOBJECT_SPHERE_H
#define COLLISIONOBJECT_SPHERE_H

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace CGL;
using namespace std;

struct Sphere : public CollisionObject {
public:
  Sphere(const Vector3D &origin, double radius, double friction, double elasticity)
      : friction(friction), CollisionObject(origin, radius) {
        this->elasticity = elasticity;
        //cout << this->mass << "\n";
      }

  void render(GLShader &shader);
  void collide(PointMass &pm, bool flag);

  Vector3D normal();
  // Vector3D velocity(double delta_t) {
  //   return (position - last_position) / delta_t;
  // }

private:
  double friction;
  bool pinned = false;
};

#endif /* COLLISIONOBJECT_SPHERE_H */
