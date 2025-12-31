#define _CRT_SECURE_NO_WARNINGS
#include "AccountBook.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
using namespace std;

AccountBook::AccountBook(const string& filename) : totalTransactions(0), nextTransactionId(1), dataFileName(filename) {
    liveTransactions.clear();
    loadFromFile(); // 构造函数中加载数据
}

AccountBook::~AccountBook() {
    saveToFile(); // 析构函数中保存数据
}

// 文件保存方法
void AccountBook::saveToFile() {
    ofstream file(dataFileName);
    if (!file.is_open()) {
        cout << "无法打开文件保存数据: " << dataFileName << endl;
        return;
    }

    // 保存交易记录
    for (const auto& transaction : transactions) {
        file << convertTransactionToLine(transaction) << endl;
    }

    file.close();
    cout << "数据已保存到文件: " << dataFileName << endl;
}

// 文件加载方法
void AccountBook::loadFromFile() {
    ifstream file(dataFileName);
    if (!file.is_open()) {
        cout << "无法找到数据文件，将创建新文件: " << dataFileName << endl;
        return;
    }

    transactions.clear();
    liveTransactions.clear();

    string line;
    int maxId = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        Transaction transaction = parseTransactionFromLine(line);
        if (transaction.getId() > 0) {
            transactions.push_back(transaction);
            liveTransactions.push_back(transaction.getId());
            totalTransactions++;

            if (transaction.getId() > maxId) {
                maxId = transaction.getId();
            }
        }
    }

    nextTransactionId = maxId + 1;
    file.close();
    cout << "从文件加载了 " << transactions.size() << " 条交易记录" << endl;
}

// 将交易记录转换为字符串格式
string AccountBook::convertTransactionToLine(const Transaction& transaction) {
    ostringstream oss;
    oss << transaction.getId() << "|"
        << transaction.getAmount() << "|"
        << static_cast<int>(transaction.getType()) << "|"
        << static_cast<int>(transaction.getKeyword()) << "|"
        << transaction.getDate() << "|"
        << transaction.getNote() << "|"
        << transaction.getDeadtime();
    return oss.str();
}

// 从字符串解析交易记录
Transaction AccountBook::parseTransactionFromLine(const string& line) {
    stringstream ss(line);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 7) {
        return Transaction(0, 0, TransactionType::EXPENSE, KeyType::OTHERS, "");
    }

    try {
        int id = stoi(tokens[0]);
        double amount = stod(tokens[1]);
        TransactionType type = static_cast<TransactionType>(stoi(tokens[2]));
        KeyType keyword = static_cast<KeyType>(stoi(tokens[3]));
        string date = tokens[4];
        string note = tokens[5];
        string deadtime = tokens[6];

        return Transaction(id, amount, type, keyword, date, note, deadtime);
    }
    catch (const exception& e) {
        cout << "解析交易记录错误: " << e.what() << endl;
        return Transaction(0, 0, TransactionType::EXPENSE, KeyType::OTHERS, "");
    }
}

// 获取当前日期字符串
string AccountBook::getCurrentDate() {
    auto now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);
    tm* localTime = localtime(&time);

    ostringstream oss;
    oss << put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 检查交易是否过期
bool AccountBook::isTransactionExpired(const Transaction& transaction) {
    if (transaction.getDeadtime().empty()) return false;
    return transaction.getDeadtime() < getCurrentDate();
}

// 添加交易记录（修改后自动保存）
void AccountBook::addTransaction(double amount, TransactionType type, KeyType keyword,
    const string& note, const string& deadtime) {
    string date = getCurrentDate();
    transactions.emplace_back(nextTransactionId, amount, type, keyword, date, note, deadtime);
    liveTransactions.push_back(nextTransactionId);
    totalTransactions++;
    nextTransactionId++;

    saveToFile(); // 添加后自动保存

    cout << "交易记录添加成功！ID: " << (nextTransactionId - 1) << endl;
}

// 删除交易记录（修改后自动保存）
void AccountBook::deleteTransaction(int id) {
    auto it = find_if(transactions.begin(), transactions.end(),
        [id](const Transaction& t) { return t.getId() == id; });

    if (it != transactions.end()) {
        transactions.erase(it);

        auto liveIt = find(liveTransactions.begin(), liveTransactions.end(), id);
        if (liveIt != liveTransactions.end()) {
            liveTransactions.erase(liveIt);
        }

        saveToFile(); // 删除后自动保存
        cout << "交易记录 ID: " << id << " 已删除" << endl;
    }
    else {
        cout << "未找到ID为 " << id << " 的交易记录" << endl;
    }
}

// 修改交易记录（修改后自动保存）
void AccountBook::changeTransaction(int id) {
    auto it = find_if(transactions.begin(), transactions.end(),
        [id](const Transaction& t) { return t.getId() == id; });

    if (it != transactions.end()) {
        cout << "修改交易记录 ID: " << id << endl;
        cout << "当前信息: ";
        it->display();

        double newAmount;
        string input;
        cout << "输入新金额 (直接回车保持原值): ";
        getline(cin, input);
        if (!input.empty()) {
            newAmount = stod(input);
            it->setAmount(newAmount);
        }

        cout << "输入新备注 (直接回车保持原值): ";
        getline(cin, input);
        if (!input.empty()) {
            it->setNote(input);
        }

        saveToFile(); // 修改后自动保存
        cout << "交易记录修改成功！" << endl;
    }
    else {
        cout << "未找到ID为 " << id << " 的交易记录" << endl;
    }
}

// 显示所有交易记录
void AccountBook::displayAllTransactions() {
    //引入空指针并进行函数调用
    //Transaction* tran = nullptr;
    //tran->display();
    if (transactions.empty()) {
        cout << "暂无交易记录。" << endl;
        return;
    }

    cout << "\n=== 所有交易记录 ===" << endl;
    for (const auto& transaction : transactions) {
        if (!isTransactionExpired(transaction)) {
            transaction.display();
        }
    }
}

// 统计总收入
double AccountBook::calculateTotalIncome() {
    double total = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::INCOME &&
            !isTransactionExpired(transaction)) {
            total += transaction.getAmount();
        }
    }
    return total;
}

// 统计总支出
double AccountBook::calculateTotalExpense() {
    double total = 0.0;
    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::EXPENSE &&
            !isTransactionExpired(transaction)) {
            total += transaction.getAmount();
        }
    }
    return total;
}

// 显示统计信息
void AccountBook::displayStatistics() {
    check(); // 先检查过期记录

    double income = calculateTotalIncome();
    double expense = calculateTotalExpense();
    double balance = income - expense;

    cout << "\n=== 统计信息 ===" << endl;
    cout << "总收入: " << income << " 元" << endl;
    cout << "总支出: " << expense << " 元" << endl;
    cout << "当前结余: " << balance << " 元" << endl;

    // 按类别统计
    map<KeyType, double> categoryExpense;
    map<KeyType, double> categoryIncome;

    for (const auto& transaction : transactions) {
        if (isTransactionExpired(transaction)) continue;

        if (transaction.getType() == TransactionType::EXPENSE) {
            categoryExpense[transaction.getKeyword()] += transaction.getAmount();
        }
        else {
            categoryIncome[transaction.getKeyword()] += transaction.getAmount();
        }
    }

    cout << "\n--- 支出分类统计 ---" << endl;
    for (const auto& pair : categoryExpense) {
        cout << Transaction::keywordToString(pair.first) << ": " << pair.second << " 元" << endl;
    }

    cout << "\n--- 收入分类统计 ---" << endl;
    for (const auto& pair : categoryIncome) {
        cout << Transaction::keywordToString(pair.first) << ": " << pair.second << " 元" << endl;
    }
}

// 搜索功能实现
void AccountBook::search_by_keyword(const string& keyword) {
    KeyType kw = Transaction::stringToKeyword(keyword);
    //不清楚下面符合判断条件的数量就确定容器的大小，初始化有误，会导致越界访问
    //vector<Transaction> results(10);
    vector<Transaction> results;

    for (const auto& transaction : transactions) {
        if (transaction.getKeyword() == kw && !isTransactionExpired(transaction)) {
            results.push_back(transaction);
        }
    }

    displaySearchResults(results, "关键词搜索: " + keyword);
}

void AccountBook::search_by_time(const string& time) {
    vector<Transaction> results;

    for (const auto& transaction : transactions) {
        if (transaction.getDate().find(time) != string::npos &&
            !isTransactionExpired(transaction)) {
            results.push_back(transaction);
        }
    }

    displaySearchResults(results, "时间搜索: " + time);
}

void AccountBook::search_by_type(const string& typeStr) {
    TransactionType type = (typeStr == "收入" || typeStr == "income") ?
        TransactionType::INCOME : TransactionType::EXPENSE;
    vector<Transaction> results;

    for (const auto& transaction : transactions) {
        if (transaction.getType() == type && !isTransactionExpired(transaction)) {
            results.push_back(transaction);
        }
    }

    displaySearchResults(results, "类型搜索: " + typeStr);
}

void AccountBook::search_by_amount(double amount) {
    vector<Transaction> results;

    for (const auto& transaction : transactions) {
        if (transaction.getAmount() == amount && !isTransactionExpired(transaction)) {
            results.push_back(transaction);
        }
    }

    displaySearchResults(results, "金额搜索: " + to_string(amount));
}

void AccountBook::search_by_note(const string& searchNote) {
    vector<Transaction> results;

    for (const auto& transaction : transactions) {
        if (transaction.getNote().find(searchNote) != string::npos &&
            !isTransactionExpired(transaction)) {
            results.push_back(transaction);
        }
    }

    displaySearchResults(results, "备注搜索: " + searchNote);
}

// 检查并清除过期记录（修改后自动保存）
void AccountBook::check() {
    vector<Transaction> validTransactions;
    liveTransactions.clear();

    for (const auto& transaction : transactions) {
        if (!isTransactionExpired(transaction)) {
            validTransactions.push_back(transaction);
            liveTransactions.push_back(transaction.getId());
        }
    }

    if (validTransactions.size() < transactions.size()) {
        cout << "已清除 " << (transactions.size() - validTransactions.size()) << " 条过期记录" << endl;
        transactions = validTransactions;
        saveToFile(); // 清除过期记录后自动保存
    }
}

// 显示搜索结果
void AccountBook::displaySearchResults(const vector<Transaction>& results, const string& title) {
    if (results.empty()) {
        cout << "未找到符合条件的交易记录。" << endl;
        return;
    }

    cout << "\n=== " << title << " ===" << endl;
    for (const auto& transaction : results) {
        transaction.display();
    }
    cout << "共找到 " << results.size() << " 条记录" << endl;
}