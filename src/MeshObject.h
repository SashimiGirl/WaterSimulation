#include "Mesh.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class MeshObject : public Mesh {
public:
  MeshObject(aiMesh *mesh);
protected:
  GLuint vao;
};
