#include "logger.h"

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
    _thread->join();
}