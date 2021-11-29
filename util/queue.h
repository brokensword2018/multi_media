#pragma once
#include "common.h"


template<class T>
class Queue {
public:
    Queue(const uint32_t size = 40);

    bool push(T&& msg);
    bool push(const T& msg);
    bool pop(T& msg);
    bool try_pop(T& msg);
    void end_input();
    void quit();
    size_t size();



private:
    queue<T> _msgs;
    uint32_t _max_size = 0;
    mutex _mtx;
    condition_variable _not_empty;
    condition_variable _not_full;
    atomic_bool _end_input{false}; // 结束push
    atomic_bool _quit{false};  // 无法push 和 pop
};


template<class T>
Queue<T>::Queue(const uint32_t size)
:_max_size(size)
{
   
}

template<class T>
bool Queue<T>::push(T&& msg) {
    unique_lock<mutex> lock(_mtx);
    while (!_quit && !_end_input) {
        if (_msgs.size() >= _max_size) {
            _not_full.wait(lock);
        } else {
            _msgs.push(move(msg));
            _not_empty.notify_all();
            return true;
        }
    }
    return false;
}

template<class T>
bool Queue<T>::push(const T& msg) {
    unique_lock<mutex> lock(_mtx);
    while (!_quit && !_end_input) {
        if (_msgs.size() >= _max_size) {
            _not_full.wait(lock);
        } else {
            _msgs.push(msg);
            _not_empty.notify_all();
            return true;
        }
    }
    return false;
}

template<class T>
bool Queue<T>::pop(T& msg) {
    unique_lock<mutex> lock(_mtx);

    while (!_quit) {
        if (!_msgs.empty()) {
            msg = move(_msgs.front());
            _msgs.pop();
            _not_full.notify_all();
            return true;
        }
        if (_end_input) {
            return false;
        }
        _not_empty.wait(lock);
    }
    return false;
}

template<class T>
bool Queue<T>::try_pop(T& msg) {
    unique_lock<mutex> lock(_mtx);

    while (!_quit) {
        if (!_msgs.empty()) {
            msg = move(_msgs.front());
            _msgs.pop();
            _not_full.notify_all();
            return true;
        }
        if (_end_input) {
            return false;
        }
        return false;
    }
    return false;
}


template<class T>
void Queue<T>::end_input() {
    _end_input = true;
    _not_empty.notify_all();
}

template<class T>
void Queue<T>::quit() {
    _quit = true;
    _not_empty.notify_all();
    _not_full.notify_all();
}

template<class T>
size_t Queue<T>::size() {
    unique_lock<mutex> lock(_mtx);
    return _msgs.size();
}
