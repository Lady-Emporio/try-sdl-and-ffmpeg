//
//
//#include <iostream>
//#include "sky.h"
////#include <string>
////#include <vector>
////
////#include <SDL.h>
////#include <SDL_thread.h>
////#include <SDL_ttf.h>
//
//
//
//int main(int argc, char *argv[]) {
//	if (TTF_Init() == -1) {
//		e("ttf init");
//	}
//
//	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
//	{
//		e("sdl init");
//	}
//	SDL_Surface* screen_surface = NULL;
//	SDL_Window* window = NULL;
//	window = SDL_CreateWindow("Draw text", SDL_WINDOWPOS_UNDEFINED,
//		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
//		SDL_WINDOW_SHOWN);
//
//	if (window == NULL) {
//		e("sdl window");
//	}
//	screen_surface = SDL_GetWindowSurface(window);
//
//	SDL_Renderer *renderer;
//	SDL_Texture *texture;
//
//	renderer = SDL_CreateRenderer(window, -1, 0);
//	if (!renderer) {
//		e("! render");
//	};
//
//	texture = SDL_CreateTexture(
//		renderer,
//		SDL_PIXELFORMAT_RGB24,
//		SDL_TEXTUREACCESS_STREAMING,
//		1920,
//		1080);
//	if (!texture) {
//		e("! texture");
//	}
//
//
//
//	SDL_Texture *image = renderText("TTF fonts are cool!",renderer);
//
//
//
//
//
//	SDL_Event event;
//
//	while (true) {
//		while (SDL_PollEvent(&event) != 0) {
//			if (event.type == SDL_QUIT) {
//				e("exit");
//			}
//			if (event.type == SDL_KEYDOWN) {
//				if (event.key.keysym.sym == SDLK_RIGHT) {
//				}
//				if (event.key.keysym.sym == SDLK_LEFT) {
//				}
//			}
//		}
//
//
//		SDL_RenderClear(renderer);
//		//We can draw our message as we do any other texture, since it's been
//		//rendered to a texture
//		int x = 0;
//		int y = 0;
//		renderTexture(image, renderer, x, y,400,200);
//		SDL_RenderPresent(renderer);
//	}
//
//
//	return 0;
//}
