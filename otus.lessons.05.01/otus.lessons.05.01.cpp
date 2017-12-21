// otus.lessons.05.01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <spdlog/spdlog.h>

#include <SDL2/SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#include <chrono>
#include <ctime>
#include <boost/lexical_cast.hpp>

#include "global.h"

#include "scene.h"

int main(int argc, char** argv)
{
    auto console = spdlog::stdout_logger_st("console");
    console->info("Wellcome!");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        console->error("SDL_Init Error: {1}", SDL_GetError());
        return 1;
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0) {
        console->error("SDL_CreateWindow Error: {1}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    long width = display_mode.w >> 1;
    long height = display_mode.h >> 1;

    Scene scene;

    SDL_Window *win = SDL_CreateWindow(
                          "Hellow World!",
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED,
                          width,
                          height,
                          SDL_WINDOW_SHOWN
                      );
    if (win == nullptr) {
        console->error("SDL_CreateWindow Error: {1}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *scr = SDL_GetWindowSurface(win);
    SDL_Surface *img = SDL_CreateRGBSurface(0, scr->w, scr->h, 32, 0, 0, 0, 0);

    scene.init(width, height, 9);

    auto start = std::chrono::system_clock::now();
    auto last = start;

    int count = 0;
    int last_count = 0;

    double time_step = 0.0;

    bool run = true;
    while (run) {
        auto loop_start = std::chrono::system_clock::now();
        ++count;

        SDL_LockSurface(img);

        scene.process(img, time_step > 0.01 ? 0.01 : time_step);

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

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> full_elapsed = end - start;
        std::chrono::duration<double> last_elapsed = end - last;
        std::chrono::duration<double> loop_elapsed = end - loop_start;
        time_step = loop_elapsed.count();

        if (!run || last_elapsed.count() >= 1) {
            int frames = count - last_count;
            double fps = ((double)frames) / last_elapsed.count();

            SDL_SetWindowTitle(win, ("Hello World! FPS: " + boost::lexical_cast<std::string>(fps)).c_str());

            console->info("[{0} / {1}] fps: {2}; time_step: {3}", full_elapsed.count(), count, fps, time_step);

            last = end;
            last_count = count;
        }

    }

    SDL_FreeSurface(img);
    SDL_FreeSurface(scr);

    SDL_DestroyWindow(win);
    SDL_Quit();

    scene.done();

    console->info("Goodby!");

    return 0;
}

