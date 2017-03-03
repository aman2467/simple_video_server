/*
 * @file    : display_thread.c
 *
 * @description : This file contains the video display thread.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

#include <stdio.h>
#include <common.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_keyboard.h>

/*
 * @function : This is the display thread function. It displays video locally
 *             using simple DirectMedia Layer(SDL2).
 *
 * @arg  : void
 * @return     : void
 */
void *displayThread(void)
{
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	char resolution[20] = {0};
	int kill = 0;
	SERVER_CONFIG *serverConfig = GetServerConfig();
#ifdef INPUT_TEXT
	int done = 0;
	char text[50] = {0};
#endif
	SDL_Init(SDL_INIT_VIDEO);
	snprintf(resolution, 20, "Capture : %dx%d",
		 serverConfig->capture.width,
		 serverConfig->capture.height);
	win = SDL_CreateWindow(resolution,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			serverConfig->capture.width,
			serverConfig->capture.height,
			SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_YUY2,
			SDL_TEXTUREACCESS_STREAMING,
			serverConfig->capture.width,
			serverConfig->capture.height);
#ifdef INPUT_TEXT
	SDL_StartTextInput();
	while (!done) {
		SDL_Event ev;

		if (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				break;
			case SDL_TEXTINPUT:
				strcat(text, ev.text.text);
				if (strlen(text) >= 5)
					memset(text, 0, 50);
				else if (!strcmp(text, "aman"))
					done = 1;
				break;
			case SDL_TEXTEDITING:
				printf("text editing occurred\n");
				break;
			}
		}
	}
	SDL_StopTextInput();
	printf("%s\n", text);
#endif

	while (!KillDisplayThread) {
		SDL_Event e;

		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
			else if (e.key.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
				case SDLK_RIGHT:
					if (e.key.state == SDL_RELEASED) {
						serverConfig->algo_type++;
						if (serverConfig->algo_type > ALGO_MULTI_3)
							serverConfig->algo_type
								= ALGO_NONE;
					}
					break;
				case SDLK_LEFT:
					if (e.key.state == SDL_RELEASED) {
						serverConfig->algo_type--;
						if (serverConfig->algo_type < ALGO_NONE)
							serverConfig->algo_type
								= ALGO_MULTI_3;
					}
					break;
				case SDLK_c:
					if (serverConfig->enable_imagesave_thread) {
						serverConfig->image.type = 0;
						serverConfig->image.recordenable
							= TRUE;
					}
					break;
				case SDLK_v:
					if (serverConfig->enable_videosave_thread) {
						serverConfig->video.recordenable
							^= TRUE;
					}
					break;
				case SDLK_ESCAPE:
					kill = TRUE;
					break;
				default:
					break;
				}
				if (kill)
					break;
			}
		}
		if (serverConfig->enable_osd_thread)
			SDL_UpdateTexture(texture, 0,
					serverConfig->disp.display_frame,
					serverConfig->capture.width*BPP);
		else {
			if (serverConfig->algo_type)
				SDL_UpdateTexture(texture, 0,
					  serverConfig->disp.sdl_frame,
					  serverConfig->capture.width*BPP);
			else
				SDL_UpdateTexture(texture, 0,
					  serverConfig->disp.display_frame,
					  serverConfig->capture.width*BPP);
		}
		usleep(30000);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	SDL_Quit();
	exit(0);
	return 0;
}
