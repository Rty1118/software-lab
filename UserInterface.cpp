#include "UserInterface.h"
#include <iostream>
using namespace std;

// 菜单显示方法
void UserInterface::displayMainMenu() {
    cout << "\n=== 智能记账本系统 ===" << endl;
    cout << "1. 记录交易" << endl;
    cout << "2. 查看所有记录" << endl;
    cout << "3. 查看统计信息" << endl;
    cout << "4. 搜索交易记录" << endl;
    cout << "5. 修改交易记录" << endl;
    cout << "6. 删除交易记录" << endl;
    cout << "7. 检查过期记录" << endl;
    cout << "0. 退出系统" << endl;
    cout << "请选择操作: ";
}

void UserInterface::displayRecordMenu() {
    cout << "\n=== 记录交易 ===" << endl;
    cout << "1. 记录收入" << endl;
    cout << "2. 记录支出" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "请选择: ";
}

void UserInterface::displaySearchMenu() {
    cout << "\n=== 搜索交易记录 ===" << endl;
    cout << "1. 按关键词搜索" << endl;
    cout << "2. 按时间搜索" << endl;
    cout << "3. 按类型搜索" << endl;
    cout << "4. 按金额搜索" << endl;
    cout << "5. 按备注搜索" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "请选择: ";
}

void UserInterface::displayKeywordMenu() {
    cout << "\n请选择类别:" << endl;
    cout << "1. 旅行  2. 餐饮  3. 学习  4. 房租" << endl;
    cout << "5. 工资  6. 医疗  7. 交通  8. 其他" << endl;
    cout << "请选择(1-8): ";
}

// 工具方法
KeyType UserInterface::getKeywordFromInput(int choice) {
    switch (choice) {
    case 1: return KeyType::TRAVEL;
    case 2: return KeyType::FOOD;
    case 3: return KeyType::STUDY;
    case 4: return KeyType::RENT;
    case 5: return KeyType::WAGES;
    case 6: return KeyType::MEDICINE;
    case 7: return KeyType::TRANSPORTATION;
    case 8: return KeyType::OTHERS;
    default: return KeyType::OTHERS;
    }
}

// 记录收入
void UserInterface::recordIncome() {
    double amount;
    int keywordChoice;
    string note, deadtime;

    cout << "请输入收入金额: ";
    cin >> amount;
    cin.ignore();

    displayKeywordMenu();
    cin >> keywordChoice;
    cin.ignore();

    cout << "请输入备注(可选): ";
    getline(cin, note);

    cout << "请输入失效时间(格式: YYYY-MM-DD, 可选): ";
    getline(cin, deadtime);

    KeyType keyword = getKeywordFromInput(keywordChoice);
    accountBook.addTransaction(amount, TransactionType::INCOME, keyword, note, deadtime);
}

// 记录支出
void UserInterface::recordExpense() {
    double amount;
    int keywordChoice;
    string note, deadtime;

    cout << "请输入支出金额: ";
    cin >> amount;
    cin.ignore();

    displayKeywordMenu();
    cin >> keywordChoice;
    cin.ignore();

    cout << "请输入备注(可选): ";
    getline(cin, note);

    cout << "请输入失效时间(格式: YYYY-MM-DD, 可选): ";
    getline(cin, deadtime);

    KeyType keyword = getKeywordFromInput(keywordChoice);
    accountBook.addTransaction(amount, TransactionType::EXPENSE, keyword, note, deadtime);
}

// 记录交易主函数
void UserInterface::record() {
    int choice;
    do {
        displayRecordMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            recordIncome();
            break;
        case 2:
            recordExpense();
            break;
        case 0:
            cout << "返回主菜单" << endl;
            break;
        default:
            cout << "无效选择，请重新输入！" << endl;
        }
    } while (choice != 0);
}

// 搜索功能
void UserInterface::search() {
    int choice;
    do {
        displaySearchMenu();
        cin >> choice;
        cin.ignore();

        string searchStr;
        double searchAmount;

        switch (choice) {
        case 1:
            cout << "请输入关键词: ";
            getline(cin, searchStr);
            accountBook.search_by_keyword(searchStr);
            break;
        case 2:
            cout << "请输入时间(YYYY-MM-DD): ";
            getline(cin, searchStr);
            accountBook.search_by_time(searchStr);
            break;
        case 3:
            cout << "请输入类型(收入/支出): ";
            getline(cin, searchStr);
            accountBook.search_by_type(searchStr);
            break;
        case 4:
            cout << "请输入金额: ";
            cin >> searchAmount;
            cin.ignore();
            accountBook.search_by_amount(searchAmount);
            break;
        case 5:
            cout << "请输入备注关键词: ";
            getline(cin, searchStr);
            accountBook.search_by_note(searchStr);
            break;
        case 0:
            cout << "返回主菜单" << endl;
            break;
        default:
            cout << "无效选择，请重新输入！" << endl;
        }
    } while (choice != 0);
}

// 主运行函数
void UserInterface::run() {
    int choice;

    do {
        displayMainMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            record();
            break;
        case 2:
            accountBook.displayAllTransactions();
            break;
        case 3:
            accountBook.displayStatistics();
            break;
        case 4:
            search();
            break;
        case 5: {
            int id;
            cout << "请输入要修改的交易记录ID: ";
            cin >> id;
            cin.ignore();
            accountBook.changeTransaction(id);
            break;
        }
        case 6: {
            int id;
            cout << "请输入要删除的交易记录ID: ";
            cin >> id;
            cin.ignore();
            accountBook.deleteTransaction(id);
            break;
        }
        case 7:
            accountBook.check();
            cout << "过期记录检查完成！" << endl;
            break;
        case 0:
            cout << "感谢使用记账本系统！" << endl;
            break;
        default:
            cout << "无效选择，请重新输入！" << endl;
        }
    } while (choice != 0);
}