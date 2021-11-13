#include "util.h"






uint64_t get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


string get_fmt_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm* tm = localtime(&tv.tv_sec);

    char fmt_time[128];
    sprintf(fmt_time, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + tm->tm_year,
    tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000));
    return fmt_time;
}

void set_thread_name(string name) {
    assert(name.size() < 16);
    pthread_setname_np(pthread_self(), name.c_str());
}

string get_thread_name() {
    char name[16] = {0};
    prctl(PR_GET_NAME, (unsigned long)name);
    return name;
}

string get_filename_from_path(const string& path) {
    size_t found = path.rfind('/');
    return found == string::npos ? path : path.substr(found + 1);
}