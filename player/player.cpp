#include "player.h"
#include "util/util.h"

#include "ffmpeg.h"


extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavutil/time.h>
	#include <libavutil/imgutils.h>
}


Player::Player(const string& filename) {
    _demuxer.reset(new Demuxer(filename));
    _video_packet_queue.reset(new PacketQueue(40));
    _audio_packet_queue.reset(new PacketQueue(40));
    _video_frame_queue.reset(new FrameQueue(40));
    _audio_frame_queue.reset(new FrameQueue(40));

    _video_decoder.reset(new Decoder(_demuxer->video_stream()->codecpar));
    _audio_decoder.reset(new Decoder(_demuxer->audio_stream()->codecpar));

     _format_converter.reset(new FormatConverter(
        _video_decoder->codec_context()->width, _video_decoder->codec_context()->height,
        _video_decoder->codec_context()->pix_fmt, AV_PIX_FMT_YUV420P));
    _displayer.reset(new Display(_video_decoder->codec_context()->width, _video_decoder->codec_context()->height));
}

void Player::play() {
    

    _demux_thread = thread(&Player::demux, this);
    _audio_decode_thread = thread(&Player::decode_audio, this);
    _video_decode_thread = thread(&Player::decode_video, this);


    display();

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
    set_thread_name("audio decoder");
    static int packet_count = 0;
    static int frame_count = 0;
    while (true) {
        unique_ptr<AVPacket, function<void(AVPacket*)>> packet;
        if (!_audio_packet_queue->pop(packet)) {
            ilog << "no more audio packet to decode";
            break;
        }       
        packet_count += 1;

        while(!_audio_decoder->send(packet.get())) { // 尝试至发送成功。
            unique_ptr<AVFrame, function<void(AVFrame*)>> frame(av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); } );
            while (_audio_decoder->receive(frame.get()) == 0) {
                frame_count += 1;
                // ilog << frame->pts * av_q2d(_demuxer->audio_stream()->time_base);
                // TODO 处理解码的帧
            }
        }
    }
    while (true) {
        unique_ptr<AVFrame, function<void(AVFrame *)>> frame(av_frame_alloc(), [](AVFrame *p) { av_frame_free(&p); });
        int ret = _audio_decoder->receive(frame.get());
        if (ret == AVERROR(EAGAIN)) {
            ilog << "flush the decoder";
            if (!_audio_decoder->send(nullptr)) {
                elog << " fail to flush video decoder";
            }
            continue;
        }
        else if (ret == 0) {
            frame_count += 1;
            ilog <<  "pts "  << frame->pts << "  " << frame->pts * av_q2d(_demuxer->video_stream()->time_base);
            // TODO 处理解码的帧
        } 
        else if (ret == AVERROR_EOF) {
            ilog << "end of the decoder";
            break;
        } 
    }
    ilog << "packet num " << packet_count << " frame num " << frame_count;
}

void Player::decode_video() {
    set_thread_name("video decoder");
    static int packet_count = 0;
    static int frame_count = 0;
    while (true) {
        unique_ptr<AVPacket, function<void(AVPacket *)>> packet;
        if (!_video_packet_queue->pop(packet)) {
            ilog << "no more video packet to decode";
            break;
        }
        packet_count += 1;

        while (!_video_decoder->send(packet.get())) { // 尝试至发送成功。
            unique_ptr<AVFrame, function<void(AVFrame *)>> frame(av_frame_alloc(), [](AVFrame *p) { av_frame_free(&p); });
            while (_video_decoder->receive(frame.get()) == 0) {
                frame_count += 1;
                //ilog <<  "pts "  << frame->pts << "  " << frame->pts * av_q2d(_demuxer->video_stream()->time_base);
                convert_frame(frame);
            }
        }
    }

    while (true) {
        unique_ptr<AVFrame, function<void(AVFrame *)>> frame(av_frame_alloc(), [](AVFrame *p) { av_frame_free(&p); });
        int ret = _video_decoder->receive(frame.get());
        if (ret == AVERROR(EAGAIN)) {
            ilog << "flush the decoder";
            if (!_video_decoder->send(nullptr)) {
                elog << " fail to flush video decoder";
            }
            continue;
        }
        else if (ret == 0) {
            frame_count += 1;
            //ilog <<  "pts "  << frame->pts << "  " << frame->pts * av_q2d(_demuxer->video_stream()->time_base);
            convert_frame(frame);
        } 
        else if (ret == AVERROR_EOF) {
            ilog << "end of the decoder";
            _video_frame_queue->end_input();
            break;
        } 
    }

    ilog << "packet num " << packet_count << " frame num " << frame_count;
}

void Player::convert_frame(unique_ptr<AVFrame, function<void(AVFrame*)>>& frame) {
    unique_ptr<AVFrame, function<void(AVFrame*)>> frame_converted(av_frame_alloc(), [](AVFrame* p) { av_free(p->data[0]); });

    ffmpeg::check(av_frame_copy_props(frame_converted.get(), frame.get()));
    ffmpeg::check(av_image_alloc(frame_converted->data, frame_converted->linesize, _video_decoder->codec_context()->width,
    _video_decoder->codec_context()->height, _video_decoder->codec_context()->pix_fmt, 1));
    (*_format_converter)(frame.get(), frame_converted.get());
    _video_frame_queue->push(move(frame_converted));
}


void Player::display() {
    for (int frame_num = 0; ; frame_num++) {

        _displayer->input();
        if (_displayer->get_quit()) {
            ilog << "quit play";
            break;
        }


        unique_ptr<AVFrame, function<void(AVFrame*)>> frame;
        if (!_video_frame_queue->pop(frame)) {
            ilog << "frame_num " << frame_num;
            break;
        }
        ilog <<  "pts "  << frame->pts << "  " << frame->pts * av_q2d(_demuxer->video_stream()->time_base);
        _displayer->refresh(
					{frame->data[0], frame->data[1], frame->data[2]},
					{static_cast<size_t>(frame->linesize[0]),
					 static_cast<size_t>(frame->linesize[1]),
					 static_cast<size_t>(frame->linesize[2])});
        usleep(41 * 1000);
    }

}