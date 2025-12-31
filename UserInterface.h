#pragma once
#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "AccountBook.h"
#include <string>
using namespace std;

// 用户界面类
class UserInterface {
private:
    AccountBook accountBook;

    // 菜单显示方法
    void displayMainMenu();
    void displayRecordMenu();
    void displaySearchMenu();
    void displayKeywordMenu();

    // 工具方法
    KeyType getKeywordFromInput(int choice);

    // 功能方法
    void recordIncome();
    void recordExpense();
    void record();
    void search();

public:
    void run();
};

#endif