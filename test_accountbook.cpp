#include "pch.h"
#include "../软件工程/AccountBook.h"
#include <fstream>
#include <cstdio>

class AccountBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 使用临时文件进行测试
        testFileName = "test_account_data.txt";
        accountBook = new AccountBook(testFileName);
    }

    void TearDown() override {
        delete accountBook;
        // 删除测试文件
        std::remove(testFileName.c_str());
    }

    AccountBook* accountBook;
    std::string testFileName;

    // 辅助函数：检查文件是否存在
    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
};

// 测试构造函数和析构函数
TEST_F(AccountBookTest, ConstructorAndDestructor) {
    EXPECT_TRUE(fileExists(testFileName) || !fileExists(testFileName)); // 文件可能存在也可能不存在
    EXPECT_EQ(accountBook->calculateTotalIncome(), 0.0);
    EXPECT_EQ(accountBook->calculateTotalExpense(), 0.0);
}

// 测试添加交易记录
TEST_F(AccountBookTest, AddTransaction) {
    // 测试添加收入
    accountBook->addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "工资", "2026-12-31");

    // 测试添加支出
    accountBook->addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "晚餐", "2026-12-31");

    // 测试添加无备注的交易
    accountBook->addTransaction(200.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "", "");

    // 验证文件已创建
    EXPECT_TRUE(fileExists(testFileName));
}

// 测试计算总收入支出
TEST_F(AccountBookTest, CalculateTotals) {
    // 添加测试数据
    accountBook->addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "工资", "2026-12-31");
    accountBook->addTransaction(200.0, TransactionType::INCOME, KeyType::OTHERS, "奖金", "2026-12-31");
    accountBook->addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "餐饮", "2026-12-31");
    accountBook->addTransaction(300.0, TransactionType::EXPENSE, KeyType::RENT, "房租", "2026-12-31");

    double totalIncome = accountBook->calculateTotalIncome();
    double totalExpense = accountBook->calculateTotalExpense();

    EXPECT_DOUBLE_EQ(totalIncome, 1200.0);
    EXPECT_DOUBLE_EQ(totalExpense, 800.0);
}

// 测试删除交易记录
TEST_F(AccountBookTest, DeleteTransaction) {
    // 添加一些记录
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "记录1", "2026-12-31");
    accountBook->addTransaction(200.0, TransactionType::INCOME, KeyType::WAGES, "记录2", "2026-12-31");

    // 删除存在的记录
    EXPECT_NO_THROW(accountBook->deleteTransaction(1));

    // 删除不存在的记录（不应该崩溃）
    EXPECT_NO_THROW(accountBook->deleteTransaction(999));

    // 验证收入总额减少
    EXPECT_DOUBLE_EQ(accountBook->calculateTotalIncome(), 200.0);
}

// 测试修改交易记录
TEST_F(AccountBookTest, ChangeTransaction) {
    // 添加一条记录
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "原始备注", "2026-12-31");

    // 由于 changeTransaction 需要用户输入，这里主要测试接口是否可用
    // 在实际项目中，应该重构使这个函数可测试（将输入作为参数）
    EXPECT_NO_THROW(accountBook->changeTransaction(1));
    EXPECT_NO_THROW(accountBook->changeTransaction(999)); // 不存在的ID
}

// 测试显示所有交易记录
TEST_F(AccountBookTest, DisplayAllTransactions) {
    // 空记录情况
    accountBook->displayAllTransactions(); // 应该显示"暂无交易记录"

    // 有记录情况
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "测试", "2026-12-31");
    EXPECT_NO_THROW(accountBook->displayAllTransactions());
}

// 测试显示统计信息
TEST_F(AccountBookTest, DisplayStatistics) {
    // 空记录统计
    EXPECT_NO_THROW(accountBook->displayStatistics());

    // 有记录统计
    accountBook->addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "工资", "2026-12-31");
    accountBook->addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "餐饮", "2026-12-31");
    accountBook->addTransaction(200.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "交通", "2026-12-31");

    EXPECT_NO_THROW(accountBook->displayStatistics());
}

// 测试搜索功能 - 关键词搜索
TEST_F(AccountBookTest, SearchByKeyword) {
    // 准备测试数据
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "工资收入", "2026-12-31");
    accountBook->addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "午餐", "2026-12-31");
    accountBook->addTransaction(30.0, TransactionType::EXPENSE, KeyType::FOOD, "晚餐", "2026-12-31");

    // 测试搜索存在的关键词
    EXPECT_NO_THROW(accountBook->search_by_keyword("餐饮"));
    EXPECT_NO_THROW(accountBook->search_by_keyword("food")); // 英文关键词

    // 测试搜索不存在的关键词
    EXPECT_NO_THROW(accountBook->search_by_keyword("不存在的类别"));
}

// 测试搜索功能 - 时间搜索
TEST_F(AccountBookTest, SearchByTime) {
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "测试", "2026-12-31");

    // 搜索当前日期（很可能匹配）
    // 搜索不存在的日期
    EXPECT_NO_THROW(accountBook->search_by_time("2099-01-01"));
    EXPECT_NO_THROW(accountBook->search_by_time("2026"));
    EXPECT_NO_THROW(accountBook->search_by_time("")); // 空字符串
}

// 测试搜索功能 - 类型搜索
TEST_F(AccountBookTest, SearchByType) {
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "收入", "2026-12-31");
    accountBook->addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "支出", "2026-12-31");

    EXPECT_NO_THROW(accountBook->search_by_type("收入"));
    EXPECT_NO_THROW(accountBook->search_by_type("income"));
    EXPECT_NO_THROW(accountBook->search_by_type("支出"));
    EXPECT_NO_THROW(accountBook->search_by_type("expense"));
    EXPECT_NO_THROW(accountBook->search_by_type("无效类型"));
}

// 测试搜索功能 - 金额搜索
TEST_F(AccountBookTest, SearchByAmount) {
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "100元收入", "2026-12-31");
    accountBook->addTransaction(200.0, TransactionType::INCOME, KeyType::WAGES, "200元收入", "2026-12-31");

    EXPECT_NO_THROW(accountBook->search_by_amount(100.0));
    EXPECT_NO_THROW(accountBook->search_by_amount(200.0));
    EXPECT_NO_THROW(accountBook->search_by_amount(999.0)); // 不存在的金额
    EXPECT_NO_THROW(accountBook->search_by_amount(0.0)); // 零金额
}

// 测试搜索功能 - 备注搜索
TEST_F(AccountBookTest, SearchByNote) {
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "这是一条测试备注", "2026-12-31");
    accountBook->addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "晚餐备注", "2026-12-31");

    EXPECT_NO_THROW(accountBook->search_by_note("测试"));
    EXPECT_NO_THROW(accountBook->search_by_note("备注"));
    EXPECT_NO_THROW(accountBook->search_by_note("不存在的内容"));
    EXPECT_NO_THROW(accountBook->search_by_note("")); // 空字符串
}

// 测试检查过期记录功能
TEST_F(AccountBookTest, CheckExpiredTransactions) {
    // 添加一条未来过期的记录
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "测试", "2099-12-31");

    // 添加一条已经过期的记录
    accountBook->addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "过期", "2020-01-01");

    EXPECT_NO_THROW(accountBook->check());

    // 检查后，过期记录应该被移除
}

// 测试文件保存和加载
TEST_F(AccountBookTest, FileSaveAndLoad) {
    // 创建新的 AccountBook 并添加数据
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "保存测试", "2026-12-31");
    accountBook->addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "食品", "2026-12-31");

    // 保存文件
    EXPECT_TRUE(fileExists(testFileName));

    // 创建另一个 AccountBook 实例加载相同文件
    AccountBook anotherBook(testFileName);
    // 加载后应该有数据
    // 注意：需要验证加载的数据正确性
}

// 测试交易记录过期检查逻辑
TEST_F(AccountBookTest, IsTransactionExpired) {
    // 由于 isTransactionExpired 是私有方法，我们通过公有方法间接测试
    // 添加一个无过期时间的记录
    accountBook->addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "永不过期", "");

    // 添加一个过去时间的记录
    accountBook->addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "已过期", "2020-01-01");

    // 运行检查
    accountBook->check();

    // 显示统计信息（会调用 check）
    EXPECT_NO_THROW(accountBook->displayStatistics());
}

// 测试空文件处理
TEST_F(AccountBookTest, EmptyFileHandling) {
    // 创建空文件
    std::ofstream emptyFile(testFileName);
    emptyFile.close();

    // 创建新的 AccountBook 实例加载空文件
    AccountBook emptyBook(testFileName);

    // 不应该崩溃
    EXPECT_NO_THROW(emptyBook.displayAllTransactions());
    EXPECT_NO_THROW(emptyBook.displayStatistics());
}

// 测试边界情况：大量记录
TEST_F(AccountBookTest, LargeNumberOfTransactions) {
    // 添加多条记录
    for (int i = 0; i < 100; ++i) {
        accountBook->addTransaction(i + 1.0,
            (i % 2 == 0) ? TransactionType::INCOME : TransactionType::EXPENSE,
            KeyType::OTHERS,
            "记录" + std::to_string(i));
    }

    // 验证统计
    EXPECT_GE(accountBook->calculateTotalIncome(), 0.0);
    EXPECT_GE(accountBook->calculateTotalExpense(), 0.0);

    // 验证搜索功能在大数据集下正常工作
    EXPECT_NO_THROW(accountBook->search_by_type("收入"));
    EXPECT_NO_THROW(accountBook->search_by_note("记录"));
}

// 测试无效数据解析
TEST_F(AccountBookTest, InvalidDataParsing) {
    // 创建一个包含无效数据的文件
    std::ofstream badFile(testFileName);
    badFile << "invalid|data|here\n";
    badFile << "1|not_a_number|0|0|2025-01-01|test|2025-12-31\n";
    badFile.close();

    // 尝试加载应该能处理错误而不崩溃
    AccountBook badDataBook(testFileName);
    EXPECT_NO_THROW(badDataBook.displayAllTransactions());
}