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
    {debug, "debug"},
    {info, "info"},
    {warning, "warning"},
    {error, "error"},
};

class LogContext {
public:
    LogContext(const string& time, const LogLevel level, const uint32_t tid, const string& thread_name, 
              const std::string& file_name, const uint32_t line);
    ~LogContext();


private:
    string _prefix;
    ostringstream _log;
};