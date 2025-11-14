/**
 * @file StageComponents.h
 * @brief ã‚¹ãƒ†ãƒ¼ã‚¸é€²è¡Œç”¨ã®ã‚¿ã‚°ã¨çŠ¶æ…‹ã‚³ãƒ³ãƒãƒ¼ãƒãƒ³ãƒˆ
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
 * @brief ã‚¹ãƒ†ãƒ¼ã‚¸ã®é–‹å§‹åœ°ç‚¹ã‚’ç¤ºã™ã‚¿ã‚°
 */
struct StartTag : IComponent {};

/**
 * @struct GoalTag
 * @brief ã‚¹ãƒ†ãƒ¼ã‚¸ã®ã‚´ãƒ¼ãƒ«åœ°ç‚¹ã‚’ç¤ºã™ã‚¿ã‚°
 */
struct GoalTag : IComponent {};

/**
 * @struct StageProgress
 * @brief ã‚¹ãƒ†ãƒ¼ã‚¸ç•ªå·ã¨é€²è¡Œãƒ•ãƒ©ã‚°ã‚’ç®¡ç†
 */
struct StageProgress : IComponent {
    int currentStage = 1;
    bool requestAdvance = false;
};

/**
 * @struct StageCreate
 * @brief CSVƒtƒ@ƒCƒ‹‚ğ“Ç‚İ‚İA‚»‚ê‚ğŠî‚ÉƒXƒe[ƒW‚ğ¶¬
 */
struct StageCreate : IComponent {
    //ƒtƒ@ƒCƒ‹‚ÌéŒ¾
    ifstream m_file;

    //ƒXƒe[ƒWƒf[ƒ^‚ğ•Û‚·‚é‚½‚ß‚Ì“ñŸŒ³”z—ñ
    vector<vector<int>> stageMap;

    StageCreate() {
        m_file.open("Assets/StageData/aaa.csv");         //ƒpƒX‚Ìw’è
        //ƒtƒ@ƒCƒ‹‚ªŠJ‚¯‚È‚©‚Á‚½‚Ìˆ—iƒGƒ‰[ƒƒO“™j
        if (!m_file.is_open()) {
            cerr << "ƒtƒ@ƒCƒ‹‚Ì“Ç‚İ‚İ‚É¸”s‚µ‚Ü‚µ‚½B" << endl;
        } else {
            //ƒtƒ@ƒCƒ‹‚ªŠJ‚¢‚Ä‚¢‚éê‡AƒRƒ“ƒXƒgƒ‰ƒNƒ^“à‚Åƒf[ƒ^‚ğ“Ç‚İ‚Ş
            loadStageData();
        }
    }

    void loadStageData() {
        string line;
        while (getline(m_file, line)) {
            //1s•ª‚Ìƒf[ƒ^‚ğŠi”[‚·‚éƒxƒNƒ^
            vector<int> row;
            //ƒJƒ“ƒ}‹æØ‚è‚Åƒp[ƒX
            stringstream sstream(line);
            string cell;

            while(getline(sstream, cell,',')){
                try {
                    //•¶š—ñ‚ğ®”‚É•ÏŠ·‚µ‚ÄƒxƒNƒ^‚É’Ç‰Á
                    row.push_back(stoi(cell));
                } catch (const std::invalid_argument &error) {
                    //”’l•ÏŠ·ƒGƒ‰[‚Ìˆ—
                    cerr << "Invalid number in CSV: " << cell << endl;
                } catch (const std::out_of_range &error) {
                    //”ÍˆÍŠOƒGƒ‰[‚Ìˆ—
                    cerr << "Number out of range in CSV: " << cell << endl;
                }
            }
            //ˆ—‚µ‚½s‚ğƒxƒNƒ^[‚ÉŠi”[
            stageMap.push_back(row);
        }
        m_file.close();
    }

    StageCreate(const StageCreate &) = delete;
    StageCreate& operator=(const StageCreate&) = delete;
};





