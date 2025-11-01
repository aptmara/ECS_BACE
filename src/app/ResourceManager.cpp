#include "app/ResourceManager.h"
#include "app/DebugLog.h"

const std::vector<ModelComponent>& ResourceManager::GetModel(const std::string& filePath) {
    static const std::vector<ModelComponent> kEmpty;

    auto it = modelCache_.find(filePath);
    if (it != modelCache_.end()) {
        DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "Model cache hit: " + filePath);
        return it->second;
    }

    DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "Model cache miss, loading: " + filePath);
    std::vector<ModelComponent> loadedModel = ModelLoader::LoadModel(filePath);

    if (loadedModel.empty()) {
        return kEmpty;
    }

    auto result = modelCache_.emplace(filePath, std::move(loadedModel));
    return result.first->second;
}

void ResourceManager::Clear() {
    DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "Clearing all cached resources.");
    modelCache_.clear();
}

