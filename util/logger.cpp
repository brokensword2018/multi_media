#include "logger.h"
#include "util.h"

#include <pthread.h>



IMPL_SINGLETON(AsyncLogWriter);


AsyncLogWriter::AsyncLogWriter() {
    _front = std::make_shared<vector<string>>();
    _back = std::make_shared<vector<string>>();
    _thread = std::make_shared<std::thread>([this]() {
        pthread_setname_np(pthread_self(), "async log");
        this->loop();
    });
}

void AsyncLogWriter::loop() {
    while (true) {
        flush();
        std::unique_lock<std::mutex> lock(_mtx);
        while (_front->empty()) {
            _need_flush.wait(lock);
        }
    }
}

void AsyncLogWriter::flush() {
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _front.swap(_back);
    }
    for (std::string& log : *_back) {
        std::cout << log << endl;
    }
    _back->clear();
}

void AsyncLogWriter::write(const std::string& log) {
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _front->push_back(log);
    }
    _need_flush.notify_one();
}

void AsyncLogWriter::join() {
    ilog << "join the async thread";
    _thread->join();
}

LogContext::LogContext(const LogLevel level, const std::string& filename, const uint32_t line, const string& func_name) {
    _prefix << log_level_map.at(level) << " ";
    _prefix << get_fmt_time() << " ";
    _prefix << "[" << gettid() << "-" << get_thread_name() << "] ";
    _prefix << get_filename_from_path(filename) << ":" << line << " " << func_name <<  " | ";

    _log.flags(ios::left);
    _log.precision(8);
}

LogContext::~LogContext() {
    AsyncLogWriter::instance().write(_prefix.str() + _log.str());
}