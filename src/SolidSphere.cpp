#include "SolidSphere.h"


SolidSphere::SolidSphere (float radius)
{
  this->r = radius;
  glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	for (int i = 0; i <= SPHERE_NUM_LAT; i++) {
    for (int j = 0; j <= SPHERE_NUM_LON; j++) {
      double lat = ((double)i) / SPHERE_NUM_LAT;
      double lon = ((double)j) / SPHERE_NUM_LON;
      double *vptr = &Vertices[VERTEX_SIZE * SINDEX(i, j)];

      vptr[TCOORD_OFFSET + 0] = lon;
      vptr[TCOORD_OFFSET + 1] = 1 - lat;

      lat *= M_PI;
      lon *= 2 * M_PI;
      double sinlat = sin(lat);

      vptr[NORMAL_OFFSET + 0] = vptr[VERTEX_OFFSET + 0] = sinlat * sin(lon);
      vptr[NORMAL_OFFSET + 1] = vptr[VERTEX_OFFSET + 1] = cos(lat),
                           vptr[NORMAL_OFFSET + 2] = vptr[VERTEX_OFFSET + 2] =
                               sinlat * cos(lon);
    }
  }

  for (int i = 0; i < SPHERE_NUM_LAT; i++) {
    for (int j = 0; j < SPHERE_NUM_LON; j++) {
      unsigned int *iptr = &Indices[6 * (SPHERE_NUM_LON * i + j)];

      unsigned int i00 = SINDEX(i, j);
      unsigned int i10 = SINDEX(i + 1, j);
      unsigned int i11 = SINDEX(i + 1, j + 1);
      unsigned int i01 = SINDEX(i, j + 1);

      iptr[0] = i00;
      iptr[1] = i10;
      iptr[2] = i11;
      iptr[3] = i11;
      iptr[4] = i01;
      iptr[5] = i00;
    }
  }
  GLfloat vertices[3 * SPHERE_NUM_INDICES];
  GLfloat normals[3 * SPHERE_NUM_INDICES];
  GLfloat* v = vertices;
  GLfloat* n = normals;
  for (int i = 0; i < SPHERE_NUM_INDICES; i += 3) {
    double *vPtr1 = &Vertices[VERTEX_SIZE * Indices[i]];
    double *vPtr2 = &Vertices[VERTEX_SIZE * Indices[i + 1]];
    double *vPtr3 = &Vertices[VERTEX_SIZE * Indices[i + 2]];

    glm::dvec3 p1(vPtr1[VERTEX_OFFSET], vPtr1[VERTEX_OFFSET + 1],
                vPtr1[VERTEX_OFFSET + 2]);
    glm::dvec3 p2(vPtr2[VERTEX_OFFSET], vPtr2[VERTEX_OFFSET + 1],
                vPtr2[VERTEX_OFFSET + 2]);
    glm::dvec3 p3(vPtr3[VERTEX_OFFSET], vPtr3[VERTEX_OFFSET + 1],
                vPtr3[VERTEX_OFFSET + 2]);

    glm::dvec3 n1(vPtr1[NORMAL_OFFSET], vPtr1[NORMAL_OFFSET + 1],
                vPtr1[NORMAL_OFFSET + 2]);
    glm::dvec3 n2(vPtr2[NORMAL_OFFSET], vPtr2[NORMAL_OFFSET + 1],
                vPtr2[NORMAL_OFFSET + 2]);
    glm::dvec3 n3(vPtr3[NORMAL_OFFSET], vPtr3[NORMAL_OFFSET + 1],
                vPtr3[NORMAL_OFFSET + 2]);

    *v++ = p1.x; *v++ = p1.y; *v++ = p1.z;
    *v++ = p2.x; *v++ = p2.y; *v++ = p2.z;
    *v++ = p3.x; *v++ = p3.y; *v++ = p3.z;

    *n++ = n1.x; *n++ = n1.y; *n++ = n1.z;
    *n++ = n2.x; *n++ = n2.y; *n++ = n2.z;
    *n++ = n3.x; *n++ = n3.y; *n++ = n3.z;
  }
  
  //shader.uploadAttrib("in_position", positions);
  //shader.uploadAttrib("in_normal", normals);
  
	glGenBuffers(1, &vbo[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, 3 * SPHERE_NUM_INDICES * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &vbo[NORMAL_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, 3 * SPHERE_NUM_INDICES * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	/**
	glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, 2 * faces * sizeof(GLfloat), tt, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray (vao, 1);
	delete[] tt;**

	delete[] n;
	delete[] nbc;
	
	unsigned int *i = new unsigned int[faces * 4];
	for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
    *i++ = r * sectors + s;
    *i++ = r * sectors + (s+1);
    *i++ = (r+1) * sectors + (s+1);
    *i++ = (r+1) * sectors + s;
  }

	glGenBuffers(1, &vbo[INDEX_BUFFER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * faces * sizeof(GLuint), i, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray (3);
	delete[] i;
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	*/
}

void SolidSphere::draw (GLfloat x, GLfloat y, GLfloat z, GLuint pid)
{
    //glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    //glTranslatef(x,y,z);
    glBindVertexArray(vao);
    GLuint loc = glGetUniformLocation(pid, "modelMatrix");
    glm::mat4 model_matrix(r, 0, 0, 0, 0, r, 0, 0, 0, 0, r, 0, x, y, z, 1);
    //glm::mat4 model_matrix = glm::translate(glm::mat4(r), glm::vec3(x, y, z));
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_matrix)); 
    
    glEnableVertexAttribArray (0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	  
	  glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
	  glEnableVertexAttribArray (2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glDrawArrays(GL_TRIANGLES, 0, 3 * SPHERE_NUM_INDICES);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(2);
    
    /*
    glBindVertexArray(vao);
    GLuint loc = glGetUniformLocation(pid, "modelMatrix");
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_matrix)); 
    
	  glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);
	  glBindVertexArray(0);
	  /**
    glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
    glNormalPointer(GL_FLOAT, 0, &normals[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
    **/
    //glPopMatrix();
}
