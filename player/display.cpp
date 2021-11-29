#include "display.h"
#include <stdexcept>

#include "player.h"

template <typename T>
inline T check_SDL(T value, const std::string &message) {
	if (!value) {
		string msg = "SDL " + message + string(SDL_GetError());
		throw std::runtime_error{msg};
	} else {
		return value;
	}
}

SDL::SDL() {
	check_SDL(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO), "init");
}

SDL::~SDL() {
	SDL_Quit();
}

Display::Display(const unsigned width, const unsigned height, AVCodecParameters* audio_codec_par, Player* player) :
	window_{check_SDL(SDL_CreateWindow(
		"player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
		"window"), SDL_DestroyWindow},
	renderer_{check_SDL(SDL_CreateRenderer(
		window_.get(), -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
		"renderer"), SDL_DestroyRenderer},
	texture_{check_SDL(SDL_CreateTexture(
		renderer_.get(), SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
		width, height), "renderer"), SDL_DestroyTexture} {

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer_.get(), width, height);

	SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);
	SDL_RenderClear(renderer_.get());
	SDL_RenderPresent(renderer_.get());

	_player = player;

	// 音频的初始化
	SDL_AudioSpec wanted_spec;
    wanted_spec.freq = audio_codec_par->sample_rate;
    wanted_spec.format = AUDIO_S16;
    wanted_spec.channels = audio_codec_par->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.userdata = this;
    wanted_spec.callback = [](void *userdata, Uint8 *stream, int len) {
        Display *_this = (Display *) userdata;
        _this->on_req_pcm((char *) stream, len);
    };
    if (SDL_OpenAudio(&wanted_spec, nullptr) < 0) {
        throw std::runtime_error("SDL_OpenAudio failed");
    }

}

void Display::refresh(
	std::array<uint8_t*, 3> planes, std::array<size_t, 3> pitches) {
	check_SDL(!SDL_UpdateYUVTexture(
		texture_.get(), nullptr,
		planes[0], pitches[0],
		planes[1], pitches[1],
		planes[2], pitches[2]), "texture update");
	SDL_RenderClear(renderer_.get());
	SDL_RenderCopy(renderer_.get(), texture_.get(), nullptr, nullptr);
	SDL_RenderPresent(renderer_.get());
}


void Display::on_req_pcm(char* stream, int len) {
	SDL_memset(stream, 0, len);
	string pcm = _player->get_pcm();
	len = min(len, (int)pcm.size());
    memcpy(stream, pcm.data(), len);
    return;
}


void Display::start_play_audio() {
	SDL_PauseAudio(0);
}

void Display::stop_play_audio() {
	SDL_PauseAudio(1);
}


void Display::input() {
	if (SDL_PollEvent(&event_)) {
		switch (event_.type) {
		case SDL_KEYUP:
			switch (event_.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit_ = true;
				break;
			case SDLK_SPACE:
				play_ = !play_;
				break;
			default:
				break;
			}
			break;
		case SDL_QUIT:
			quit_ = true;
			break;
		default:
			break;
		}
	}
}

bool Display::get_quit() {
	return quit_;
}

bool Display::get_play() {
	return play_;
}
