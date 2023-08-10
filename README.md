## 使用 c++14 实现简易线程池

- 基于c++14实现的单队列和多队列线程池，主要运用c++14提供的新特性

- 使用 c++标准库中的线程+锁+条件变量实现线程池

- 使用 c++14标准库提供的 bind，forward，future，decltype等函数构造适配函数和返回类型。

- 单任务队列线程池的实现思路：在线程池构造时初始化线程数，在析构时停止线程池。对外也只需要提供提交任务的接口就够了。

- 支持调用普通函数，成员函数，Lambdas，std::bind 等任何可以通过“()”来调用的对象。

- 多任务队列的线程池的设计思路: 每个线程对应着一个自己的任务队列。当提交一个任务时，我们可以指定它放到任意一个线程的任务队列中。在用户没有指定任务队列时，就为该任务随机选择一个线程所对应的任务队列。

- 在数据量足够大的情况下，因为多任务队列的设计，多任务队列线程池性能甚至达到了单任务队列线程池的两倍。

## 测试

```
cd code

g++ ThreadPool.h MultiThreadPool.h SafeQueue.h test.cpp -pthread -o test

./test
```

![](https://github.com/XzpFlight/ThreadPool_BaseOn_Cpp14/blob/main/TestResult.png)

## 参考

https://zhuanlan.zhihu.com/p/367309864
