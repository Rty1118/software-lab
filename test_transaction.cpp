#include "pch.h"
#include "../软件工程/Transaction.h"

// 测试 Transaction 类的构造函数和 Getter 方法
TEST(TransactionTest, ConstructorAndGetters) {
    Transaction t(1, 100.5, TransactionType::INCOME, KeyType::WAGES,
        "2025-01-01", "工资收入", "2025-12-31");

    EXPECT_EQ(t.getId(), 1);
    EXPECT_DOUBLE_EQ(t.getAmount(), 100.5);
    EXPECT_EQ(t.getType(), TransactionType::INCOME);
    EXPECT_EQ(t.getKeyword(), KeyType::WAGES);
    EXPECT_EQ(t.getDate(), "2025-01-01");
    EXPECT_EQ(t.getNote(), "工资收入");
    EXPECT_EQ(t.getDeadtime(), "2025-12-31");
}

// 测试 Setter 方法
TEST(TransactionTest, Setters) {
    Transaction t(1, 100.0, TransactionType::EXPENSE, KeyType::FOOD, "2026-04-01");

    t.setAmount(200.0);
    t.setType(TransactionType::INCOME);
    t.setKeyword(KeyType::TRAVEL);
    t.setNote("修改后的备注");
    t.setDeadtime("2026-06-01");

    EXPECT_DOUBLE_EQ(t.getAmount(), 200.0);
    EXPECT_EQ(t.getType(), TransactionType::INCOME);
    EXPECT_EQ(t.getKeyword(), KeyType::TRAVEL);
    EXPECT_EQ(t.getNote(), "修改后的备注");
    EXPECT_EQ(t.getDeadtime(), "2026-06-01");
}

// 边界测试：测试金额边界值
TEST(TransactionTest, AmountBoundary) {
    // 测试零值
    Transaction t1(1, 0.0, TransactionType::INCOME, KeyType::OTHERS, "2025-02-01");
    EXPECT_DOUBLE_EQ(t1.getAmount(), 0.0);

    // 测试负值（如果支持负金额）
    // 注意：根据业务逻辑，金额应该为正数
    Transaction t2(2, -100.0, TransactionType::EXPENSE, KeyType::FOOD, "2025-02-01");
    EXPECT_DOUBLE_EQ(t2.getAmount(), -100.0);
}

// 测试 keywordToString 转换
TEST(TransactionTest, KeywordToString) {
    EXPECT_EQ(Transaction::keywordToString(KeyType::TRAVEL), "旅行");
    EXPECT_EQ(Transaction::keywordToString(KeyType::FOOD), "餐饮");
    EXPECT_EQ(Transaction::keywordToString(KeyType::STUDY), "学习");
    EXPECT_EQ(Transaction::keywordToString(KeyType::RENT), "房租");
    EXPECT_EQ(Transaction::keywordToString(KeyType::WAGES), "工资");
    EXPECT_EQ(Transaction::keywordToString(KeyType::MEDICINE), "医疗");
    EXPECT_EQ(Transaction::keywordToString(KeyType::TRANSPORTATION), "交通");
    EXPECT_EQ(Transaction::keywordToString(KeyType::OTHERS), "其他");
    EXPECT_EQ(Transaction::keywordToString(static_cast<KeyType>(999)), "未知"); // 测试无效枚举
}

// 测试 stringToKeyword 转换
TEST(TransactionTest, StringToKeyword) {
    // 测试中文关键字
    EXPECT_EQ(Transaction::stringToKeyword("旅行"), KeyType::TRAVEL);
    EXPECT_EQ(Transaction::stringToKeyword("餐饮"), KeyType::FOOD);
    EXPECT_EQ(Transaction::stringToKeyword("学习"), KeyType::STUDY);

    // 测试英文关键字
    EXPECT_EQ(Transaction::stringToKeyword("travel"), KeyType::TRAVEL);
    EXPECT_EQ(Transaction::stringToKeyword("food"), KeyType::FOOD);
    EXPECT_EQ(Transaction::stringToKeyword("study"), KeyType::STUDY);

    // 测试无效关键字
    EXPECT_EQ(Transaction::stringToKeyword("不存在"), KeyType::OTHERS);
    EXPECT_EQ(Transaction::stringToKeyword(""), KeyType::OTHERS);
}

// 测试 display 方法（不测试输出，只测试是否正常运行）
TEST(TransactionTest, DisplayNoCrash) {
    Transaction t1(1, 100.0, TransactionType::INCOME, KeyType::WAGES, "2024-01-01", "有备注", "2024-12-31");
    Transaction t2(2, 50.0, TransactionType::EXPENSE, KeyType::FOOD, "2024-01-02", "", "");

    // 确保 display 方法不会崩溃
    EXPECT_NO_THROW(t1.display());
    EXPECT_NO_THROW(t2.display());
}

// 测试空备注和截止时间
TEST(TransactionTest, EmptyNoteAndDeadtime) {
    Transaction t(1, 100.0, TransactionType::INCOME, KeyType::WAGES, "2024-01-01");

    EXPECT_TRUE(t.getNote().empty());
    EXPECT_TRUE(t.getDeadtime().empty());

    // 设置后再清空
    t.setNote("测试备注");
    t.setDeadtime("2024-12-31");
    EXPECT_FALSE(t.getNote().empty());
    EXPECT_FALSE(t.getDeadtime().empty());

    t.setNote("");
    t.setDeadtime("");
    EXPECT_TRUE(t.getNote().empty());
    EXPECT_TRUE(t.getDeadtime().empty());
}

// 测试相等操作符（如果需要比较）
TEST(TransactionTest, CopyAndAssignment) {
    Transaction t1(1, 100.0, TransactionType::INCOME, KeyType::WAGES, "2026-01-01");

    // 测试拷贝构造
    Transaction t2 = t1;
    EXPECT_EQ(t2.getId(), 1);
    EXPECT_DOUBLE_EQ(t2.getAmount(), 100.0);

    // 修改 t2，确保不影响 t1
    t2.setAmount(200.0);
    EXPECT_DOUBLE_EQ(t1.getAmount(), 100.0);
    EXPECT_DOUBLE_EQ(t2.getAmount(), 200.0);
}

// 测试边缘日期格式
TEST(TransactionTest, DateFormats) {
    // 各种可能的日期格式
    Transaction t1(1, 100.0, TransactionType::INCOME, KeyType::WAGES, "2024-01-01");
    Transaction t2(2, 200.0, TransactionType::INCOME, KeyType::WAGES, "2024/01/01");
    Transaction t3(3, 300.0, TransactionType::INCOME, KeyType::WAGES, "2024-12-31 23:59:59");

    EXPECT_EQ(t1.getDate(), "2024-01-01");
    EXPECT_EQ(t2.getDate(), "2024/01/01");
    EXPECT_EQ(t3.getDate(), "2024-12-31 23:59:59");
}

// 测试类型转换
TEST(TransactionTest, TypeTesting) {
    Transaction income(1, 100.0, TransactionType::INCOME, KeyType::WAGES, "2024-01-01");
    Transaction expense(2, 50.0, TransactionType::EXPENSE, KeyType::FOOD, "2024-01-01");

    EXPECT_EQ(income.getType(), TransactionType::INCOME);
    EXPECT_EQ(expense.getType(), TransactionType::EXPENSE);
    EXPECT_NE(income.getType(), expense.getType());
}