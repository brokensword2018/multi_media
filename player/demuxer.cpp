#include "demuxer.h"
#include "ffmpeg.h"




Demuxer::Demuxer(const string& filename) {
    ffmpeg::check(avformat_open_input(&_format_context, filename.c_str(), nullptr, nullptr));
}

Demuxer::~Demuxer() {
    avformat_close_input(&_format_context);
}


bool Demuxer::get_avpacket(AVPacket& pkt) {
    return av_read_frame(_format_context, &pkt) == 0;
}