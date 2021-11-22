#include "ffmpeg.h"



string err_string(const int error_code) {
    char str[64] = {0};
    av_make_error_string(str, sizeof(str), error_code);
    return str;
}


namespace ffmpeg {
FfmpegError::FfmpegError(const int error_code)
:std::runtime_error(err_string(error_code))
{

}

int check(const int error_code) {
    if (error_code < 0) {
        throw FfmpegError(error_code);
    }
    return error_code;
}

}// namespace ffmpeg end