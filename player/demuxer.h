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

private:
    AVFormatContext* _format_context = nullptr;
};



