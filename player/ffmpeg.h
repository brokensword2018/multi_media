#pragma once
#include "util/common.h"

extern "C" {
    #include "libavutil/avutil.h"
}

string err_string(const int error_code);


namespace ffmpeg {

class FfmpegError : public std::runtime_error {
public:
    FfmpegError(const int error_code);
};

int check(const int error_code);


} // namespace ffmpeg end

