#pragma once
#include "util/common.h"
#include "util/queue.h"

#include "demuxer.h"
#include "decoder.h"


struct AVPacket;
using PacketQueue = Queue<unique_ptr<AVPacket, function<void(AVPacket*)>>>;
using FrameQueue = Queue<unique_ptr<AVFrame, function<void(AVFrame*)>>>;

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
    unique_ptr<FrameQueue> _video_frame_queue;
    unique_ptr<FrameQueue> _audio_frame_queue;

    unique_ptr<Decoder> _video_decoder;
    unique_ptr<Decoder> _audio_decoder;

    thread _demux_thread;
    thread _video_decode_thread;
    thread _audio_decode_thread;
};