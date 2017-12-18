// otus.lessons.05.01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SDL2\SDL.h>
#include <spdlog\spdlog.h>

#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")

int main(int argc, char** argv)
{
	auto console = spdlog::stdout_logger_st("console");
	console->info("Wellcome!");

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		console->error("SDL_Init Error: {1}", SDL_GetError());
		return 1;
	}
	SDL_Window *win = SDL_CreateWindow(
		"Hellow World!",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		512,
		512,
		SDL_WINDOW_SHOWN
	);
	if (win == nullptr) {
		console->error("SDL_CreateWindow Error: {1}", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Surface *scr = SDL_GetWindowSurface(win);
	SDL_Surface *img = SDL_CreateRGBSurface(0, scr->w, scr->h, 32, 0, 0, 0, 0);

	bool run = true;
	while (run)
	{
		SDL_LockSurface(img);

		SDL_UnlockSurface(img);

		SDL_BlitSurface(img, nullptr, scr, nullptr);

		SDL_UpdateWindowSurface(win);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT
				|| event.type == SDL_KEYDOWN
				|| event.type == SDL_KEYUP) {
				run = false;
			}
		}
	}

	SDL_FreeSurface(img);
	SDL_FreeSurface(scr);

	SDL_DestroyWindow(win);
	SDL_Quit();

	console->info("Goodby!");

	return 0;
}

