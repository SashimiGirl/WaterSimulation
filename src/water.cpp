#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "water.h"
#include "collision/plane.h"
#include "collision/sphere.h"

#define PARTICLE_RADIUS 0.01

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
            Vector3D(2 * PARTICLE_RADIUS * j, 2 * PARTICLE_RADIUS * i + 1.0, 2 * PARTICLE_RADIUS * k),
            PARTICLE_RADIUS);
      }
    }
  }

}

void Water::simulate(double frames_per_sec, double simulation_steps, WaterParameters *wp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  //double mass = wp->density;
  wp->damping = 0.2;
  double mass = 0.001;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;
  for (int i = 0; i < point_masses.size(); i++) {
    point_masses[i].forces = 0;
    for (int j = 0; j < external_accelerations.size(); j++) {
      //point_masses[i].forces += 0.001 * external_accelerations[0];
      point_masses[i].forces += mass * external_accelerations[j];
    }
  }

  /*
  build_spatial_map();
  for (PointMass &pm : point_masses) {
    self_collide(pm, simulation_steps);
  }*/
  /*
  printf("mass: %f\n", mass);

  printf("ext force: (%f, %f, %f)\n", 
    external_accelerations[0].x, external_accelerations[0].y,
    external_accelerations[0].z);

  printf("position: (%f, %f, %f)\n", point_masses[0].position.x, 
    point_masses[0].position.y, point_masses[0].position.z);

  printf("Force: (%f, %f, %f)\n", point_masses[0].forces.x, 
    point_masses[0].forces.y, point_masses[0].forces.z);
  */
  // Verlet integration to compute new point mass positions
  for (int i = 0; i < point_masses.size(); i++) {
    Vector3D old = point_masses[i].position;
    point_masses[i].position = point_masses[i].position
      + (1 - (wp->damping / 100)) 
      * (point_masses[i].position - point_masses[i].last_position)
      + point_masses[i].forces / mass * pow(delta_t, 2);
    point_masses[i].last_position = old;
  }


  for (PointMass &pm : point_masses) {
    for (CollisionObject *co : *collision_objects) {
      co->collide(pm);
    }
  }

  // TODO (Part 2.3): Constrain the changes to be such that the water particles do not change
  // in pos more than <simething> per timestep.
}

void Water::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  for (int i = 0; i < point_masses.size(); i++) {
    float code = hash_position(point_masses[i].position);
    if (map.find(code) == map.end()) {
      vector<PointMass *> *temp =  new vector<PointMass *>();
      temp->emplace_back(&point_masses[i]);
      map[code] = temp;
    } else {
      vector<PointMass *> *temp =  map[code];
      temp->emplace_back(&point_masses[i]);
    }
  }

}

void Water::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4.3): Handle self-collision for a given point mass.
  float code = hash_position(pm.position);
  vector<PointMass *> *can = (map.find(code)->second);
  
  int correctNum = 0;
  Vector3D correction = Vector3D(0,0,0);
  for (int i = 0; i < can->size(); i++) {
    float dist = (pm.position - (*can)[i]->position).norm();
    if (&pm != (*can)[i] &&  dist < 2 * PARTICLE_RADIUS) {
      /* Add pressure stuffs.
      Vector3D tangent = pm.position + (2 * thickness - dist)
        * (pm.position - (*can)[i]->position) / dist;
      correction += tangent - pm.last_position;
      correctNum++;*/
    }
  }
  
  if (correctNum > 0) {
    correction = 1.0/(correctNum * simulation_steps) * correction;
    pm.position = pm.last_position + correction ;
  }
}

float Water::hash_position(Vector3D pos) {
  // TODO (Part 4.1): Hash a 3D position into a unique float identifier that represents
  // membership in some uniquely identified 3D box volume.
  float t = 5 * PARTICLE_RADIUS;
  Vector3D bucket = Vector3D((int) floor(pos.x/t), (int) floor(pos.y/t), 
    (int) floor(pos.z/t));

  return (bucket.x * 31 + bucket.y) * 31 + bucket.z;
}

void Water::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
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
