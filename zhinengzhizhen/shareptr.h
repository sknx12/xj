#pragma once
#include "Counter.h"
#include "WeakPtr.h"

template<typename T>
class SharePtr {
private:
    T* _ptr;          // ��Դָ��
    Counter<T>* cnt;  // ��������ָ��

public:
    // Ĭ�Ϲ��죺��ָ��
    SharePtr() : _ptr(nullptr), cnt(nullptr) {}

    // ԭʼָ�빹�죨explicit ��ֹ��ʽת����
    explicit SharePtr(T* p) {
      
       
        if (p) {
            cnt = new Counter<T>(p);
            _ptr = p;
        }
        else {
            cnt = nullptr;
            _ptr = nullptr;
        }
    }

    // �� WeakPtr ���죨�� WeakPtr::lock() ʹ�ã�
    SharePtr(const WeakPtr<T>& wp) : _ptr(nullptr), cnt(nullptr) {
        // ����ǿ����>0ʱ���ų�����Դ������������ͷ���Դ��
        if (wp.cnt && wp.cnt->s > 0) {
            cnt = wp.cnt;
            _ptr = wp._ptr;
            cnt->s++; // ǿ���ü���+1
        }
    }

    // �������죺���������ǿ����+1
    SharePtr(const SharePtr<T>& sp) : _ptr(sp._ptr), cnt(sp.cnt) {
        if (cnt) cnt->s++;
    }

    // �ƶ����죺ת������Ȩ�������Ӽ�����
    SharePtr(SharePtr<T>&& sp) noexcept : _ptr(sp._ptr), cnt(sp.cnt) {
        // ԭ�����ÿգ���������ʱ�ظ��ͷ�
        sp._ptr = nullptr;
        sp.cnt = nullptr;
    }

    // �������ͷ�ǿ���ã�����Ϊ0ʱɾ����������
    ~SharePtr() {
        if (cnt) {
            cnt->s--;
            // ǿ���ú������ö�Ϊ0ʱ����ɾ���������󣨱����ڴ�й©��
            if (cnt->s == 0&&cnt->w==0 ) {
                delete cnt;
            }
            // �ÿյ�ǰָ�룬����Ұָ��
            cnt = nullptr;
            _ptr = nullptr;
        }
    }

    // ������ֵ�����ͷŵ�ǰ���ٹ����¼���
    SharePtr<T>& operator=(const SharePtr<T>& sp) {
        if (this != &sp) {
            // �ͷŵ�ǰǿ����
            if (cnt) {
                cnt->s--;
                if (cnt->s == 0 && cnt->w == 0) {
                    delete cnt;
                }
            }
            // �����¼���
            _ptr = sp._ptr;
            cnt = sp.cnt;
            if (cnt) cnt->s++;
        }
        return *this;
    }

    // �ƶ���ֵ��ת������Ȩ�������Ӽ�����
    SharePtr<T>& operator=(SharePtr<T>&& sp) noexcept {
        if (this != &sp) {
            // �ͷŵ�ǰǿ����
            if (cnt) {
                cnt->s--;
                if (cnt->s == 0 && cnt->w == 0) {
                    delete cnt;
                }
            }
            // ת������Ȩ
            _ptr = sp._ptr;
            cnt = sp.cnt;
            // ԭ�����ÿգ���������ʱ�ظ��ͷ�
            sp._ptr = nullptr;
            sp.cnt = nullptr;
        }
        return *this;
    }

    // �����������
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }

    // ��ȡԭʼ��Դָ��
    T* get() const { return _ptr; }

    // ����ѡ�������Ե��ԣ���ȡǿ���ü���
    int getShareCount() const {
        return cnt ? cnt->s : 0;
    }
    
    void reset(T* p = nullptr) {
        // 1. �ͷž���Դ��ԭ���߼�������
        if (cnt) {
            cnt->s--;
            if (cnt->s == 0 && cnt->w == 0) {
                delete cnt;
            }
            cnt = nullptr;
            _ptr = nullptr;
        }
        // 2. ������Դ���ص���˴���
        if (p) {
            cnt = new Counter<T>(p);  // �����¼�������
            _ptr = p;  // ȷ��_ptrָ������Դ
           
            std::cout << "[reset] ������Դ��id=" << _ptr->id << std::endl;
        }
        else {
            cnt = nullptr;
            _ptr = nullptr;
        }
    }
    bool operator==(std::nullptr_t) const {
        return _ptr == nullptr;
    }

    // 2. ����һ�� SharePtr �Ƚϣ��ж��Ƿ�ָ��ͬһ��Դ
    bool operator==(const SharePtr<T>& other) const {
        // �����߶�Ϊ�գ���ָ��ͬһԭʼ��Դ�������
        return (_ptr == other._ptr) && (cnt == other.cnt);
    }

    // 3. ����ѡ����� != ��������� == �߼��෴
    bool operator!=(std::nullptr_t) const {
        return !(*this == nullptr);
    }

    bool operator!=(const SharePtr<T>& other) const {
        return !(*this == other);
    }
    // ��Ԫ������ WeakPtr ����˽�г�Ա
    friend class WeakPtr<T>;
};