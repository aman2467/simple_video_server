
#include <stdio.h>
#include <common.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>

extern char *display_frame;
extern char *sdl_frame;

void *displayThread(void)
{
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	SDL_Surface *bitmapSurface = NULL;
	char resolution[20] = {0};
	SERVER_CONFIG *serverConfig = GetServerConfig();

	SDL_Init(SDL_INIT_VIDEO);
	snprintf(resolution, 20, "Capture : %dx%d", serverConfig->capture.width, serverConfig->capture.height);
	win = SDL_CreateWindow(resolution, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			serverConfig->capture.width, 
			serverConfig->capture.height, 
			SDL_WINDOW_RESIZABLE);/* SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_BORDERLESS */ 

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_YUY2,SDL_TEXTUREACCESS_STREAMING,
			serverConfig->capture.width,serverConfig->capture.height);

	while (!KillDisplayThread) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}
		if(serverConfig->enable_osd_thread) {
			SDL_UpdateTexture(texture,0,display_frame,serverConfig->capture.width*BPP);
		} else {
			if(serverConfig->algo_type) {
				SDL_UpdateTexture(texture,0,sdl_frame,serverConfig->capture.width*BPP);
			} else {
				SDL_UpdateTexture(texture,0,display_frame,serverConfig->capture.width*BPP);
			}
		}
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	SDL_Quit();

	return 0;
}
