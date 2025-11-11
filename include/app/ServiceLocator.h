#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "app/DebugLog.h"

/**
 * @file ServiceLocator.h
 * @brief グローバルサービスへのアクセスを提供するサービスロケータ
 * @author 山内陽
 * @date 2025
 * @version 6.0
 * 
 * @details
 * GfxDeviceやTextureManagerなどのグローバルなサービス（オブジェクト）を登録し、
 * アプリケーションのどこからでも静的にアクセスできるようにします。
 * これにより、オブジェクトを関数の引数で引き回す必要がなくなります。
 */

class ServiceLocator {
public:
    /**
     * @brief サービス（オブジェクト）を登録します。
     * @tparam T サービスの型
     * @param service 登録するサービスへのポインタ
     */
    template<typename T>
    static void Register(T* service) {
        if (service) {
            DEBUGLOG("Service registered: " + std::string(typeid(T).name()));
            services_[typeid(T)] = service;
        } else {
            DEBUGLOG_WARNING("Attempted to register a null service: " + std::string(typeid(T).name()));
        }
    }

    /**
     * @brief 登録されたサービスを取得します。
     * @tparam T 取得するサービスの型
     * @return T& サービスへの参照
     * @throws std::runtime_error サービスが登録されていない場合
     */
    template<typename T>
    static T& Get() {
        auto it = services_.find(typeid(T));
        if (it == services_.end() || it->second == nullptr) {
            std::string errorMsg = "Service not found or is null: " + std::string(typeid(T).name());
            DEBUGLOG_ERROR(errorMsg);
            throw std::runtime_error(errorMsg.c_str());
        }
        return *static_cast<T*>(it->second);
    }

    /**
     * @brief 登録されたサービスを安全に取得します。
     * @tparam T 取得するサービスの型
     * @return T* サービスへのポインタ（登録されていない場合はnullptr）
     */
    template<typename T>
    static T* TryGet() {
        auto it = services_.find(typeid(T));
        if (it == services_.end() || it->second == nullptr) {
            return nullptr;
        }
        return static_cast<T*>(it->second);
    }

    /**
     * @brief すべてのサービスをクリアします。
     * アプリケーション終了時に呼び出します。
     */
    static void Shutdown() {
        DEBUGLOG("ServiceLocator shutting down.");
        services_.clear();
    }

private:
    // 型情報とサービスへのポインタをマッピング
    inline static std::unordered_map<std::type_index, void*> services_;
};
