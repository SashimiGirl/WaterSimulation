#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
  // Handle point mass collisions with spheres.
  double dist = (pm.position - position).norm();
  if (abs(dist) <= radius) {
  	Vector3D tangent = position + (pm.position - position) / dist * radius;
    Vector3D correction = tangent - pm.last_position;
    pm.position = pm.last_position + (1 - friction) * (correction);
    Vector3D normalc = tangent - position;
    normalc.normalize();
    normalc = (1 + this->elasticity)* normalc * dot(pm.velocity, normalc);
    this->velocity -= normalc * pm.mass / mass;
    pm.velocity += normalc;
  }
  cout << this->velocity <<"\n";
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  //printf("Postion: (%f, %f, %f)\n", position.x, position.y, position.z);
  Misc::draw_sphere(shader, position, radius);
}
