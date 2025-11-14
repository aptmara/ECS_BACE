/**
 * @file StageComponents.h
 * @brief ステージ進行用のタグと状態コンポーネント
 */
#pragma once

#include "components/Component.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

/**
 * @struct StartTag
 * @brief ステージの開始地点を示すタグ
 */
struct StartTag : IComponent {};

/**
 * @struct GoalTag
 * @brief ステージのゴール地点を示すタグ
 */
struct GoalTag : IComponent {};

/**
 * @struct StageProgress
 * @brief ステージ番号と進行フラグを管理
 */
struct StageProgress : IComponent {
    int currentStage = 1;
    bool requestAdvance = false;
};

/**
 * @struct StageCreate
 * @brief CSVファイルを読み込み、それを基にステージを生成
 */
struct StageCreate : IComponent {
    //ファイルの宣言
    ifstream m_file;

    //ステージデータを保持するための二次元配列
    vector<vector<int>> stageMap;

    StageCreate() {
        m_file.open("Assets/StageData/aaa.csv");         //パスの指定
        //ファイルが開けなかった時の処理（エラーログ等）
        if (!m_file.is_open()) {
            cerr << "ファイルの読み込みに失敗しました。" << endl;
        } else {
            //ファイルが開いている場合、コンストラクタ内でデータを読み込む
            loadStageData();
        }
    }

    void loadStageData() {
        string line;
        while (getline(m_file, line)) {
            //1行分のデータを格納するベクタ
            vector<int> row;
            //カンマ区切りでパース
            stringstream sstream(line);
            string cell;

            while(getline(sstream, cell,',')){
                try {
                    //文字列を整数に変換してベクタに追加
                    row.push_back(stoi(cell));
                } catch (const std::invalid_argument &error) {
                    //数値変換エラーの処理
                    cerr << "Invalid number in CSV: " << cell << endl;
                } catch (const std::out_of_range &error) {
                    //範囲外エラーの処理
                    cerr << "Number out of range in CSV: " << cell << endl;
                }
            }
            //処理した行をベクターに格納
            stageMap.push_back(row);
        }
        m_file.close();
    }

    StageCreate(const StageCreate &) = delete;
    StageCreate& operator=(const StageCreate&) = delete;
};





