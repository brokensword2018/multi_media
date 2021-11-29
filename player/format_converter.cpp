#include "format_converter.h"
#include "ffmpeg.h"

FormatConverter::FormatConverter(size_t width, size_t height, 
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

void FormatConverter::operator()(AVFrame* src, AVFrame* dst) {
    sws_scale(_sws_context,
		// Source
		src->data, src->linesize, 0, _height,
		// Destination
		dst->data, dst->linesize);	
}