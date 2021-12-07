#pragma once

#include "common.h"
#include "singleton.h"
#include "function_traits.h"

class NoticeCenter {
public:
    DECLARE_SINGLETON(NoticeCenter);
    template<typename FUNC>
    bool add_listener(const string& event, FUNC&& cb) {
        using cb_type = typename function_traits<typename std::remove_reference<FUNC>::type>::stl_function_type;
        shared_ptr<void> cb_ptr(new cb_type(std::forward<FUNC>(cb)), [](void *ptr) {
            cb_type *p = (cb_type *)ptr;
            delete p;
        });

        {
            lock_guard<mutex> lock(_cb_mtx);
            return _event_to_cbs.emplace(event, cb_ptr).second;
        }
    }

    void delete_listener(const string& event) {
        lock_guard<mutex> lock(_cb_mtx);
        _event_to_cbs.erase(event);
    }

    template<typename ...ArgTypes>
    void emit_event(const string& event, ArgTypes&& ...args) {
        using cb_type = function<void(decltype(std::forward<ArgTypes>(args))...)>;
        decltype(_event_to_cbs) copy; // copy一份，防止回调中操作NoticeCenter造成死锁。
        {
            lock_guard<mutex> lock(_cb_mtx);
            copy = _event_to_cbs;
        }
        auto iter = copy.find(event);
        if (iter != copy.end()) {
            cb_type* cb = (cb_type*)(iter->second.get());
            (*cb)(std::forward<ArgTypes>(args)...);
        }
    }



private:
    mutex _cb_mtx;
    unordered_map<string, shared_ptr<void>> _event_to_cbs;
};