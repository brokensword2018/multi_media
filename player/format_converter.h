#pragma once

extern "C" {
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
}

class FormatConverter {
public:
    FormatConverter(size_t width, size_t height, AVPixelFormat input_format, AVPixelFormat output_format);
    void operator()(AVFrame* src, AVFrame* dst);
private:
    size_t _width = 0;
    size_t _height = 0;
    SwsContext* _sws_context = nullptr;
};