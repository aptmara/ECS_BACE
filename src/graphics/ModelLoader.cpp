#include "graphics/ModelLoader.h"
#include "app/ServiceLocator.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

// 頂点構造体 (PositionとTexCoordのみ)
struct SimpleVertex {
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 TexCoord;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT3 Tangent;
    DirectX::XMFLOAT3 Bitangent;
};

std::vector<ModelComponent> ModelLoader::LoadModel(const std::string& filePath)
{
    auto& gfx = ServiceLocator::Get<GfxDevice>();
    auto& texMgr = ServiceLocator::Get<TextureManager>();
    std::vector<ModelComponent> modelComponents;
    Assimp::Importer importer;

    // モデルをロード
    // aiProcess_Triangulate: 全てのプリミティブを三角形に変換
    // aiProcess_FlipUVs: UV座標を反転 (DirectXの慣例に合わせる)
    // aiProcess_GenNormals: 法線がなければ生成
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    // エラーチェック
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        DEBUGLOG_ERROR("Assimp Error: " + std::string(importer.GetErrorString()));
        return modelComponents;
    }

    // ファイルパスからディレクトリを抽出
    std::string directory = filePath.substr(0, filePath.find_last_of('/'));
    if (directory.empty()) {
        directory = filePath.substr(0, filePath.find_last_of('\\'));
    }

    // シーンのルートノードから再帰的に処理を開始
    // 現状はルートノード直下のメッシュのみを処理する簡易実装
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        ProcessMesh(modelComponents, mesh, scene, directory, gfx);
    }

    DEBUGLOG_CATEGORY(DebugLog::Category::Render, "Model loaded: " + filePath + ", Meshes: " + std::to_string(modelComponents.size()));
    return modelComponents;
}

void ModelLoader::ProcessMesh(
    std::vector<ModelComponent>& modelComponents,
    aiMesh* mesh,
    const aiScene* scene,
    const std::string& directory,
    GfxDevice& gfx
) {
    std::vector<SimpleVertex> vertices;
    std::vector<unsigned short> indices;

    // 頂点データを処理
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        SimpleVertex vertex;
        vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        
        // テクスチャ座標が存在する場合
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        } else {
            vertex.TexCoord = { 0.0f, 0.0f };
        }

        if (mesh->mNormals) {
            vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        } else {
            vertex.Normal = { 0.0f, 1.0f, 0.0f }; // Default normal if not present
        }

        if (mesh->mTangents) {
            vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        }

        if (mesh->mBitangents) {
            vertex.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
        }
        vertices.push_back(vertex);
    }

    // インデックスデータを処理
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(static_cast<unsigned short>(face.mIndices[j]));
        }
    }

    // ModelComponentを作成
    ModelComponent mc;
    mc.indexCount = static_cast<UINT>(indices.size());

    // 頂点バッファの作成
    D3D11_BUFFER_DESC vbd{};
    vbd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(SimpleVertex));
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA vinit{ vertices.data(), 0, 0 };
    if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, mc.vertexBuffer.GetAddressOf()))) {
        DEBUGLOG_ERROR("Failed to create vertex buffer for model.");
        return;
    }

    // インデックスバッファの作成
    D3D11_BUFFER_DESC ibd{};
    ibd.ByteWidth = static_cast<UINT>(indices.size() * sizeof(unsigned short));
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA iinit{ indices.data(), 0, 0 };
    if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, mc.indexBuffer.GetAddressOf()))) {
        DEBUGLOG_ERROR("Failed to create index buffer for model.");
        return;
    }

    // マテリアルを処理
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 現時点ではDiffuseテクスチャのみをロード
        mc.texture = LoadMaterialTextures(material, aiTextureType_DIFFUSE, directory);
        mc.normalTexture = LoadMaterialTextures(material, aiTextureType_NORMALS, directory);

        // マテリアルから色情報を取得 (Ambient/Diffuse/Specularなど、ここではDiffuseを代表として使用)
        aiColor3D color (0.f,0.f,0.f);
        if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            mc.color = {color.r, color.g, color.b};
        }
    }

    modelComponents.push_back(mc);
}

TextureManager::TextureHandle ModelLoader::LoadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    const std::string& directory
) {
    auto& texMgr = ServiceLocator::Get<TextureManager>();
    TextureManager::TextureHandle textureHandle = TextureManager::INVALID_TEXTURE;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string filename = str.C_Str();
        
        // テクスチャパスを構築 (モデルファイルと同じディレクトリを基準)
        std::string fullPath = directory + "/" + filename;
        
        // テクスチャマネージャーでロード
        textureHandle = texMgr.LoadFromFile(fullPath.c_str());
        if (textureHandle != TextureManager::INVALID_TEXTURE) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "Loaded texture: " + fullPath);
            break; // 最初のテクスチャのみを使用
        } else {
            DEBUGLOG_WARNING("Failed to load texture: " + fullPath);
        }
    }
    return textureHandle;
}