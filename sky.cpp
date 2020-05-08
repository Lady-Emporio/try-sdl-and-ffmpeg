#include <iostream>
#include "sky.h"

void e(std::string errorMessage) {
	std::cout << "Error :'" << errorMessage << "'." << std::endl;
	exit(2);
}

SDL_Texture* renderText(const std::string &message, SDL_Renderer *renderer)
{
	SDL_Color color = { 255, 0, 0, 255 };
	int fontSize = 14;
	//Open the font
	TTF_Font *font = TTF_OpenFont("ariali.ttf", fontSize);
	if (font == nullptr) {
		std::cout << TTF_GetError() << std::endl;
		e("TTF_OpenFont");
		return nullptr;
	}
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr) {
		TTF_CloseFont(font);
		e("TTF_RenderText");
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr) {
		e("CreateTexture");
	}
	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}