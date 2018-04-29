#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../waterSimulator.h"
#include "box.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Box::collide(PointMass &pm) {
  // TODO (Part 3.2): Handle collisions with planes.
  double t = dot((point - pm.last_position), normal) / dot((pm.position - pm.last_position).unit(), normal);
  if (t <= 0) {
    t = dot((point - pm.position), normal) / dot(normal, normal);
    if (t >= 0) {
      Vector3D tangent = pm.position + t * normal;
      Vector3D correction = (tangent - pm.last_position) + SURFACE_OFFSET * normal;
      //Vector3D correction = (tangent - pm.last_position) * (1 -SURFACE_OFFSET);
      pm.position = pm.last_position + (1 - friction) * correction;
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

  positions.col(0) << Vector3f(xshift + s, yshift, zshift);
  positions.col(1) << Vector3f(xshift + s, yshift, zshift + s);
  positions.col(2) << Vector3f(xshift, yshift, zshift);
  positions.col(3) << Vector3f(xshift, yshift, zshift + s);

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

}
