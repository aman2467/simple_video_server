/* ==========================================================================
 * @file    : display_thread.c
 *
 * @description : This file contains the video display thread.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <stdio.h>
#include <common.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>

/****************************************************************************
 * @function : This is the display thread function. It displays video locally
 *             using simple DirectMedia Layer(SDL2).
 *
 * @arg  : void
 * @return     : void
 * *************************************************************************/
void *displayThread(void)
{
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	SDL_Surface *bitmapSurface = NULL;
	char resolution[20] = {0};
	SERVER_CONFIG *serverConfig = GetServerConfig();

	SDL_Init(SDL_INIT_VIDEO);
	snprintf(resolution, 20, "Capture : %dx%d", serverConfig->capture.width, 
			                                   serverConfig->capture.height);
	win = SDL_CreateWindow(resolution, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			serverConfig->capture.width, 
			serverConfig->capture.height, 
			SDL_WINDOW_RESIZABLE);

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
			SDL_UpdateTexture(texture,0,serverConfig->disp.display_frame,serverConfig->capture.width*BPP);
		} else {
			if(serverConfig->algo_type) {
				SDL_UpdateTexture(texture,0,serverConfig->disp.sdl_frame,serverConfig->capture.width*BPP);
			} else {
				SDL_UpdateTexture(texture,0,serverConfig->disp.display_frame,serverConfig->capture.width*BPP);
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
