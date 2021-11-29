#include "format_converter.h"
#include "ffmpeg.h"

VideoFormatConverter::VideoFormatConverter(size_t width, size_t height, 
AVPixelFormat input_format, AVPixelFormat output_format)
: _width(width),
_height(height)
{
    _sws_context = sws_getContext(
		// Source
		width, height, input_format,
		// Destination
		width, height, output_format,
		// Filters
		SWS_BICUBIC, nullptr, nullptr, nullptr);
    if (!_sws_context) {
        throw ffmpeg::FfmpegError("sws_getContext fail");
    }
}

VideoFormatConverter::~VideoFormatConverter() {
	sws_freeContext(_sws_context);
}

void VideoFormatConverter::operator()(AVFrame* src, AVFrame* dst) {
    sws_scale(_sws_context,
		// Source
		src->data, src->linesize, 0, _height,
		// Destination
		dst->data, dst->linesize);	
}

AudioFormatConverter::AudioFormatConverter(AVSampleFormat format, int channels, int channel_layout, int samplerate) {
	_sample_format = format;
	_out_channels = channels;
	_out_channel_layout = channel_layout;
	_out_samplerate = samplerate;
}

AudioFormatConverter::~AudioFormatConverter() {
	swr_free(&_swr_context);
}

void AudioFormatConverter::operator()(AVFrame* src, AVFrame* dst) {
	if (!_swr_context) {
		_swr_context = swr_alloc_set_opts(nullptr, _out_channel_layout, _sample_format, _out_samplerate,
                                  src->channel_layout, (AVSampleFormat) src->format,
                                  src->sample_rate, 0, nullptr);
	}
	if (!_swr_context) {
		throw ffmpeg::FfmpegError("swr_alloc_set_opts fail");
	}
	dst->format = _sample_format;
	dst->channels = _out_channels;
	dst->channel_layout = _out_channel_layout;
	dst->sample_rate = _out_samplerate;
	dst->pkt_dts = src->pkt_dts;
	dst->pts = src->pts;
	ffmpeg::check(swr_convert_frame(_swr_context, dst, src));
}



