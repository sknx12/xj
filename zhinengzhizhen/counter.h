#pragma once
template<typename T>
class Counter {
public:
    T* ptr;    // ������Դָ��
    int s;     // ǿ���ü�����SharePtr ���У�
    int w;     // �����ü�����WeakPtr ���У�

    // ���죺��ʼ����Դ�ͼ���
    Counter(T* p) : ptr(p), s(1), w(0) {}

    // �������ͷ���Դ
    ~Counter() { delete ptr; }

    // ���䣺�����Լ����֤��������ѡ���Ǳ��赫���ڵ��ԣ�
    int getShareCount() const { return s; }
    int getWeakCount() const { return w; }
};