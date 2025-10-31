#include "app/ResourceManager.h"
#include "app/DebugLog.h"

std::vector<ModelComponent> ResourceManager::GetModel(const std::string& filePath) {
    // キャッシュを検索
    auto it = modelCache_.find(filePath);
    if (it != modelCache_.end()) {
        DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "Model cache hit: " + filePath);
        // キャッシュが見つかったら、そのコピーを返す
        return it->second;
    }

    DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "Model cache miss, loading: " + filePath);
    // キャッシュになければロード
    std::vector<ModelComponent> loadedModel = ModelLoader::LoadModel(filePath);

    // ロードに成功したらキャッシュに保存
    if (!loadedModel.empty()) {
        modelCache_[filePath] = loadedModel;
    }

    return loadedModel;
}

void ResourceManager::Clear() {
    DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "Clearing all cached resources.");
    modelCache_.clear();
}
