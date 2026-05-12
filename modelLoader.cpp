#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool ModelLoader::Load(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_LimitBoneWeights);

    if (!scene)
        return false;

    return true;
}