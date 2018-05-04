/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Particles.cpp
 * Author: swl
 *
 * Created on April 15, 2016, 12:16 PM
 */

#include "Simulator.h"

#define VISUAL_RADIUS 0.015f
#define PARTICLE_RADIUS 0.02f
#define TARGET_REST 2.0f * PARTICLE_RADIUS
#define NEIGHBOR_RADIUS 2.0f * PARTICLE_RADIUS
#define KERNEL_RADIUS TARGET_REST
#define SPIKE_SCALE 15.0f
#define POLY_SCALE 315.0f
#define EPSILON 1.0f
#define SURFACE_OFFSET 0.0001f

SolidSphere Simulator::sphere = SolidSphere();

Simulator::Simulator(GLuint id, glm::vec3 gravity, WaterParameters *wp, float fps, float steps) :
  wp(wp), fps(fps), steps(steps), gravity(gravity), pid(id), container(Box(glm::vec3(0.0,-0.5,0.0), 1.0, 1.0, 0.2, 0.4)) {
  float mass = wp->density * 4 / 3 * M_PI * PARTICLE_RADIUS * PARTICLE_RADIUS * PARTICLE_RADIUS;
  int num_length_points = wp->num_length_points;
  int num_width_points = wp->num_width_points;
  int num_height_points = wp->num_height_points;
  int hash = 0;
  for (int k = -num_length_points/2; k < num_length_points/2; k++) {
    for (int i = -num_height_points/2; i < num_height_points/2; i++) {
      for (int j = -num_width_points/2; j < num_width_points/2; j++) {
        particles.emplace_back(
            glm::vec3(TARGET_REST * j + (double)rand() / RAND_MAX * TARGET_REST * 0.1,
                     TARGET_REST * i + (double)rand() / RAND_MAX * TARGET_REST * 0.1,
                     TARGET_REST * k + (double)rand() / RAND_MAX * TARGET_REST * 0.1),
            mass, PARTICLE_RADIUS, hash);
        hash += 1;
      }
    }
  }
  sphere = SolidSphere(VISUAL_RADIUS);
  paused = true;
}

void Simulator::glSphere(float x, float y, float z, GLuint pid) {
    Simulator::sphere.draw(x, y, z, pid);
}

void Simulator::PointMass::render(GLuint shader) {
  glSphere(position.x, position.y, position.z, shader);
}

void Simulator::Box::collide(PointMass &pm) {
  for (int i = 0; i < 5; i++) {
    float t = glm::dot((pm.position - point[i]), normal[i]);
    if (t <= pm.radius) {
      pm.position += (1.0f + elasticity) * (-t +  pm.radius + SURFACE_OFFSET) * normal[i];
      pm.position = (1.0f - friction) * (pm.position - pm.last_position) + pm.last_position;
      pm.velocity = (1.0f - friction) * (pm.velocity - normal[i] * glm::dot(pm.velocity, normal[i]));
    }
  }
}
void Simulator::Box::collide(Obj &o) {
}

void Simulator::Box::render(GLuint shader) {

}

void Simulator::render() {
    //glColorMaterial(GL_FRONT, GL_DIFFUSE);
    //glColor3f(0.2, 0.5, 0.8);
    //glColorMaterial(GL_FRONT, GL_SPECULAR);
    //glColor3f(0.9, 0.9, 0.9);
    //glColorMaterial(GL_FRONT, GL_AMBIENT);
    //glColor3f(0.2, 0.5, 0.8);
    GLuint loc = glGetUniformLocation(pid, "lPosition");
    glUniform3fv(loc, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
    loc = glGetUniformLocation(pid, "lIntensity");
    glUniform3fv(loc, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));


    for(Obj &o : objects) {
      o.render(this->pid);
    }
    for(PointMass &p : particles) {
      p.render(this->pid);
    }
}

void Simulator::step() {
  if (paused) {
    return;
  }
  float delta_t = 1.0f / (this->fps * this->steps);
  glm::vec3 waterf = this->particles[0].mass * this->gravity;
  //std::cout << waterf.y << "\n";
  for (PointMass &p : particles) {
    p.forces = waterf;
    p.last_position = p.position;
  }
  // Applying external forces (gravity) to objects
  for (Obj &o : objects) {
    o.forces = o.mass * this->gravity;
  }

  // Verlet integration to compute new point mass positions
  // Used to find predicted positions and velocities of the particles
  #pragma omp parallel for
  for (auto iter = particles.begin(); iter < particles.end(); iter++) {
    PointMass &p = iter[0];
    p.position += p.velocity * delta_t + 0.5f * p.forces / p.mass * delta_t * delta_t;
    //p.velocity *= 1 - wp->damping;
    p.velocity += p.forces / p.mass * delta_t;
    for (Obj &o : objects) {
      o.collide(p);
    }
    container.collide(p);
    p.tmp_position = p.position;
  }


  build_spatial_map();
  std::vector<std::vector<PointMass*> *> bs;
  for (auto &bucket : map) {
    bs.push_back(bucket.second);
  }




  //Assigning neighbors to each point mass for easy access and efficiency.
  //Stores it in neighbors vector in each particle.
  #pragma omp parallel for
  for (auto iter = bs.begin(); iter < bs.end(); iter++) {
    std::vector<PointMass*> bmasses = *(iter[0]);
    std::vector<PointMass*> candidates;
    self_collide((*bmasses[0]), candidates);

    for (PointMass* pm : bmasses) {
      pm->neighbors.clear();
      std::vector<PointMass*> close;
      for (PointMass* c : candidates) {
        glm::vec3 dist = pm->position - c->position;
        float distf = glm::l2Norm(dist);
        if (pm != c && distf < NEIGHBOR_RADIUS) {
          close.push_back(c);
        }
      }
      //delete &pm->neighbors;
      pm->neighbors = close;

    }
  }

  //THE WOWOOWOWOWOWOOWOWOWOOWOWOWOWOW
  //THE WOWOOWOWOWOWOOWOWOWOOWOWOWOWOW
  //THE WOWOOWOWOWOWOOWOWOWOOWOWOWOWOW
  for (int wow = 0; wow < 5; wow ++) {
    //Calculated the lambda at every point mass. Stores it in lambdas vector.
    #pragma omp parallel for
    for (auto iter = particles.begin(); iter < particles.end(); iter++) {
      PointMass &pboi = iter[0];
      float rh = pboi.mass * pointDensity(pboi);
      float C_i = rh/TARGET_REST - 1;
      float denom = 0; // The denominator of the lambda
      for (PointMass* i : pboi.neighbors) {// Case 1 k=j.
        denom += glm::length2(gradC(i, &pboi));
      }
      denom += glm::length2(gradC(&pboi, &pboi));//case 2 k=i
      //cout << denom << "\n";
      pboi.lambda = -C_i / (denom + EPSILON);
    }

    //Find delta p for every point and update the predicted positions.
    //Update the velocities. Notice that force is never used except for external forces.

    #pragma omp parallel for
    for (auto iter = particles.begin(); iter < particles.end(); iter++) {
      PointMass &p = iter[0];
      glm::vec3 dp = deltaP(p);
      p.position = p.tmp_position + dp / steps;
      for (Obj &o : objects) {
        o.collide(p);
      }
      container.collide(p);
      p.velocity = (p.position - p.last_position) / delta_t;
    }

    for (PointMass& p : particles) {
      p.tmp_position = p.position;
    }
  }


  // Verlet integration to compute new object positions
  for (Obj &o : objects) {
      glm::vec3 old = o.position;
      o.position += (o.velocity) * delta_t
        + 0.5f * o.forces / o.mass * delta_t * delta_t;
    //  o.last_position = old;
    // Check if object hit the box
      o.velocity += o.forces / o.mass * delta_t;
      //cout << co->position << "\n";
      container.collide(o);
  }
}


void Simulator::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  this->map.clear();
  //#pragma omp parallel for
  for (auto p = particles.begin(); p < particles.end(); p++) {
    uint64_t code = hash_position(p->position);
    //#pragma omp critical
    if (this->map.find(code) == this->map.end()) {
      std::vector<PointMass *> *temp =  new std::vector<PointMass *>();
      temp->emplace_back(&(p[0]));
      this->map[code] = temp;
    } else {
      this->map[code]->emplace_back(&(p[0]));
    }
  }

}

void Simulator::self_collide(PointMass &pm, std::vector<PointMass *> &candidates) {
  uint64_t code = hash_position(pm.position);
  uint64_t offsets[4] = { 0x000000000001, 0x000000010000, 0x000100000000, 0x000100010001};
  code = code - offsets[3];

  int n, m, p;
  for (int i = 0; i < 27; i++) {
    n = i%3;
    m = i%9/3;
    p = i/9;
    auto tmp = this->map.find(code + n * offsets[0]+ m * offsets[1] + p * offsets[2]);
    if (tmp != this->map.end()) {
      candidates.insert(candidates.end(), tmp->second->begin(), tmp->second->end());
    }
  }
}
//Spikey kernel
float Simulator::SPkernel(glm::vec3 in, float var, float scalar) {
  float tmp = glm::l2Norm(in);
  if (tmp > var) {
    return 0;
  }
  tmp = var - tmp;
  return scalar / (M_PI * pow(var, 6)) * tmp * tmp * tmp;
}
//Poly kernel
float Simulator::Pkernel(glm::vec3 in, float var, float scalar, float scalar2) {
  float n = glm::l2Norm(in);
  if (n > var) {
    return 0;
  }
  n = var*var - n*n;
  return scalar / (scalar2 * M_PI * pow(var, 9)) * n * n * n;
}
// derivative of spikey (gradient)
glm::vec3 Simulator::dSPkernel(glm::vec3 in, float var, float scalar) {
  float tmp = glm::l2Norm(in);
  if (tmp > var) {
    return glm::vec3();
  }
  tmp = var - tmp;
  return float(3 * scalar / (M_PI * pow(var, 6)) * tmp * tmp) * glm::normalize(in);
}
//Find the density at ever point. This is the C() function.
float Simulator::pointDensity(PointMass &pm) {
  float rho_i = Pkernel(glm::vec3(), KERNEL_RADIUS, POLY_SCALE, 64);
  for (PointMass* boi: pm.neighbors) {
    rho_i += Pkernel(pm.position - boi->position, KERNEL_RADIUS, POLY_SCALE, 64);
  }
  return rho_i;
}
// Find the delta p correction for zero pressure change.
glm::vec3 Simulator::deltaP(PointMass& p) {
  glm::vec3 result;
  for (PointMass* it : p.neighbors) {
    result += (p.lambda + it->lambda) * dSPkernel(p.position - it->position, KERNEL_RADIUS, SPIKE_SCALE);
  }
  result *= 1.0 / TARGET_REST;
  if (glm::length2(result) > 0.0001) {
    result = glm::normalize(result) * 0.01f;
  }
  return result;
}
//Find the gradient Ck
glm::vec3 Simulator::gradC(PointMass* pk, PointMass* pi) {
  if (pk == pi) {
    glm::vec3 gradient;
    for (PointMass* p : pk->neighbors) {
      gradient += dSPkernel(pi->position - p->position, KERNEL_RADIUS, SPIKE_SCALE);
    }
    return gradient / TARGET_REST;
  }
  else {
    return -dSPkernel(pi->position - pk->position, KERNEL_RADIUS, SPIKE_SCALE) / TARGET_REST;
  }
}


uint64_t Simulator::hash_position(glm::vec3 pos) {
  float t = NEIGHBOR_RADIUS;
  uint64_t result = (uint16_t) (floor(pos.x/t) + 32768);
  result = (result << 16) | (uint16_t) (floor(pos.x/t) + 32768);
  result = (result << 16) | (uint16_t) (floor(pos.x/t) + 32768);
  return result;
}

void Simulator::reset() {
  PointMass *pm = &particles[0];
  for (int i = 0; i < particles.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm->tmp_position = pm->start_position;
    pm->velocity = glm::vec3();
    pm++;
  }
  for (Obj &o : this->objects) {
    //o.last_position = o.start_position;
    o.position = o.start_position;
    //o.tmp_position = o.start_position;
    o.velocity = glm::vec3();
  }
}

void Simulator::pause() {
  this->paused = !this->paused;
}

void Simulator::load(std::string path)
{
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, 0);
  if(!scene) {
    printf("Unable to load mesh: %s\n", importer.GetErrorString());
  }

  for(int j = 0; j < scene->mNumMeshes; j += 1) {
    this->meshes.push_back(new MeshObject(scene->mMeshes[j]));
  }
}
