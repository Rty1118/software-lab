#pragma once
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
using namespace std;

// 枚举类型定义
enum class TransactionType {
    INCOME,
    EXPENSE
};

enum class KeyType {
    TRAVEL,
    FOOD,
    STUDY,
    RENT,
    WAGES,
    MEDICINE,
    TRANSPORTATION,
    OTHERS
};

// 交易类
class Transaction {
private:
    int id;
    double amount;
    TransactionType type;
    KeyType keyword;
    string date;
    string note;
    string deadtime;

public:
    Transaction(int id, double amt, TransactionType t, KeyType kw,
        const string& dt, const string& nt = "", const string& ddt = "");

    // Getter方法
    int getId() const;
    double getAmount() const;
    TransactionType getType() const;
    KeyType getKeyword() const;
    string getDate() const;
    string getNote() const;
    string getDeadtime() const;

    // Setter方法
    void setAmount(double amt);
    void setType(TransactionType t);
    void setKeyword(KeyType kw);
    void setNote(const string& nt);
    void setDeadtime(const string& ddt);

    // 显示交易信息
    void display() const;

    // 静态工具方法
    static string keywordToString(KeyType kw);
    static KeyType stringToKeyword(const string& str);
};

#endif