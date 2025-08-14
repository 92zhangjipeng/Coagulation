#ifndef SINGLETONCASE_H
#define SINGLETONCASE_H
#include <iostream>
#include <atomic>
#include <mutex>
#include <cassert>
#include <string>

using namespace std;


class SingletonCase
{
private:
	static std::atomic<SingletonCase*> SingletonCase::sin;
	static std::mutex SingletonCase::m_mutex;
    SingletonCase(){}
public:
    static SingletonCase* getInstance()
    {
        SingletonCase* tmp = sin.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if(tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            tmp = sin.load(std::memory_order_release);
            if(tmp == nullptr)
            {
                tmp = new SingletonCase();
                std::atomic_thread_fence(std::memory_order_release);
                sin.store(tmp,std::memory_order_relaxed);
            }
        }
        return tmp;
    }  
};
















#endif // SINGLETONCASE_H
