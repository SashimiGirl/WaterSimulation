#include <nanogui/nanogui.h>

#include "misc/sphere_drawing.h"
#include "pointMass.h"

using namespace nanogui;
using namespace CGL;

void PointMass::collide(PointMass &pm) {
  // Handle collisions with spheres.
  double dist = (pm.position - position).norm();
  if (abs(dist) <= radius) {
  	Vector3D tangent = position + (pm.position - position) / dist * radius;
    Vector3D correction = tangent - pm.last_position;
    pm.position = pm.last_position + (1 - friction) * (correction);
  }
}

void PointMass::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  Misc::draw_sphere(shader, position, radius);
}
