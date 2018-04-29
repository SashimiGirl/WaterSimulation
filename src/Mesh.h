 #ifndef MESH_H
 #define MESH_H
 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include <vector>
 #include <stdio.h>
 #include <iostream>
 #include <stdlib.h>
 #include <math.h>
 #include "glm/glm.hpp"
 #include "glm/gtc/type_ptr.hpp"
 #include "glm/gtc/matrix_transform.hpp"
 

 class Mesh {
 public:
     Mesh() {};
     virtual void draw(GLfloat x, GLfloat y, GLfloat z, GLuint pid) {};
 protected:
    enum BUFFERS {
			VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER
		};
    GLuint vao;
		GLuint vbo[4];
		unsigned int elementCount;
		/**
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;
    **/
 };
 #endif
