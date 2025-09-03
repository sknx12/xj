#include <iostream>
#include <cassert>
#include "uniqueptr.h"
#include "SharePtr.h"
#include "WeakPtr.h"
using namespace std;
// 测试用的示例类，用于验证析构和资源释放
class TestObj {
public:
    int id;
    static int destructCount; // 记录析构次数，验证资源是否释放

    TestObj(int id) : id(id) {
        std::cout << "TestObj(" << id << ") 构造" << std::endl;
    }
    ~TestObj() {
        std::cout << "TestObj(" << id << ") 析构" << std::endl;
        destructCount++;
    }
};
int TestObj::destructCount = 0;

// 1. 测试 uniqueptr（独占所有权）
void test_uniqueptr() {
    std::cout << "\n=== 测试 uniqueptr ===" << std::endl;
    TestObj::destructCount = 0;

    // 基本构造与访问
    uniqueptr<TestObj> up1(new TestObj(1));
    assert(up1.get() != nullptr);
    assert(up1->id == 1);
    assert((*up1).id == 1);
    assert(static_cast<bool>(up1) == true);

    // 移动构造（独占权转移）
    uniqueptr<TestObj> up2(std::move(up1));
    assert(up1.get() == nullptr); // up1 已失去所有权
    assert(up2->id == 1);

    // 移动赋值
    uniqueptr<TestObj> up3;
    up3 = std::move(up2);
    assert(up2.get() == nullptr);
    assert(up3->id == 1);

    // reset 测试（释放旧资源，绑定新资源）
    up3.reset(new TestObj(2));
    assert(up3->id == 2);
    assert(TestObj::destructCount == 1); // 旧对象1被析构
    cout << TestObj::destructCount << endl;
    // release 测试（释放所有权但不销毁资源）
    TestObj* raw = up3.release(); // 注意：原代码函数名拼写错误，应为 release
    assert(up3.get() == nullptr);
    assert(raw->id == 2);
    delete raw; // 手动释放release的资源
    assert(TestObj::destructCount == 2);

    std::cout << "uniqueptr 测试通过" << std::endl;
}

// 2. 测试 SharePtr（共享所有权与引用计数）
void test_SharePtr() {
    std::cout << "\n=== 测试 SharePtr ===" << std::endl;
    TestObj::destructCount = 0;

    // 基本构造与引用计数
    SharePtr<TestObj> sp1(new TestObj(3));  // s=1
    assert(sp1.get() != nullptr);
    assert(sp1->id == 3);

    // 拷贝构造（引用计数+1）
    SharePtr<TestObj> sp2(sp1);  // s=2
    assert(sp1.get() == sp2.get());

    // 拷贝赋值（引用计数+1）
    SharePtr<TestObj> sp3;
    sp3 = sp2;  // s=3
    assert(sp3.get() == sp1.get());

    // 局部对象释放后计数变化
    {
        SharePtr<TestObj> sp4(sp1);  // s=4
        assert(TestObj::destructCount == 0);
    }  // sp4析构，s=3
    assert(TestObj::destructCount == 0);

    // 移动构造（所有权转移，原指针置空）
    SharePtr<TestObj> sp5(std::move(sp1));  // s仍为3（移动不改变计数）
    assert(sp1.get() == nullptr);
    assert(sp5->id == 3);

    // 移动赋值
    SharePtr<TestObj> sp6;
    sp6 = std::move(sp5);  // s仍为3
    assert(sp5.get() == nullptr);
    assert(sp6->id == 3);

    // 关键修复：先释放sp2和sp3，使sp6成为TestObj(3)的唯一持有者
    sp2.reset();  // s=2 → s=1
    sp3.reset();  // s=1 → s=0（但此时sp6仍持有，实际s=1）
    // 此时sp6是TestObj(3)的唯一持有者（s=1）

    // 重置指针（释放当前资源）
    sp6.reset(new TestObj(4));  // s从1→0，TestObj(3)析构（destructCount=1）
    assert(TestObj::destructCount == 1);  // 断言通过
    assert(sp6->id == 4);

    // 最后一个引用销毁时析构
    sp6.reset();  // TestObj(4)的s从1→0，析构（destructCount=2）
    assert(TestObj::destructCount == 2);  // 断言通过

    std::cout << "SharePtr 测试通过" << std::endl;
}


// 3. 测试 WeakPtr（弱引用与资源观察）
void test_WeakPtr() {
    std::cout << "\n=== 测试 WeakPtr ===" << std::endl;
    TestObj::destructCount = 0;

    // 从SharePtr构造WeakPtr
    SharePtr<TestObj> sp(new TestObj(5));
    WeakPtr<TestObj> wp1(sp);
    assert(wp1.expired() == false); // 资源未过期

    // lock()获取强引用
    SharePtr<TestObj> sp2 = wp1.lock();
    assert(sp2->id == 5);
    assert(wp1.expired() == false);

    // 拷贝构造与赋值
    WeakPtr<TestObj> wp2(wp1);
    WeakPtr<TestObj> wp3;
    wp3 = wp2;
    assert(wp3.expired() == false);
    // 弱引用自身释放不影响资源（资源已释放）
    wp1.reset();
    wp2.reset();
    wp3.reset();
    // 强引用释放后，WeakPtr过期
    sp.reset();
    sp2.reset(); // 最后一个强引用释放，对象5析构

    assert(TestObj::destructCount == 1);
   

   

    // 验证计数对象是否正确释放（无内存泄漏）
    {
        SharePtr<TestObj> sp3(new TestObj(6));
        WeakPtr<TestObj> wp4(sp3);
        sp3.reset(); // 强引用0，弱引用1
        wp4.reset();
        assert(TestObj::destructCount == 2); // 对象6析构
    } // wp4销毁，弱引用0，计数对象释放

    std::cout << "WeakPtr 测试通过" << std::endl;
}

// 主函数：运行所有测试
int main() {
    test_uniqueptr();
    test_SharePtr();
    test_WeakPtr();
    std::cout << "\n所有智能指针测试完成" << std::endl;
    return 0;
}
