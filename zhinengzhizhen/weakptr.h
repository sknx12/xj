#pragma once
#include "Counter.h"

// ǰ������ SharePtr�����ѭ������
template<typename T>
class SharePtr;

template<typename T>
class WeakPtr {
private:
    T* _ptr;          // ��Դָ�루��ӵ������Ȩ��
    Counter<T>* cnt;  // ��������ָ�루�� SharePtr ����

    // �ͷ������ã�����Ϊ0ʱɾ����������
    void release() {
        if (cnt) {
            cnt->w--;
            // ǿ���ú������ö�Ϊ0ʱ��ɾ���������󣨱����ڴ�й©��
            if (cnt->s == 0 && cnt->w == 0) {
                delete cnt;
            }
            // �ÿյ�ǰָ�룬����Ұָ��
            cnt = nullptr;
            _ptr = nullptr;
        }
    }

public:
    // Ĭ�Ϲ��죺��ָ��
    WeakPtr() : _ptr(nullptr), cnt(nullptr) {}

    // �� SharePtr ���죺������+1
    WeakPtr(const SharePtr<T>& sp) : _ptr(sp._ptr), cnt(sp.cnt) {
        if (cnt) cnt->w++;
    }

    // �������죺������+1
    WeakPtr(const WeakPtr<T>& wp) : _ptr(wp._ptr), cnt(wp.cnt) {
        if (cnt) cnt->w++;
    }

    // �������ͷ�������
    ~WeakPtr() { release(); }

    // ������ֵ�����ͷŵ�ǰ���ٹ����¼���
    WeakPtr<T>& operator=(const WeakPtr<T>& wp) {
        if (this != &wp) {
            release();
            cnt = wp.cnt;
            _ptr = wp._ptr;
            if (cnt) cnt->w++;
        }
        return *this;
    }

    // �� SharePtr ��ֵ�����ͷŵ�ǰ����������+1
    WeakPtr<T>& operator=(const SharePtr<T>& sp) {
        release();
        cnt = sp.cnt;
        _ptr = sp._ptr;
        if (cnt) cnt->w++;
        return *this;
    }

    // ���ķ�������ȡǿ���ã���Դ����򷵻���Ч SharePtr�����򷵻ؿգ�
    SharePtr<T> lock() const {
        return SharePtr<T>(*this);
    }

    // ���ķ������ж���Դ�Ƿ���ڣ�ǿ����Ϊ0����ڣ�
    bool expired() const {
        return cnt == nullptr || cnt->s == 0;
    }

    // ���ã��ֶ��ͷ�������
    void reset() { release(); }

    // ����ѡ�������Ե��ԣ���ȡ�����ü���
    int getWeakCount() const {
        return cnt ? cnt->w : 0;
    }

    // ��Ԫ������ SharePtr ����˽�г�Ա
    friend class SharePtr<T>;
};