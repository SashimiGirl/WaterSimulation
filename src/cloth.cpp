#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1.1): Build a grid of masses.
  for (int i = 0; i < num_height_points; i++) {
    for (int j = 0; j < num_width_points; j++) {
      if (orientation == HORIZONTAL) {
        point_masses.emplace_back(Vector3D(width / (double) num_width_points * i,
          1, height / (double) num_height_points * j), false);
      } else {
        double random = rand()%20;
        point_masses.emplace_back(Vector3D(width / (double)  num_width_points * i,
          height / (double) num_height_points * j,
          (random) / 10000.0 - 1.0 / 1000.), false);
      }
    }
  }
  /**
  for (int i = 0; i < pinned.size(); i++) {
    for (int j = 0; j < pinned[i].size(); j++) {
      point_masses[i * num_width_points + pinned[i][j]].pinned = true;
    }
  }

  // TODO (Part 1.2): Add springs 
  for (int i = 0; i < num_height_points; i++) {
    for (int j = 0; j < num_width_points; j++) {
      switch (i) {
        case 1: // Can do x == 0 && x == 1
          // Struct (0) case 2 (x, y) -> (x - 1, y)
          springs.emplace_back(&point_masses[i*num_width_points + j],
            &point_masses[(i - 1) * num_width_points + j], STRUCTURAL);
          if (j > 0) {
            // Sheer (1) case 1 (x, y) -> (x - 1, y - 1)
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[(i - 1) * num_width_points + j - 1], SHEARING);
          }
          if (j < num_width_points - 1) {
            // Sheer (1) case 2 (x, y) -> (x - 1, y + 1)
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[(i - 1) * num_width_points + j + 1], SHEARING);
          }
        case 0: // Can only do x == 0 cases
          if (j > 0) {
            // Struct (0) case 1 (x, y) -> (x, y - 1)  
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[i * num_width_points + j - 1], STRUCTURAL);
          }
          if (j >= 2) {
            // Bending (2) case 1 (x, y) -> (x, y - 2)  
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[i * num_width_points + j - 2], BENDING);
          }
          break;
        default:
          // Struct (0) case 2 (x, y) -> (x - 1, y)
          springs.emplace_back(&point_masses[i*num_width_points + j],
            &point_masses[(i - 1) * num_width_points + j], STRUCTURAL);
          // Bending (2) case 2 (x, y) -> (x - 2, y)
          springs.emplace_back(&point_masses[i*num_width_points + j],
            &point_masses[(i - 2) * num_width_points + j], BENDING);
          if (j >= 2) {
            // Bending (2) case 1 (x, y) -> (x, y - 2)
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[i * num_width_points + j - 2], BENDING);
          }
          if (j > 0) {
            // Struct (0) case 1 (x, y) -> (x, y - 1)  
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[i * num_width_points + j - 1], STRUCTURAL);
            // Sheer (1) case 1 (x, y) -> (x - 1, y - 1)
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[(i - 1) * num_width_points + j - 1], SHEARING);
          } 
          if (j < num_width_points - 1) {
            // Sheer (1) case 2 (x, y) -> (x - 1, y + 1)
            springs.emplace_back(&point_masses[i*num_width_points + j],
              &point_masses[(i - 1) * num_width_points + j + 1], SHEARING);
          }
          break;
      }
    }
  }*/
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2.1): Compute total force acting on each point mass.
  for (int i = 0; i < point_masses.size(); i++) {
    point_masses[i].forces = 0;
    for (int j = 0; j < external_accelerations.size(); j++) {
      point_masses[i].forces += mass * external_accelerations[j];
    }
  }
  for (int i = 0; i < springs.size(); i++) {
    double f = 0.0;
    switch(springs[i].spring_type) {
      case STRUCTURAL:
        if (cp->enable_structural_constraints) {
          f = cp->ks * 
           ((springs[i].pm_a->position - springs[i].pm_b->position).norm()
            - springs[i].rest_length);
        }
        break;
      case SHEARING:
        if (cp->enable_shearing_constraints) {
          f = cp->ks * 
           ((springs[i].pm_a->position - springs[i].pm_b->position).norm()
            - springs[i].rest_length);
        }
        break;
      case BENDING:
        if (cp->enable_bending_constraints) {
          f = cp->ks * 
           ((springs[i].pm_a->position - springs[i].pm_b->position).norm()
            - springs[i].rest_length);
        }
        break;
      default:
        break;
    }
    springs[i].pm_a->forces -= (springs[i].pm_a->position - springs[i].pm_b->position).unit() * f;
    springs[i].pm_b->forces += (springs[i].pm_a->position - springs[i].pm_b->position).unit() * f;
  }

  // TODO (Part 2.2): Use Verlet integration to compute new point mass positions
  for (int i = 0; i < point_masses.size(); i++) {
    if (!point_masses[i].pinned) {
      Vector3D old = point_masses[i].position;
      point_masses[i].position = point_masses[i].position
        + (1 - (cp->damping / 100)) 
        * (point_masses[i].position - point_masses[i].last_position)
        + point_masses[i].forces / mass * pow(delta_t, 2);
      point_masses[i].last_position = old;
    }
  }

  // This won't do anything until you complete Part 4.
  build_spatial_map();
  for (PointMass &pm : point_masses) {
    self_collide(pm, simulation_steps);
  }

  // This won't do anything until you complete Part 3.
  for (PointMass &pm : point_masses) {
    for (CollisionObject *co : *collision_objects) {
      co->collide(pm);
    }
  }

  // TODO (Part 2.3): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  for (int i = 0; i < springs.size(); i++) {
    double dist = (springs[i].pm_a->position 
      - springs[i].pm_b->position).norm();
    if (abs(dist) > springs[i].rest_length * 1.1) {
      if (springs[i].pm_a->pinned) {
        if (!springs[i].pm_b->pinned) {
          springs[i].pm_b->position += 
          ((springs[i].pm_a->position - springs[i].pm_b->position) / abs(dist))
            * (dist - (springs[i].rest_length * 1.1));
        }
      } else if (springs[i].pm_b->pinned) {
        springs[i].pm_a->position -= 
        ((springs[i].pm_a->position - springs[i].pm_b->position) / abs(dist))
          * (dist - (springs[i].rest_length * 1.1));
      } else {
        springs[i].pm_a->position -= 1/2 *
        ((springs[i].pm_a->position - springs[i].pm_b->position) / abs(dist))
          * (dist - (springs[i].rest_length * 1.1));  
        springs[i].pm_b->position += 1/2 *
        ((springs[i].pm_a->position - springs[i].pm_b->position) / abs(dist))
          * (dist - (springs[i].rest_length * 1.1));
      }
    }
  }
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4.2): Build a spatial map out of all of the point masses.
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

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4.3): Handle self-collision for a given point mass.
  float code = hash_position(pm.position);
  vector<PointMass *> *can = (map.find(code)->second);
  
  int correctNum = 0;
  Vector3D correction = Vector3D(0,0,0);
  for (int i = 0; i < can->size(); i++) {
    float dist = (pm.position - (*can)[i]->position).norm();
    if (&pm != (*can)[i] &&  dist < 2 * thickness) {
      Vector3D tangent = pm.position + (2 * thickness - dist)
        * (pm.position - (*can)[i]->position) / dist;
      correction += tangent - pm.last_position;
      correctNum++;
    }
  }
  
  if (correctNum > 0) {
    correction = 1.0/(correctNum * simulation_steps) * correction;
    pm.position = pm.last_position + correction ;
  }
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4.1): Hash a 3D position into a unique float identifier that represents
  // membership in some uniquely identified 3D box volume.
  float w = 3 * width / num_width_points;
  float h = 3 * height / num_height_points;
  float t = max(w, h);
  Vector3D bucket = Vector3D((int) floor(pos.x/w), (int) floor(pos.y/h), 
    (int) floor(pos.z/t));

  return (bucket.x * 31 + bucket.y) * 31 + bucket.z;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

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
  this->clothMesh = clothMesh;
}
