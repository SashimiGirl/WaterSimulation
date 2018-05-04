#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../waterSimulator.h"
#include "box.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Box::collide(PointMass &pm) {
  // Handle collisions with box like sphere objects
  for (int i = 0; i < 5; i++) {
    double t = dot((pm.position - point[i]), normal[i]);
    if (t <= pm.radius) {
      //t = dot((point[i] - pm.position), normal[i])
      //  / dot(normal[i], normal[i]);
      //Vector3D tangent = pm.position + t * normal[i];
      // Little bounce?
      //pm.last_position = pm.position;
      pm.position += (1.0 + elasticity) * (dot(point[i] - pm.position, normal[i]) +  pm.radius + SURFACE_OFFSET) * normal[i];
      pm.position = (1 - friction) * (pm.position - pm.last_position) + pm.last_position;
      pm.velocity = (1 - friction) * (pm.velocity - normal[i] * dot(pm.velocity, normal[i]));
    }
  }
}

/*Sphere collision
void Sphere::collide(PointMass &pm) {
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
    this->velocity = bigboi + (this->elasticity * (vsboi-vfatboi) + vfatboi) * normalc;
    pm.velocity = lilbit + (this->elasticity * (vpboi-vfatboi) + vfatboi) * normalc;
  }
  //cout << this->velocity <<"\n";
}

*/

void Box::collide(CollisionObject &s) {
  // Handle sphere collisions with the box.
  for (int i = 0; i < 5; i++) {
    double t = dot((s.position - point[i]), normal[i]);
    if (t <= s.radius) {
      t = dot((point[i] - s.position), normal[i]);
      Vector3D tangent = s.position + t * normal[i];
      // Little bounce?
      s.last_position = s.position;
      s.position = tangent + normal[i] * s.radius;
    }
  }
}

void Box::render(GLShader &shader) {
  nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);

  /*
   * That box dohhh
   * (1, 1, 0) | (1, 1, 1)
   * (1, 0, 0) | (1, 0, 1)
   * (0, 0, 0) | (0, 0, 1)
   * (0, 1, 0) | (0, 1, 1)
  **/

  // Bottom plane
  MatrixXf positions(3, 4);
  MatrixXf normals(3, 4);
  Vector3f sNormal(0, 1, 0);

  // positions.col(0) << Vector3f(xshift + s, yshift, zshift);
  // positions.col(1) << Vector3f(xshift + s, yshift, zshift + s);
  // positions.col(2) << Vector3f(xshift, yshift, zshift);
  // positions.col(3) << Vector3f(xshift, yshift, zshift + s);
  //
  // normals.col(0) << sNormal;
  // normals.col(1) << sNormal;
  // normals.col(2) << sNormal;
  // normals.col(3) << sNormal;
  //
  //
  // if (shader.uniform("in_color", false) != -1) {
  //   shader.setUniform("in_color", color);
  // }
  //
  // shader.uploadAttrib("in_position", positions);
  // shader.uploadAttrib("in_normal", normals);
  //
  // shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);

/*

  // Left Plane
  sNormal = Vector3f(1, 0, 0);

  positions.col(0) << Vector3f(xshift, yshift, zshift);
  positions.col(1) << Vector3f(xshift, yshift, zshift + s);
  positions.col(2) << Vector3f(xshift, yshift + s, zshift);
  positions.col(3) << Vector3f(xshift, yshift + s, zshift + s);

  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;


  if (shader.uniform("in_color", false) != -1) {
    shader.setUniform("in_color", color);
  }

  shader.uploadAttrib("in_position", positions);
  shader.uploadAttrib("in_normal", normals);

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);

  // Right Plane
  sNormal = Vector3f(-1, 0, 0);

  positions.col(0) << Vector3f(xshift + s, yshift + s, zshift);
  positions.col(1) << Vector3f(xshift + s, yshift + s, zshift + s);
  positions.col(2) << Vector3f(xshift + s, yshift, zshift);
  positions.col(3) << Vector3f(xshift + s, yshift, zshift + s);

  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;


  if (shader.uniform("in_color", false) != -1) {
    shader.setUniform("in_color", color);
  }

  shader.uploadAttrib("in_position", positions);
  shader.uploadAttrib("in_normal", normals);

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);

  // Back Plane
  sNormal = Vector3f(0, 0, -1);

  positions.col(0) << Vector3f(xshift, yshift, zshift);
  positions.col(1) << Vector3f(xshift, yshift + s, zshift);
  positions.col(3) << Vector3f(xshift + s, yshift + s, zshift);
  positions.col(2) << Vector3f(xshift + s, yshift, zshift);


  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;


  if (shader.uniform("in_color", false) != -1) {
    shader.setUniform("in_color", color);
  }

  shader.uploadAttrib("in_position", positions);
  shader.uploadAttrib("in_normal", normals);

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);

  // Front Plane
  /* Commented out so that we can see in the box...
  sNormal = Vector3f(0, 0, 1);

  positions.col(0) << Vector3f(xshift, yshift, zshift + s);
  positions.col(1) << Vector3f(xshift, yshift + s, zshift + s);
  positions.col(3) << Vector3f(xshift + s, yshift + s, zshift + s);
  positions.col(2) << Vector3f(xshift + s, yshift, zshift + s);


  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;


  if (shader.uniform("in_color", false) != -1) {
    shader.setUniform("in_color", color);
  }

  shader.uploadAttrib("in_position", positions);
  shader.uploadAttrib("in_normal", normals);

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
*/
}
