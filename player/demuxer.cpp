#include "demuxer.h"
#include "ffmpeg.h"




Demuxer::Demuxer(const string& filename) {
    ffmpeg::check(avformat_open_input(&_format_context, filename.c_str(), nullptr, nullptr));

    ffmpeg::check(avformat_find_stream_info(_format_context, nullptr));

	_audio_index = ffmpeg::check(av_find_best_stream(_format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0));
	_video_index = ffmpeg::check(av_find_best_stream(_format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0));
}

Demuxer::~Demuxer() {
    avformat_close_input(&_format_context);
}


bool Demuxer::get_avpacket(AVPacket& pkt) {
    return av_read_frame(_format_context, &pkt) == 0;
}


int Demuxer::audio_index() {
    return _audio_index;
}
int Demuxer::video_index() {
    return _video_index;
}