#pragma once

#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>



const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void e(std::string errorMessage);
SDL_Texture* renderText(const std::string &message, SDL_Renderer *renderer);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);