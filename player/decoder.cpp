#include "decoder.h"
#include "ffmpeg.h"

#include "util/util.h"


Decoder::Decoder(const AVCodecParameters* codec_par) {
    AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    if (!codec) {
        throw ffmpeg::FfmpegError(StrPrinter << "find decoder fail unsupported codec id" << codec_par->codec_id);
    }
    _codec_context = avcodec_alloc_context3(codec);
    if (!_codec_context) {
        throw ffmpeg::FfmpegError(StrPrinter << "alloc context fail unsupported codec id" << codec_par->codec_id);
    }
    ilog << codec_par->codec_id;
    ffmpeg::check(avcodec_parameters_to_context(_codec_context, codec_par));
    ffmpeg::check(avcodec_open2(_codec_context, codec, nullptr));
}

Decoder::~Decoder() {
    avcodec_free_context(&_codec_context);
}

const AVCodecContext* Decoder::codec_context() const {
    return _codec_context;
}

bool Decoder::send(AVPacket* packet) {
    int ret = avcodec_send_packet(_codec_context, packet);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return false; 
    } else {
        ffmpeg::check(ret);
        return true;
    }
}

int Decoder::receive(AVFrame* frame) {
    int ret = avcodec_receive_frame(_codec_context, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return ret; 
    } else {
        ffmpeg::check(ret);
        return ret;
    }
}