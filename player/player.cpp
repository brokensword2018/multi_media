#include "player.h"
#include "util/util.h"


void Player::play(const string& filename) {
    _demuxer.reset(new Demuxer(filename));
    _video_packet_queue.reset(new PacketQueue(40));
    _audio_packet_queue.reset(new PacketQueue(40));

    _demux_thread = thread(&Player::demux, this);
    _audio_decode_thread = thread(&Player::decode_audio, this);
    _video_decode_thread = thread(&Player::decode_video, this);


    _demux_thread.join();
    _audio_decode_thread.join();
    _video_decode_thread.join();
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
        
        if (packet->stream_index == _demuxer->video_index()) {
            _video_packet_queue->push(move(packet));
        } else if (packet->stream_index == _demuxer->audio_index()) {
            _audio_packet_queue->push(move(packet));
        }
    }
    
}

void Player::decode_audio() {
    set_thread_name("decode_audio");
    while (true) {
        unique_ptr<AVPacket, function<void(AVPacket*)>> packet;
        if (!_audio_packet_queue->pop(packet)) {
            ilog << "no more audio packet to decode";
            break;
        }
        static int count = 0;
        ilog << count++;
    }
    
}

void Player::decode_video() {
    set_thread_name("decode_video");
    while (true) {
        unique_ptr<AVPacket, function<void(AVPacket *)>> packet;
        if (!_video_packet_queue->pop(packet)) {
            ilog << "no more video packet to decode";
            break;
        }

        static int count = 0;
        ilog << count++;
    }
}