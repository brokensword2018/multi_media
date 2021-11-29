#pragma once

extern "C" {
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
    #include "libavutil/samplefmt.h"
    #include "libswresample/swresample.h"
}

class VideoFormatConverter {
public:
    VideoFormatConverter(size_t width, size_t height, AVPixelFormat input_format, AVPixelFormat output_format);
    ~VideoFormatConverter();
    void operator()(AVFrame* src, AVFrame* dst);

private:
    size_t _width = 0;
    size_t _height = 0;
    SwsContext *_sws_context = nullptr;
};


class AudioFormatConverter {
public:
    AudioFormatConverter(AVSampleFormat format, int channels, int channel_layout, int samplerate);
    ~AudioFormatConverter();
    void operator()(AVFrame* src, AVFrame* dst);
private:
    AVSampleFormat _sample_format = AV_SAMPLE_FMT_NONE;
    int _out_channels = 0;
    int _out_channel_layout = 0;
    int _out_samplerate = 0;
    SwrContext* _swr_context = nullptr;
};