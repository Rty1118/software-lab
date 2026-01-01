cmake_minimum_required(VERSION 3.14)
project(AccountBook VERSION 1.0.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用测试
enable_testing()

# 查找 GoogleTest
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 添加可执行文件（主程序）
add_executable(AccountBookMain
    sys.cpp
    UserInterface.cpp
    AccountBook.cpp
    Transaction.cpp
)

# 添加头文件目录
target_include_directories(AccountBookMain PRIVATE .)

# 添加单元测试可执行文件
add_executable(AccountBookTests
    test_transaction.cpp
    test_accountbook.cpp
    test_integration.cpp
    AccountBook.cpp
    Transaction.cpp
    UserInterface.cpp
)

# 链接 GoogleTest
target_link_libraries(AccountBookTests
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

target_include_directories(AccountBookTests PRIVATE .)

# 添加集成测试可执行文件
add_executable(IntegrationTests
    integration_tests.cpp
    AccountBook.cpp
    Transaction.cpp
    UserInterface.cpp
)

target_link_libraries(IntegrationTests
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

target_include_directories(IntegrationTests PRIVATE .)

# 将测试添加到 CTest
add_test(NAME TransactionTests COMMAND AccountBookTests --gtest_filter="TransactionTest.*")
add_test(NAME AccountBookTests COMMAND AccountBookTests --gtest_filter="AccountBookTest.*")
add_test(NAME IntegrationTests COMMAND IntegrationTests)
