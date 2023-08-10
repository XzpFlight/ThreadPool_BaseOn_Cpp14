#include <iostream>
#include <random>
#include "ThreadPool.h"
#include "MultiThreadPool.h"

std::random_device rd; // 真实随机数产生器

std::mt19937 mt(rd()); //生成计算随机数mt

std::uniform_int_distribution<int> dist(-1000, 1000); //生成-1000到1000之间的离散均匀分布数

auto rnd = std::bind(dist, mt);

// 设置线程睡眠时间
void simulate_hard_computation()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + rnd()));
}

// 添加两个数字的简单函数并打印结果
void multiply(const int a, const int b)
{
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
}

// 添加两个数字的简单函数并打印结果
void Multimultiply(const int a, const int b)
{
    simulate_hard_computation();
    const int res = a * b;
    std::cout << "Multi: " << a << " * " << b << " = " << res << std::endl;
}

// 添加并输出结果
void multiply_output(int& out, const int a, const int b)
{
    simulate_hard_computation();
    out = a * b;
    std::cout << a << " * " << b << " = " << out << std::endl;
}

// 结果返回
int multiply_return(const int a, const int b)
{
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
    return res;
}

void example()
{
    // 创建线程池
    ThreadPool pool(5);
    MultiThreadPool Multipool(5);

    // 提交Multi乘法操作
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 5; ++j)
        {
            Multipool.AddTask(i, Multimultiply, i, j);
        }

    // 提交乘法操作
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 5; ++j)
        {
            pool.AddTask(multiply, i, j);
        }

    // 使用ref传递的输出参数提交函数
    int output_ref;
    auto future1 = pool.AddTask(multiply_output, std::ref(output_ref), 5, 6);

    // 等待乘法输出完成
    future1.get();
    std::cout << "Last operation result is equals to " << output_ref << std::endl;

    // 使用return参数提交函数
    auto future2 = pool.AddTask(multiply_return, 5, 3);

    // 等待乘法输出完成
    int res = future2.get();
    std::cout << "Last operation result is equals to " << res << std::endl;
}

int main()
{
    example();
    return 0;
}