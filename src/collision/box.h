#ifndef COLLISIONOBJECT_BOX_H
#define COLLISIONOBJECT_BOX_H

#include <nanogui/nanogui.h>
#include <vector>

#include "../clothMesh.h"
#include "collisionObject.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

struct Box : public CollisionObject {
public:
  Box(double friction, double elasticity, double xshift, double yshift,
    double zshift, double s)
      : friction(friction), xshift(xshift), yshift(yshift),
      zshift(zshift), s(s), CollisionObject(100.0) {

      	point.emplace_back(Vector3D(xshift, yshift, zshift)); // Bottom
      	point.emplace_back(Vector3D(xshift, yshift, zshift)); // Left
      	point.emplace_back(Vector3D(xshift + s, yshift + s, zshift)); // Right
      	point.emplace_back(Vector3D(xshift, yshift, zshift)); // Back
      	point.emplace_back(Vector3D(xshift, yshift, zshift + s)); // Front

      	normal.emplace_back(Vector3D(0, 1, 0)); // Bottom
      	normal.emplace_back(Vector3D(1, 0, 0)); // Left
      	normal.emplace_back(Vector3D(-1, 0, 0)); // Right
      	normal.emplace_back(Vector3D(0, 0, 1)); // Back
      	normal.emplace_back(Vector3D(0, 0, -1)); // Front
        this->elasticity = elasticity;
      }

  void render(GLShader &shader);
  void collide(PointMass &pm);
  void collide(CollisionObject &s);

  vector<Vector3D> point;
  vector<Vector3D> normal;

  double friction;
  double xshift;
  double yshift;
  double zshift;
  double s;
};

#endif /* COLLISIONOBJECT_PLANE_H */
