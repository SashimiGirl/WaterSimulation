#ifndef COLLISIONOBJECT_BOX_H
#define COLLISIONOBJECT_BOX_H

#include <nanogui/nanogui.h>
#include <vector>

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

struct Box : public CollisionObject {
public:
  Box(const Vector3D &oh, 
  	const Vector3D &ok, double friction,
  	double xshift, double yshift, double zshift, double s)
      : friction(friction),
      xshift(xshift), yshift(yshift), zshift(zshift), s(s) {
      
      	point.emplace_back(Vector3D(xshift, yshift, zshift)); // Bottom
      	point.emplace_back(Vector3D(xshift, yshift, zshift)); // Left
      	point.emplace_back(Vector3D(xshift + s, yshift, zshift)); // Right
      	point.emplace_back(Vector3D(xshift, yshift, zshift)); // Back
      	point.emplace_back(Vector3D(xshift, yshift, zshift + s)); // Front

      	normal.emplace_back(Vector3D(0, 1, 0));
      	normal.emplace_back(Vector3D(1, 0, 0));
      	normal.emplace_back(Vector3D(-1, 0, 0));
      	normal.emplace_back(Vector3D(0, 0, 1));
      	normal.emplace_back(Vector3D(0, 0, -1));
      	
      }

  void render(GLShader &shader);
  void collide(PointMass &pm);

  vector<Vector3D> point;
  vector<Vector3D> normal;

  double friction;
  double xshift;
  double yshift;
  double zshift;
  double s;
};

#endif /* COLLISIONOBJECT_PLANE_H */
