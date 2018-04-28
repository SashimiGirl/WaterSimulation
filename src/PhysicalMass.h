#ifndef PHYSICALMASS_H
#define PHYSICALMASS_H

 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include "glm/glm.hpp"
 #include <vector>

// Forward declarations
class PhysicalMass {
  public:
    //PhysicalMass (double m, double f);
    void set_mass (double m) {mass = m;}
    double get_mass () {return mass;}
    void set_forces (glm::dvec3 f) {forces = f;}
    glm::dvec3 get_forces () {return forces;}
    void collision (PhysicalMass hit);


  private:
    double mass;
    glm::dvec3 forces;
};
#endif /* PHYSICALMASS_H */
