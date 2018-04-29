#ifndef COLLISIONOBJECT_BOX_H
#define COLLISIONOBJECT_BOX_H

#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

struct Box : public CollisionObject {
public:
  Box(const Vector3D &point, 
  	const Vector3D &normal, double friction,
  	double xshift, double yshift, double zshift, double s)
      : point(point), normal(normal.unit()), friction(friction),
      xshift(xshift), yshift(yshift), zshift(zshift), s(s) {}

  void render(GLShader &shader);
  void collide(PointMass &pm);

  Vector3D point;
  Vector3D normal;

  double friction;
  double xshift;
  double yshift;
  double zshift;
  double s;
};

#endif /* COLLISIONOBJECT_PLANE_H */
