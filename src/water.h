#ifndef WATER_H
#define WATER_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "clothMesh.h"
#include "collision/collisionObject.h"
#include "spring.h"

using namespace CGL;
using namespace std;

struct WaterParameters {
  WaterParameters() {}
  WaterParameters(double damping,
                  double density, double ks)
      : damping(damping), density(density), ks(ks) {}
  ~WaterParameters() {}

  // liquid parameters
  double damping;
  double density;
  double ks;
  double viscosity;
};

struct Water {
  Water() {}
  Water(int num_length_points, int num_width_points, int num_height_points);
  ~Water();

  void buildVolume();

  void simulate(double frames_per_sec, double simulation_steps, WaterParameters *wp,
                vector<Vector3D> external_accelerations,
                vector<CollisionObject *> *collision_objects);

  void reset();

  void build_spatial_map();
  void buildSurfaceMesh();
  void self_collide(PointMass &pm, vector<PointMass *> &candidates);
  uint64_t hash_position(Vector3D pos);

  // Water properties
  int num_length_points;
  int num_width_points;
  int num_height_points;

  // Cloth components
  vector<PointMass> point_masses;
  ClothMesh *surfaceMesh;

  // Spatial hashing
  unordered_map<uint64_t, vector<PointMass *> *> map;
};

#endif /* WATER_H */
