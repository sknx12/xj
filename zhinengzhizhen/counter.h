#pragma once
template<typename T>
class Counter {
public:
    T* ptr;    // 共享资源指针
    int s;     // 强引用计数（SharePtr 持有）
    int w;     // 弱引用计数（WeakPtr 持有）

    // 构造：初始化资源和计数
    Counter(T* p) : ptr(p), s(1), w(0) {}

    // 析构：释放资源
    ~Counter() { delete ptr; }

    // 补充：供测试间接验证计数（可选，非必需但便于调试）
    int getShareCount() const { return s; }
    int getWeakCount() const { return w; }
};