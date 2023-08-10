#include <iostream>
#include <random>
#include "ThreadPool.h"
#include "MultiThreadPool.h"

std::random_device rd; // ��ʵ�����������

std::mt19937 mt(rd()); //���ɼ��������mt

std::uniform_int_distribution<int> dist(-1000, 1000); //����-1000��1000֮�����ɢ���ȷֲ���

auto rnd = std::bind(dist, mt);

// �����߳�˯��ʱ��
void simulate_hard_computation()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + rnd()));
}

// ����������ֵļ򵥺�������ӡ���
void multiply(const int a, const int b)
{
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
}

// ����������ֵļ򵥺�������ӡ���
void Multimultiply(const int a, const int b)
{
    simulate_hard_computation();
    const int res = a * b;
    std::cout << "Multi: " << a << " * " << b << " = " << res << std::endl;
}

// ��Ӳ�������
void multiply_output(int& out, const int a, const int b)
{
    simulate_hard_computation();
    out = a * b;
    std::cout << a << " * " << b << " = " << out << std::endl;
}

// �������
int multiply_return(const int a, const int b)
{
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
    return res;
}

void example()
{
    // �����̳߳�
    ThreadPool pool(5);
    MultiThreadPool Multipool(5);

    // �ύMulti�˷�����
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 5; ++j)
        {
            Multipool.AddTask(i, Multimultiply, i, j);
        }

    // �ύ�˷�����
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 5; ++j)
        {
            pool.AddTask(multiply, i, j);
        }

    // ʹ��ref���ݵ���������ύ����
    int output_ref;
    auto future1 = pool.AddTask(multiply_output, std::ref(output_ref), 5, 6);

    // �ȴ��˷�������
    future1.get();
    std::cout << "Last operation result is equals to " << output_ref << std::endl;

    // ʹ��return�����ύ����
    auto future2 = pool.AddTask(multiply_return, 5, 3);

    // �ȴ��˷�������
    int res = future2.get();
    std::cout << "Last operation result is equals to " << res << std::endl;
}

int main()
{
    example();
    return 0;
}