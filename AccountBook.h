#pragma once
#ifndef ACCOUNTBOOK_H
#define ACCOUNTBOOK_H

#include "Transaction.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
using namespace std;

// 记账本核心类
class AccountBook {
private:
    vector<Transaction> transactions;
    vector<int> liveTransactions;
    int totalTransactions;
    int nextTransactionId;
    string dataFileName;

    // 私有方法
    string getCurrentDate();
    bool isTransactionExpired(const Transaction& transaction);
    void displaySearchResults(const vector<Transaction>& results, const string& title);

    // 文件操作方法
    void saveToFile();
    void loadFromFile();
    Transaction parseTransactionFromLine(const string& line);
    string convertTransactionToLine(const Transaction& transaction);

public:
    AccountBook(const string& filename = "account_data.txt");
    ~AccountBook();

    // 交易记录管理
    void addTransaction(double amount, TransactionType type, KeyType keyword,
        const string& note = "", const string& deadtime = "");
    void deleteTransaction(int id);
    void changeTransaction(int id);
    void displayAllTransactions();

    // 统计功能
    double calculateTotalIncome();
    double calculateTotalExpense();
    void displayStatistics();

    // 搜索功能
    void search_by_keyword(const string& keyword);
    void search_by_time(const string& time);
    void search_by_type(const string& typeStr);
    void search_by_amount(double amount);
    void search_by_note(const string& searchNote);

    // 系统维护
    void check();
};

#endif