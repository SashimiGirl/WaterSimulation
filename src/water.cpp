#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "water.h"
#include "collision/sphere.h"

#define PARTICLE_RADIUS 0.01
#define TARGET_MIN 2 * PARTICLE_RADIUS
#define TARGET_MAX 3 * PARTICLE_RADIUS
#define TARGET_REST 2 * PARTICLE_RADIUS
#define REST_DENSITY 1
#define BIGCHIC 15
#define BIGCHIC2 315
#define BIGCHIC3 64
#define EPSILON 0.1
using namespace std;

Water::Water(int num_length_points, int num_width_points, int num_height_points) {

  this->num_length_points = num_length_points;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;

  buildVolume();
  buildSurfaceMesh();
}

Water::~Water() {
  point_masses.clear();
  if (surfaceMesh) {
    delete surfaceMesh;
  }
}

void Water::buildVolume() {
  // TODO (Part 1.1): Build a grid of masses.
  for (int k = 0; k < num_length_points; k++) {
    for (int i = 0; i < num_height_points; i++) {
      for (int j = 0; j < num_width_points; j++) {
        point_masses.emplace_back(
            Vector3D(TARGET_MIN * j, TARGET_MIN * i + 1.0, TARGET_MIN * k),
            PARTICLE_RADIUS, k * num_width_points * num_height_points + i * num_width_points + j);
      }
    }
  }

}

void Water::simulate(double frames_per_sec, double simulation_steps, WaterParameters *wp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects,
                     Box *container) {
  this->lambdas.clear();
  double r = TARGET_REST / 2;
  double mass = wp->density * 4 / 3 * M_PI * r * r * r;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;
  Vector3D ef = Vector3D();
  for (int j = 0; j < external_accelerations.size(); j++) {
    ef += external_accelerations[j];
  }
  Vector3D waterf = mass * ef;
  for (PointMass &p : point_masses) {
    p.forces = waterf;
    p.mass = mass;
    p.last_position = p.position;
    p.last_velocity = p.velocity;
  }
  // Applying external forces (gravity) to objects
  for (auto co : *collision_objects) {
    co->forces = ef * co->mass;
  }
  //Move verlet here for sph

  build_spatial_map();
  vector<vector<PointMass*> *> bs;
  for (auto &bucket : map) {
    bs.push_back(bucket.second);
  }
  /**
  #pragma omp parallel for
  for (auto iter = bs.begin(); iter < bs.end(); iter++) {
    vector<PointMass*> bmasses = *(iter[0]);
    vector<PointMass*> candidates;
    self_collide((*bmasses[0]), candidates);
    for (PointMass* pm : bmasses) {
      float pressure = 0.0;
      for (PointMass* c : candidates) {
        Vector3D dist = pm->last_position- c->last_position;

        float distf = dist.norm();
        if (pm != c && distf < KERNEL_DISTANCE) {
          pressure +=  mass * SPkernel(dist, KERNEL_DISTANCE, 15);//SPkernel(dist, 0.8, 15)
        }
      }
      this->pressures[pm->hash] = pressure;
    }
  }

  for (PointMass &p : point_masses) {
    vector<PointMass*> candidates;
    vector<PointMass*> neighbors;
    Vector3D correction;
    float pressure = 0.0;
    p.position = p.last_position + p.velocity * delta_t + 0.5 * p.forces / mass * delta_t * delta_t;

    hash_collide(hash_position(p.position), candidates);
    for (PointMass* c : candidates) {
      Vector3D dist = c->last_position - p.position;

      float distf = dist.norm() - TARGET_PRESSURE;
      if (p.hash != c->hash && distf < PARTICLE_RADIUS) {
        neighbors.push_back(c);
        pressure += distf / TARGET_PRESSURE;
      }
    }
    for (PointMass* n : neighbors) {
      Vector3D duck = p.position - n->position;
      duck.normalize();
      correction +=  0.05 * (this->pressures[n->hash] - pressure) * duck;
      //dSPkernel((pressure - this->pressures[n->hash]), 0.1, 15) *
    }
    p.position += correction;
    p.velocity = (p.position - p.last_position) / delta_t;
  }
**/

  #pragma omp parallel for
  for (auto iter = bs.begin(); iter < bs.end(); iter++) {
    vector<PointMass*> bmasses = *(iter[0]);
    vector<PointMass*> candidates;
    self_collide((*bmasses[0]), candidates);

    for (PointMass* pm : bmasses) {
      /*
      float r1 = (float) rand() / (RAND_MAX)*0.01;
      float r2 = (float) rand() / (RAND_MAX)*0.01;
      float r3 = (float) rand() / (RAND_MAX)*0.01;
      Vector3D pressure = Vector3D(r1,r2,r3);
      Vector3D correction = Vector3D();
      int num = 0;
      for (PointMass* c : candidates) {
        Vector3D dir = c->position - pm->position;
        if (c != pm && dir.norm2() < PARTICLE_RADIUS * PARTICLE_RADIUS) {
          dir.normalize();
          num += 1;
          Vector3D tmp = c->position - dir * PARTICLE_RADIUS;
          correction += tmp - pm->position;
        }
      }
      if (num > 0) {
        pm->position += correction / (num * simulation_steps);
      }*/

      vector<PointMass*> close;
      for (PointMass* c : candidates) {
        Vector3D dist = pm->position - c->position;

        float distf = dist.norm();
        if (pm != c && distf < TARGET_MIN) {
          // Add pressure stuffs.
          dist.normalize();
          float pmcomp = dot(pm->last_velocity, dist);
          float ccomp = dot(c->last_velocity, dist);
          if (ccomp > pmcomp) {
            pm->velocity += (1 - pm->friction) * (ccomp - pmcomp) * dist;
          }
          //pressure += dist * wp->ks * mass * distf;
        }
        else if (pm != c && distf < TARGET_MAX) {
          close.push_back(c);
        }
      }
      pm->neighbors = close;
      float r1 = (float) rand() / (RAND_MAX)*0.0001 - 0.00005;
      float r2 = (float) rand() / (RAND_MAX)*0.0001 - 0.00005;
      float r3 = (float) rand() / (RAND_MAX)*0.0001 - 0.00005;
      Vector3D tension = Vector3D(r1, r2, r3);
      for (int i=0; close.size() > 0 && i < 4; i++) {
        int hbond = (float) rand() / (RAND_MAX) * close.size();
        Vector3D dist = close[hbond]->position - pm->position;
        float distf = dist.norm2();
        dist.normalize();
        tension += wp->ks * mass * mass * TARGET_MIN * dist / distf;
        //close[hbond]->forces -= tentemp;
        //cout << distf;
      }

      pm->forces += tension;
    }
  }
  for (PointMass &pboi : point_masses) {
    float rh = mass * pointDensity(pboi);
    float C_i = rh/REST_DENSITY - 1;
    float denom = 0;
    float part = 0;
    for (PointMass &i : pboi.neighbors) {
      float ker = SPkernel(pboi.position - i.position, TARGET_MAX, BIGCHIC);
      denom += ker*ker;
      part += ker;
    }
    denom += part * part;
    lambda[pboi.hash] = -C_i/(denom+EPSILON);
  }
  // Verlet integration to compute new object positions
  for (CollisionObject* co : *collision_objects) {
      Vector3D old = co->position;
      co->position += (co->velocity) * delta_t
        + 0.5 * co->forces / co->mass * delta_t * delta_t;
      co->last_position = old;
    // Check if object hit the box
      co->velocity += co->forces / co->mass * delta_t;
      //cout << co->position << "\n";
      container->collide(*co);
  }


  // Verlet integration to compute new point mass positions
  #pragma omp parallel for
  for (auto iter = point_masses.begin(); iter < point_masses.end(); iter++) {
    PointMass &p = iter[0];
    p.position += p.velocity * delta_t + 0.5 * p.forces / mass * delta_t * delta_t;
    //p.velocity *= 1 - wp->damping;
    p.velocity += p.forces / mass * delta_t;
    for (CollisionObject *co : *collision_objects) {
      co->collide(p);
    }
    container->collide(p);
  }


  // Applying collisions to each point mass to object
  // #pragma omp parallel for
  // for (iter = point_masses.begin(); iter < point_masses.end(); iter++) {
  //
  // }
}

void Water::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();
  //#pragma omp parallel for
  for (auto p = point_masses.begin(); p < point_masses.end(); p++) {
    uint64_t code = hash_position(p->position);
    //#pragma omp critical
    if (map.find(code) == map.end()) {
      vector<PointMass *> *temp =  new vector<PointMass *>();
      temp->emplace_back(&(p[0]));
      map[code] = temp;
    } else {
      map[code]->emplace_back(&(p[0]));
    }
  }

}

void Water::self_collide(PointMass &pm, vector<PointMass *> &candidates) {
  uint64_t code = hash_position(pm.position);
  uint64_t offsets[4] = { 0x000000000001, 0x000000010000, 0x000100000000, 0x000100010001};
  code = code - offsets[3];

  int n, m, p;
  for (int i = 0; i < 27; i++) {
    n = i%3;
    m = i%9/3;
    p = i/9;
    auto tmp = map.find(code + n * offsets[0]+ m * offsets[1] + p * offsets[2]);
    if (tmp != map.end()) {
      candidates.insert(candidates.end(), tmp->second->begin(), tmp->second->end());
    }
  }
}

void Water::hash_collide(uint64_t hash, vector<PointMass *> &candidates) {
  uint64_t offsets[4] = { 0x000000000001, 0x000000010000, 0x000100000000, 0x000100010001};
  hash = hash - offsets[3];

  int n, m, p;
  for (int i = 0; i < 27; i++) {
    n = i%3;
    m = i%9/3;
    p = i/9;
    auto tmp = map.find(hash + n * offsets[0]+ m * offsets[1] + p * offsets[2]);
    if (tmp != map.end()) {
      candidates.insert(candidates.end(), tmp->second->begin(), tmp->second->end());
    }
  }
}
float Water::SPkernel(Vector3D in, float var, float scalar) {
  float tmp = var - in.norm();
  return scalar / (M_PI * pow(var, 6)) * tmp * tmp * tmp;
}
float Water::PKernel(Vector 3D in, float var, float scalar, float scalar2) {
  float n = in.norm();
  float tmp = var*var - n*n;
  return scalar / (scalar2 * M_PI * pow(var, 9)) * tmp * tmp * tmp;
}
Vector3D Water::dSPkernel(Vector3D in, float var, float scalar) {
  float tmp = var - in.norm();
  in.normalize();
  return 3 * scalar / (M_PI * pow(var, 6)) * tmp * tmp * in;
}
float Water::pointDensity(PointMass &pm) {
  float rho_i = 0;
  for (PointMass &boi: pm.neighbors) {
    rho_i += PKernel(pm.position - boi.position, TARGET_MAX, BIGCHIC2, BIGCHIC3);
  }
  return rho_i;
}


uint64_t Water::hash_position(Vector3D pos) {
  // TODO (Part 4.1): Hash a 3D position into a unique float identifier that represents
  // membership in some uniquely identified 3D box volume.
  float t = TARGET_MAX;
  uint64_t result = (uint16_t) (floor(pos.x/t) + 32768);
  result = (result << 16) | (uint16_t) (floor(pos.x/t) + 32768);
  result = (result << 16) | (uint16_t) (floor(pos.x/t) + 32768);
  return result;
}

void Water::reset(vector<CollisionObject *> *collision_objects) {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm->velocity = Vector3D();
    pm->last_velocity = Vector3D();
    pm++;
  }
  for (int i = 0; i < collision_objects->size(); i++) {
    (*collision_objects)[i]->last_position =
        (*collision_objects)[i]->start_position;
    (*collision_objects)[i]->position =
        (*collision_objects)[i]->start_position;
    (*collision_objects)[i]->velocity = Vector3D();
  }
}


void Water::buildSurfaceMesh() { //FIXME:Not ready
  if (point_masses.size() == 0) return;
  /*
  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm, pm + num_width_points, pm + 1));
      triangles.push_back(new Triangle(pm + 1, pm + num_width_points,
                                       pm + num_width_points + 1));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->surfaceMesh = clothMesh;
  */
  this->surfaceMesh = new ClothMesh();
}
