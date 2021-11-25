#pragma once
#include "util/common.h"
#include "util/queue.h"

#include "demuxer.h"


struct AVPacket;
using PacketQueue = Queue<unique_ptr<AVPacket, function<void(AVPacket*)>>>;

class Player {
public:
    Player() = default;
    void play(const string& filename);

private:
    void demux();
    void decode_audio();
    void decode_video();

private:
    unique_ptr<Demuxer> _demuxer;
    unique_ptr<PacketQueue> _video_packet_queue;
    unique_ptr<PacketQueue> _audio_packet_queue;

    thread _demux_thread;
    thread _video_decode_thread;
    thread _audio_decode_thread;
};