#pragma once
#include "util/common.h"

extern "C" {
    #include "libavutil/avutil.h"
}




namespace ffmpeg {

class FfmpegError : public std::runtime_error {
public:
    FfmpegError(const int error_code);
    FfmpegError(const string error);
};

string err_string(const int error_code);

double avrational_to_double(AVRational rational);

int check(const int error_code);


} // namespace ffmpeg end

