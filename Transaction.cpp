#include "Transaction.h"
#include <iostream>
using namespace std;

Transaction::Transaction(int id, double amt, TransactionType t, KeyType kw,
    const string& dt, const string& nt, const string& ddt)
    : id(id), amount(amt), type(t), keyword(kw), date(dt), note(nt), deadtime(ddt) {}

// Getter方法实现
int Transaction::getId() const { return id; }
double Transaction::getAmount() const { return amount; }
TransactionType Transaction::getType() const { return type; }
KeyType Transaction::getKeyword() const { return keyword; }
string Transaction::getDate() const { return date; }
string Transaction::getNote() const { return note; }
string Transaction::getDeadtime() const { return deadtime; }

// Setter方法实现
void Transaction::setAmount(double amt) { amount = amt; }
void Transaction::setType(TransactionType t) { type = t; }
void Transaction::setKeyword(KeyType kw) { keyword = kw; }
void Transaction::setNote(const string& nt) { note = nt; }
void Transaction::setDeadtime(const string& ddt) { deadtime = ddt; }

// 显示交易信息
void Transaction::display() const {
    cout << "ID: " << id
        << " | " << (type == TransactionType::INCOME ? "收入" : "支出")
        << " | 金额: " << amount
        << " | 类别: " << keywordToString(keyword)
        << " | 日期: " << date;
    if (!note.empty()) {
        cout << " | 备注: " << note;
    }
    if (!deadtime.empty()) {
        cout << " | 失效时间: " << deadtime;
    }
    cout << endl;
}

// 关键词转字符串
string Transaction::keywordToString(KeyType kw) {
    switch (kw) {
    case KeyType::TRAVEL: return "旅行";
    case KeyType::FOOD: return "餐饮";
    case KeyType::STUDY: return "学习";
    case KeyType::RENT: return "房租";
    case KeyType::WAGES: return "工资";
    case KeyType::MEDICINE: return "医疗";
    case KeyType::TRANSPORTATION: return "交通";
    case KeyType::OTHERS: return "其他";
    default: return "未知";
    }
}

// 字符串转关键词
KeyType Transaction::stringToKeyword(const string& str) {
    if (str == "旅行" || str == "travel") return KeyType::TRAVEL;
    if (str == "餐饮" || str == "food") return KeyType::FOOD;
    if (str == "学习" || str == "study") return KeyType::STUDY;
    if (str == "房租" || str == "rent") return KeyType::RENT;
    if (str == "工资" || str == "wages") return KeyType::WAGES;
    if (str == "医疗" || str == "medicine") return KeyType::MEDICINE;
    if (str == "交通" || str == "transportation") return KeyType::TRANSPORTATION;
    return KeyType::OTHERS;
}