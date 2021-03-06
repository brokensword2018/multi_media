#pragma once
#include "util/common.h"
#include "util/queue.h"

#include "demuxer.h"
#include "decoder.h"
#include "format_converter.h"
#include "display.h"


struct AVPacket;
using PacketQueue = Queue<unique_ptr<AVPacket, function<void(AVPacket*)>>>;
using FrameQueue = Queue<unique_ptr<AVFrame, function<void(AVFrame*)>>>;

class Player {
public:
    Player(const string& filename);
    void play();
    string get_pcm();

private:
    void demux();
    void decode_audio();
    void convert_audio_frame(unique_ptr<AVFrame, function<void(AVFrame*)>>& frame);
    void decode_video();
    void convert_video_frame(unique_ptr<AVFrame, function<void(AVFrame*)>>& frame);
    void display();


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

    unique_ptr<AudioFormatConverter> _audio_format_converter;
    unique_ptr<VideoFormatConverter> _video_format_converter;
    unique_ptr<Display> _displayer;
    double _audio_pts = 0.0;
    mutex _audio_pts_mtx;
};