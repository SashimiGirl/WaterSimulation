#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm, bool flag) {
  // Handle point mass collisions with spheres.
  double dist = (pm.position - position).norm();
  if (abs(dist) <= radius) {
    Vector3D tangent = position + (pm.position - position) / dist * radius;
    Vector3D correction = tangent - pm.last_position;
    pm.position = pm.last_position + (1 - friction) * (correction);
    Vector3D normalc = tangent - position;
    normalc.normalize();
    double summting = 1.0 / (mass + pm.mass);
    double vp = dot(normalc, pm.velocity);
    double vs = dot(normalc, this->velocity);
    double vfatboi = (pm.mass * vp + mass * vs) *summting;
    double vpboi = (pm.mass - mass) * summting * vp + 2 * mass * summting * vs;
    double vsboi = 2 * pm.mass * summting * vp + (mass - pm.mass) * summting * vs;

    //normalc = * normalc * dot(mass * this->velocity + pm.mass * pm.velocity, normalc);
    Vector3D bigboi = this->velocity - vs*normalc;
    Vector3D lilbit = pm.velocity - vp*normalc;
    if (flag) {
      this->velocity =  (bigboi + (this->elasticity * (vsboi-vfatboi) + vfatboi) * normalc);
    }
    pm.velocity = lilbit + (this->elasticity * (vpboi-vfatboi) + vfatboi) * normalc;

  }
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  //printf("Postion: (%f, %f, %f)\n", position.x, position.y, position.z);
  //Misc::draw_sphere(shader, position, radius);
  Misc::draw_sphere(shader, position, radius);
}
