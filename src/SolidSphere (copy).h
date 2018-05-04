#include "Mesh.h"

#define SPHERE_NUM_LAT 4
#define SPHERE_NUM_LON 7

#define SPHERE_NUM_VERTICES ((SPHERE_NUM_LAT + 1) * (SPHERE_NUM_LON + 1))
#define SPHERE_NUM_INDICES (6 * SPHERE_NUM_LAT * SPHERE_NUM_LON)
#define SINDEX(x, y) ((x) * (SPHERE_NUM_LON + 1) + (y))
#define VERTEX_SIZE 8
#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5
#define BUMP_FACTOR 1


class SolidSphere : public Mesh {
public:
  SolidSphere() : Mesh() {};
  SolidSphere(float radius);
  void draw(GLfloat x, GLfloat y, GLfloat z, GLuint pid) override;
   
private:
  float r;
  unsigned int Indices[SPHERE_NUM_INDICES];
  double Vertices[VERTEX_SIZE * SPHERE_NUM_VERTICES];
  //static double vertices[3 * SPHERE_NUM_VERTICES];
  //static double normals[3 * SPHERE_NUM_VERTICES];
};

