#pragma once
#include "common.h"



uint64_t get_time_ms();

string get_fmt_time();

void set_thread_name(string name);

string get_thread_name();

string get_filename_from_path(const string& path);

#define StrPrinter _StrPrinter()
class _StrPrinter : public string {
public:
    template<class T>
    _StrPrinter& operator << (T&& data) {
        _ss << std::forward<T>(data);
        string::operator=(_ss.str());//必须要显示调用父类的，子类有默认的operator=
        return *this;
    }

private:
    stringstream _ss;
};


