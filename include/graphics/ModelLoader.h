#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "components/ModelComponent.h"
#include "graphics/GfxDevice.h"
#include "graphics/TextureManager.h"
#include "app/DebugLog.h"

class ModelLoader {
public:
    static std::vector<ModelComponent> LoadModel(const std::string& filePath);

private:
    static void ProcessMesh(
        std::vector<ModelComponent>& modelComponents,
        aiMesh* mesh,
        const aiScene* scene,
        const std::string& directory,
        GfxDevice& gfx
    );

    static TextureManager::TextureHandle LoadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        const std::string& directory
    );
};