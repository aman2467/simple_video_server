/* ==========================================================================
 * @file    : video_player.c
 *
 * @description : This file contains the code of video player.
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
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>

#define BPP 2

/****************************************************************************
 * @function : This is the video player main function. It plays recorded video
 *             using simple DirectMedia Layer(SDL2).
 *
 * @arg  : command line arg
 * @return     : success/failure
 * *************************************************************************/
int main(int argc, char **argv )
{
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	char resolution[20] = {0};
	int width,height;
	FILE *fp;
	char *frame = NULL;
	int fps = 0, delay = 0;

	if(argc != 5) {
		printf("Usage: %s <YUY2_file.raw> <width> <height> <fps>\n",argv[0]);
		exit(0);
	}
	SDL_Init(SDL_INIT_VIDEO);
	width = atoi(argv[2]);
	height = atoi(argv[3]);
	fps = atoi(argv[4]);
	delay = 1000000/(fps+15);
	snprintf(resolution, 20, "Capture : %dx%d", width,height);
	frame = calloc(1,width*height*BPP);
	win = SDL_CreateWindow(resolution, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			width, 
			height, 
			SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_YUY2,
			SDL_TEXTUREACCESS_STREAMING,
			width,
			height);
	fp = fopen(argv[1],"rb");
	if(fp == NULL) {
		perror("fopen");
		return -1;
	}
	while (1) {
		SDL_Event e;

		if(fread(frame,1,width*height*BPP,fp) < width*height*BPP) {
			fclose(fp);
			fp = fopen(argv[1],"rb");
			if(fread(frame,1,width*height*BPP,fp) < width*height*BPP);
		}
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			} else if(e.key.type == SDL_KEYUP) {
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					break;
				}
			}
		}
		SDL_UpdateTexture(texture,0,
					frame,
					width*BPP);
		usleep(delay);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	free(frame);
	fclose(fp);
	SDL_Quit();

	return 0;
}
