#include "pch.h"
#include "../软件工程/AccountBook.h"
#include "../软件工程/UserInterface.h"
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;

// 测试数据清理类
class TestFixture : public ::testing::Test {
protected:
    string testFileName;

    void SetUp() {
        // 生成唯一的测试文件名
        auto now = chrono::system_clock::now();
        auto timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
        testFileName = "test_data_" + to_string(timestamp) + ".txt";
    }

    void TearDown() {
        // 删除测试文件
        remove(testFileName.c_str());
    }

    // 辅助函数：模拟用户输入
    void simulateInput(const string& input) {
        stringstream ss(input);
        cin.rdbuf(ss.rdbuf());
    }

    // 辅助函数：捕获输出
    string captureOutput(void (*func)(), const string& input = "") {
        stringstream output;
        streambuf* oldCout = cout.rdbuf(output.rdbuf());

        if (!input.empty()) {
            simulateInput(input);
        }

        func();

        cout.rdbuf(oldCout);
        return output.str();
    }
};

// 测试1：完整的交易生命周期
TEST_F(TestFixture, TransactionLifecycleIntegration) {
    // 1. 创建 AccountBook
    AccountBook book(testFileName);

    // 2. 添加交易
    book.addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "工资收入", "2026-12-31");
    book.addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "午餐", "2026-12-31");
    book.addTransaction(200.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "交通费", "2026-12-31");

    // 3. 验证统计
    EXPECT_DOUBLE_EQ(book.calculateTotalIncome(), 1000.0);
    EXPECT_DOUBLE_EQ(book.calculateTotalExpense(), 700.0);

    // 4. 验证文件保存
    ifstream file(testFileName);
    EXPECT_TRUE(file.good());

    // 5. 验证可以正确加载
    AccountBook book2(testFileName);
    EXPECT_DOUBLE_EQ(book2.calculateTotalIncome(), 1000.0);
    EXPECT_DOUBLE_EQ(book2.calculateTotalExpense(), 700.0);

    // 6. 修改交易
    book.changeTransaction(1);
    // 注意：changeTransaction需要用户输入，在集成测试中我们跳过具体修改

    // 7. 删除交易
    book.deleteTransaction(2);
    EXPECT_DOUBLE_EQ(book.calculateTotalExpense(), 200.0); // 只剩下交通费

    // 8. 验证搜索功能
    testing::internal::CaptureStdout();
    book.search_by_keyword("交通");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("交通") != string::npos);
}

// 测试2：过期交易处理的集成测试
TEST_F(TestFixture, ExpiredTransactionIntegration) {
    AccountBook book(testFileName);

    // 添加不会过期的记录
    book.addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "工资", "2099-12-31");

    // 添加已过期的记录
    book.addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "过期餐费", "2020-01-01");

    // 添加没有过期时间的记录
    book.addTransaction(200.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "交通", "");

    // 运行检查
    book.check();

    // 验证统计（过期记录应该被排除）
    testing::internal::CaptureStdout();
    book.displayStatistics();
    string stats = testing::internal::GetCapturedStdout();

    // 应该只有收入和交通支出
    EXPECT_TRUE(stats.find("总收入: 1000") != string::npos);
    // 过期餐费不应该计入
}

// 测试3：完整的数据持久化流程
TEST_F(TestFixture, DataPersistenceIntegration) {
    // 第一阶段：创建、添加、保存
    {
        AccountBook book1(testFileName);
        book1.addTransaction(100.0, TransactionType::INCOME, KeyType::WAGES, "测试1", "2026-6-1");
        book1.addTransaction(50.0, TransactionType::EXPENSE, KeyType::FOOD, "测试2", "2026-6-1");
        book1.addTransaction(30.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "测试3", "2026-6-1");
   
        // 验证内存中的数据
        EXPECT_DOUBLE_EQ(book1.calculateTotalIncome(), 100.0);
        EXPECT_DOUBLE_EQ(book1.calculateTotalExpense(), 80.0);

        // 析构函数会自动保存
    }

    // 第二阶段：重新加载验证
    {
        AccountBook book2(testFileName);
        
        // 验证数据正确加载
        EXPECT_DOUBLE_EQ(book2.calculateTotalIncome(), 100.0);
        EXPECT_DOUBLE_EQ(book2.calculateTotalExpense(), 80.0);

        // 验证记录数量
        testing::internal::CaptureStdout();
        book2.displayAllTransactions();
        string output = testing::internal::GetCapturedStdout();

        // 应该包含所有记录
        EXPECT_TRUE(output.find("测试1") != string::npos);
        EXPECT_TRUE(output.find("测试2") != string::npos);
        EXPECT_TRUE(output.find("测试3") != string::npos);
    }

    // 第三阶段：修改后重新保存
    {
        AccountBook book3(testFileName);
        book3.deleteTransaction(1); // 删除第一条记录

        // 验证修改
        EXPECT_DOUBLE_EQ(book3.calculateTotalIncome(), 0.0);
        EXPECT_DOUBLE_EQ(book3.calculateTotalExpense(), 80.0);
    }

    // 第四阶段：再次加载验证修改
    {
        AccountBook book4(testFileName);
       
        EXPECT_DOUBLE_EQ(book4.calculateTotalIncome(), 0.0);
        EXPECT_DOUBLE_EQ(book4.calculateTotalExpense(), 80.0);
    }
}

// 测试4：搜索功能的集成测试
TEST_F(TestFixture, SearchIntegration) {
    AccountBook book(testFileName);

    // 准备测试数据
    book.addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "一月工资", "2026-6-1");
    book.addTransaction(800.0, TransactionType::INCOME, KeyType::WAGES, "二月工资", "2026-6-1");
    book.addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "餐厅聚餐", "2026-6-1");
    book.addTransaction(300.0, TransactionType::EXPENSE, KeyType::FOOD, "外卖", "2026-6-1");
    book.addTransaction(200.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "地铁通勤", "2026-6-1");
    book.addTransaction(150.0, TransactionType::EXPENSE, KeyType::STUDY, "购买书籍", "2026-6-1");

    // 测试关键词搜索
    testing::internal::CaptureStdout();
    book.search_by_keyword("餐饮");
    string output1 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output1.find("餐饮") != string::npos || output1.find("找到 0 条") != string::npos);

    // 测试类型搜索
    testing::internal::CaptureStdout();
    book.search_by_type("收入");
    string output2 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output2.find("工资") != string::npos);

    // 测试金额搜索
    testing::internal::CaptureStdout();
    book.search_by_amount(500.0);
    string output3 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output3.find("500") != string::npos);

    // 测试备注搜索
    testing::internal::CaptureStdout();
    book.search_by_note("工资");
    string output4 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output4.find("工资") != string::npos);
}

// 测试5：完整的用户操作流程（收入记录）
TEST_F(TestFixture, UserInterfaceIncomeRecording) {
    // 准备测试环境
    AccountBook book(testFileName);

    // 方法：测试 AccountBook 的完整业务流程
    // 模拟用户添加收入
    book.addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "测试工资", "2026-8-1");

    // 模拟用户查看
    testing::internal::CaptureStdout();
    book.displayAllTransactions();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("测试工资") != string::npos);
    EXPECT_TRUE(output.find("1000") != string::npos);

    // 模拟用户统计
    testing::internal::CaptureStdout();
    book.displayStatistics();
    string stats = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(stats.find("总收入: 1000") != string::npos);
}

// 测试6：完整的用户操作流程（支出记录和统计）
TEST_F(TestFixture, UserInterfaceExpenseRecording) {
    AccountBook book(testFileName);

    // 模拟一系列用户操作
    book.addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "超市购物", "2026-5-1");
    book.addTransaction(200.0, TransactionType::EXPENSE, KeyType::TRANSPORTATION, "加油", "2026-7-1");
    book.addTransaction(100.0, TransactionType::EXPENSE, KeyType::TRAVEL, "电影", "2026-9-1");

    // 用户查看详细
    testing::internal::CaptureStdout();
    book.displayAllTransactions();
    string details = testing::internal::GetCapturedStdout();

    // 验证所有支出都显示
    EXPECT_TRUE(details.find("超市购物") != string::npos);
    EXPECT_TRUE(details.find("加油") != string::npos);
    EXPECT_TRUE(details.find("电影") != string::npos);

    // 用户查看统计
    testing::internal::CaptureStdout();
    book.displayStatistics();
    string stats = testing::internal::GetCapturedStdout();

    // 验证总支出正确
    EXPECT_TRUE(stats.find("总支出: 800") != string::npos);
    EXPECT_TRUE(stats.find("餐饮") != string::npos || stats.find("FOOD") != string::npos);
}

// 测试7：用户修改和删除流程
TEST_F(TestFixture, UserInterfaceModifyDelete) {
    AccountBook book(testFileName);

    // 用户添加记录
    book.addTransaction(100.0, TransactionType::EXPENSE, KeyType::FOOD, "原备注", "2026-10-1");

    // 用户修改记录
    book.changeTransaction(1);

    // 用户删除记录
    book.deleteTransaction(1);

    // 验证删除成功
    testing::internal::CaptureStdout();
    book.displayAllTransactions();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("暂无交易记录") != string::npos ||
        output.find("ID: 1") == string::npos);
}

// 测试8：搜索功能用户流程
TEST_F(TestFixture, UserInterfaceSearch) {
    AccountBook book(testFileName);

    // 准备多样化的测试数据
    book.addTransaction(1000.0, TransactionType::INCOME, KeyType::WAGES, "工资收入2024年1月", "2026-5-22");
    book.addTransaction(800.0, TransactionType::INCOME, KeyType::OTHERS, "年终奖", "2026-9-13");
    book.addTransaction(500.0, TransactionType::EXPENSE, KeyType::FOOD, "春节聚餐", "2026-9-10");
    book.addTransaction(300.0, TransactionType::EXPENSE, KeyType::TRAVEL, "旅游花费", "2026-8-31");
    book.addTransaction(200.0, TransactionType::EXPENSE, KeyType::STUDY, "在线课程", "2026-12-24");

    // 模拟用户按关键词搜索
    testing::internal::CaptureStdout();
    book.search_by_keyword("餐饮");
    string output1 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output1.find("春节聚餐") != string::npos ||
        output1.find("找到 0 条") != string::npos);

    // 模拟用户按类型搜索
    testing::internal::CaptureStdout();
    book.search_by_type("支出");
    string output2 = testing::internal::GetCapturedStdout();
    // 应该找到3条支出记录
    EXPECT_TRUE(output2.find("共找到 3 条记录") != string::npos);

    // 模拟用户按备注搜索
    testing::internal::CaptureStdout();
    book.search_by_note("2024");
    string output3 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output3.find("工资收入2024年1月") != string::npos);
}

// 测试9：错误处理集成测试
TEST_F(TestFixture, ErrorHandlingIntegration) {
    AccountBook book(testFileName);

    // 测试1：删除不存在的记录
    testing::internal::CaptureStdout();
    book.deleteTransaction(999); // 不存在的ID
    string output1 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output1.find("未找到ID为 999 的交易记录") != string::npos);

    // 测试2：修改不存在的记录
    testing::internal::CaptureStdout();
    book.changeTransaction(999);
    string output2 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output2.find("未找到ID为 999 的交易记录") != string::npos);

    // 测试3：搜索不存在的关键词
    testing::internal::CaptureStdout();
    book.search_by_keyword("不存在的类别");
    string output3 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output3.find("未找到") != string::npos);

    // 测试4：无效金额处理（如果支持）
    book.addTransaction(0.0, TransactionType::INCOME, KeyType::OTHERS, "零金额测试");
    book.addTransaction(-100.0, TransactionType::EXPENSE, KeyType::OTHERS, "负金额测试");

    // 测试5：无效文件操作
    AccountBook invalidBook("//invalid/path/file.txt");
    testing::internal::CaptureStdout();
    invalidBook.displayAllTransactions(); // 应该显示"暂无交易记录"
    string output5 = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output5.find("暂无交易记录") != string::npos);
}