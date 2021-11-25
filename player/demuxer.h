#pragma once
#include "util/common.h"


extern "C" {
    #include "libavformat/avformat.h"
}



class Demuxer {
public:
    Demuxer(const string& filename);
    bool get_avpacket(AVPacket& pkt);
    ~Demuxer();
    AVStream* video_stream() const;
    AVStream* audio_stream() const;

private:
    AVFormatContext* _format_context = nullptr;
    int _audio_index = 0;
    int _video_index = 0;
};



