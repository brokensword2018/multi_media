#include "player.h"
#include "util/util.h"

#include "ffmpeg.h"


void Player::play(const string& filename) {
    _demuxer.reset(new Demuxer(filename));
    _video_packet_queue.reset(new PacketQueue(40));
    _audio_packet_queue.reset(new PacketQueue(40));
    _video_frame_queue.reset(new FrameQueue(40));
    _audio_frame_queue.reset(new FrameQueue(40));

    _video_decoder.reset(new Decoder(_demuxer->video_stream()->codecpar));
    _audio_decoder.reset(new Decoder(_demuxer->audio_stream()->codecpar));

    _demux_thread = thread(&Player::demux, this);
    _audio_decode_thread = thread(&Player::decode_audio, this);
    _video_decode_thread = thread(&Player::decode_video, this);


    _demux_thread.join();
    _audio_decode_thread.join();
    _video_decode_thread.join();
    ilog << "all thread end";
}

void Player::demux() {
    set_thread_name("demux");
    while (true)
    {
       unique_ptr<AVPacket, function<void(AVPacket*)>> packet(
           av_packet_alloc(), 
           [](AVPacket*p) {  av_packet_free(&p); });

        if (!_demuxer->get_avpacket(*packet)) {
            ilog << "no more packet to demux";
            _video_packet_queue->end_input();
            _audio_packet_queue->end_input();
            break;
        }
        
        if (packet->stream_index == _demuxer->video_stream()->index) {
            _video_packet_queue->push(move(packet));
        } else if (packet->stream_index == _demuxer->audio_stream()->index) {
            _audio_packet_queue->push(move(packet));
        }
    }
    
}

void Player::decode_audio() {
    set_thread_name("decode_audio");
    static int packet_count = 0;
    static int frame_count = 0;
    while (true) {
        unique_ptr<AVPacket, function<void(AVPacket*)>> packet;
        if (!_audio_packet_queue->pop(packet)) {
            ilog << "no more audio packet to decode";
            _audio_frame_queue->end_input();
            break;
        }       
        packet_count += 1;

        while(!_audio_decoder->send(packet.get())) { // 尝试至发送成功。
            unique_ptr<AVFrame, function<void(AVFrame*)>> frame(av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); } );
            while (_audio_decoder->receive(frame.get())) {
                frame_count += 1;
                ilog << frame->pts * ffmpeg::avrational_to_double(_demuxer->audio_stream()->time_base);
            }

        }
    }
    ilog << "packet num " << packet_count << " frame num " << frame_count;
}

void Player::decode_video() {
    set_thread_name("decode_video");
    static int packet_count = 0;
    static int frame_count = 0;
    while (true) {
        unique_ptr<AVPacket, function<void(AVPacket *)>> packet;
        if (!_video_packet_queue->pop(packet)) {
            _video_frame_queue->end_input();
            ilog << "no more video packet to decode";
            break;
        }
        packet_count += 1;

        while (!_video_decoder->send(packet.get())) { // 尝试至发送成功。
            unique_ptr<AVFrame, function<void(AVFrame *)>> frame(av_frame_alloc(), [](AVFrame *p) { av_frame_free(&p); });
            while (_video_decoder->receive(frame.get())) {
                frame_count += 1;
                ilog << frame->pts * ffmpeg::avrational_to_double(_demuxer->video_stream()->time_base);
            }
        }
    }
    ilog << "packet num " << packet_count << " frame num " << frame_count;
}