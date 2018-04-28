 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include "glm/glm.hpp"
 #include <vector>
 #include <stdio.h>
 #include <iostream>
 #include <stdlib.h>
 #include <math.h>

 class SolidSphere {
 public:
     SolidSphere()
     {};
     SolidSphere(float radius, unsigned int rings, unsigned int sectors);
     void draw(GLfloat x, GLfloat y, GLfloat z);
 private:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;

 };

