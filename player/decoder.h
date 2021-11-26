#pragma once
#include "util/common.h"


extern "C" {
    #include "libavcodec/avcodec.h"
}




class Decoder {
public:
    Decoder(const AVCodecParameters* codec_par);
    ~Decoder();
    const AVCodecContext* codec_context() const;
    bool send(AVPacket* packet);
    int receive(AVFrame* frame);
private:
    AVCodecContext* _codec_context = nullptr;
};