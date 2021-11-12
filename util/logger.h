#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "singleton.h"
#include "types.h"
#include "common.h"

using namespace std;

class AsyncLogWriter {
public:
    DECLARE_SINGLETON(AsyncLogWriter);
    AsyncLogWriter();
    void loop();
    void flush();
    void write(const std::string& log);
    void join();

private:
    VecStrPtr _front;
    VecStrPtr _back;
    std::shared_ptr<std::thread> _thread;
    std::mutex _mtx;
    std::condition_variable _need_flush;
};

enum LogLevel {
    debug = 0,
    info,
    warning,
    error,
};

const static map<LogLevel, string> log_level_map = 
{
    {debug, "D"},
    {info, "I"},
    {warning, "W"},
    {error, "E"},
};

class LogContext {
public:
    LogContext(const LogLevel level, const std::string& filename, const uint32_t line, const string& func_name);
    ~LogContext();
    template<typename T>
    LogContext& operator<<(T&& val) {
        _log << std::forward<T>(val);
        return *this;
    }

private:
    ostringstream _prefix;
    ostringstream _log;
};


#define WriteLog(level) LogContext(level, __FILE__, __LINE__, __FUNCTION__)
#define dlog WriteLog(debug)
#define ilog WriteLog(info)
#define wlog WriteLog(warning)
#define elog WriteLog(error)