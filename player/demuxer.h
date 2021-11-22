#pragma once
#include "util/common.h"


extern "C" {
    #include "libavformat/avformat.h"
}



class Demuxer {
public:
    Demuxer(const string& filename);
    bool getAVpacket(AVPacket& pkt);
    ~Demuxer();

private:
    AVFormatContext* _format_context = nullptr;
};



