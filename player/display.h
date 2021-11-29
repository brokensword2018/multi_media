#pragma once
#include "SDL2/SDL.h"
#include <array>
#include <memory>

#include "util/common.h"

extern "C" {
    #include "libavformat/avformat.h"
}

struct SDL {
	SDL();
	~SDL();
};

class Player;
// 负责音视频的播放
class Display {
private:
	bool quit_{false};
	bool play_{true};

	SDL sdl_;
	std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> window_;
	std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> renderer_;
	std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> texture_;
	SDL_Event event_;

	string _pcm;
	mutex _pcm_mtx;
	Player* _player = nullptr;

public:
	Display(const unsigned width, const unsigned height, AVCodecParameters* audio_codec_par, Player* palyer);

	// Copy frame to display
	void refresh(
		std::array<uint8_t*, 3> planes, std::array<size_t, 3> pitches);
	void on_req_pcm(char* stream, int len);
	void start_play_audio();
	void stop_play_audio();

	// Handle events
	void input();

	bool get_quit();
	bool get_play();
};
