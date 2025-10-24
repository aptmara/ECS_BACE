#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <thread>

// Debug log macros
#ifdef _DEBUG
#define DEBUGLOG(message) DebugLog::GetInstance().Log(message)
#define DEBUGLOG_ERROR(message) DebugLog::GetInstance().LogError(message)
#define DEBUGLOG_WARNING(message) DebugLog::GetInstance().LogWarning(message)
#define DEBUGLOG_CATEGORY(category, message) DebugLog::GetInstance().LogWithCategory(category, message)
#else
#define DEBUGLOG(message) ((void)0)
#define DEBUGLOG_ERROR(message) ((void)0)
#define DEBUGLOG_WARNING(message) ((void)0)
#define DEBUGLOG_CATEGORY(category, message) ((void)0)
#endif

/**
 * @class DebugLog
 * @brief デバッグログユーティリティクラス
 * @author 山内陽
 * @date 2025
 *
 * @details
 * UTF-8 BOM対応、カテゴリ分類、スレッドID記録、フレーム計測を備えた
 * 強化されたデバッグログシステム
 */
class DebugLog {
public:
    /**
     * @enum Category
     * @brief ログのカテゴリ
     */
    enum class Category {
        General,   ///< 一般
        ECS,       ///< ECSシステム
        Render,    ///< レンダリング
        Input,     ///< 入力
        Graphics,  ///< グラフィックス
        Scene,     ///< シーン
        System     ///< システム
    };

    static DebugLog& GetInstance() {
        static DebugLog instance;
        return instance;
    }

    // フレームタグ設定（オプション）。メインスレッドからフレームごとに1回呼び出すことを想定。
    void SetFrame(uint64_t frame) {
        currentFrame_.store(frame, std::memory_order_relaxed);
    }

    // フレーム計測開始
    void BeginFrameTiming() {
        frameStartTime_ = std::chrono::high_resolution_clock::now();
    }

    // フレーム計測終了（Δt記録）
    void EndFrameTiming(float deltaTime) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - frameStartTime_;

        frameCount_++;
        totalTime_ += deltaTime;

        // 直近100フレームの移動平均
        recentFrameTimes_[frameCount_ % 100] = deltaTime;

        // 1000フレームごとに統計を出力
        if (frameCount_ % 1000 == 0) {
            float avgDt = totalTime_ / frameCount_;
            float avgFps = (avgDt > 0.0f) ? (1.0f / avgDt) : 0.0f;

            std::lock_guard<std::mutex> lock(mutex_);
            WriteLog("INFO", "フレーム統計", Category::System,
                     "Frames=" + std::to_string(frameCount_) +
                     ", AvgFPS=" + std::to_string(avgFps) +
                     ", AvgDt=" + std::to_string(avgDt * 1000.0f) + "ms");
        }
    }

    void Log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        WriteLog("INFO", message, Category::General);
    }

    void LogError(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        WriteLog("ERROR", message, Category::General);
    }

    void LogWarning(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        WriteLog("WARNING", message, Category::General);
    }

    void LogWithCategory(Category cat, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        WriteLog("INFO", message, cat);
    }

private:
    DebugLog() {
        // UTF-8 BOMで出力するため、バイナリモードで開く
        logFile_.open("debug_log.txt", std::ios::out | std::ios::trunc | std::ios::binary);
        if (logFile_.is_open()) {
            // UTF-8 BOMを書き込む
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            logFile_.write(reinterpret_cast<const char*>(bom), sizeof(bom));

            logFile_ << "========================================" << std::endl;
            logFile_ << "デバッグログ開始" << std::endl;
            logFile_ << "========================================" << std::endl;
            logFile_.flush();
        }
    }

    ~DebugLog() {
        if (logFile_.is_open()) {
            logFile_ << "========================================" << std::endl;
            logFile_ << "デバッグログ終了" << std::endl;
            logFile_ << "========================================" << std::endl;
            logFile_.close();
        }
    }

    const char* CategoryToString(Category cat) const {
        switch (cat) {
            case Category::ECS: return "ECS";
            case Category::Render: return "Render";
            case Category::Input: return "Input";
            case Category::Graphics: return "Graphics";
            case Category::Scene: return "Scene";
            case Category::System: return "System";
            default: return "General";
        }
    }

    void WriteLog(const std::string& level, const std::string& message, Category cat, const std::string& extra = "") {
        if (logFile_.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::tm bt{};
            localtime_s(&bt, &in_time_t);

            // フレームプレフィックス
            uint64_t frame = currentFrame_.load(std::memory_order_relaxed);

            // スレッドID取得
            auto threadId = std::this_thread::get_id();
            std::ostringstream oss;
            oss << threadId;

            logFile_ << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
                     << " [F#" << frame << "]"
                     << " [TID:" << oss.str() << "]"
                     << " [" << CategoryToString(cat) << "]"
                     << " [" << level << "] " << message;

            if (!extra.empty()) {
                logFile_ << " | " << extra;
            }

            logFile_ << std::endl;

            // 重要なログは即座にフラッシュ
            if (level == "ERROR" || level == "WARNING") {
                logFile_.flush();
            }
        }
    }

    DebugLog(const DebugLog&) = delete;
    DebugLog& operator=(const DebugLog&) = delete;

    std::ofstream logFile_;
    std::mutex mutex_;
    std::atomic<uint64_t> currentFrame_{0};

    // フレーム計測
    std::chrono::high_resolution_clock::time_point frameStartTime_;
    uint64_t frameCount_ = 0;
    float totalTime_ = 0.0f;
    float recentFrameTimes_[100] = {};
};
