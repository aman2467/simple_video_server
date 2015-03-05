/* ==========================================================================
 * @file    : jpegsave_thread.c
 *
 * @description : This file contains the JPEG save thread.
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <jpeglib.h>
#include <common.h>

extern int g_capture_width;
extern int g_capture_height;
extern int g_jpeg_quality;
extern int g_image_save;
extern unsigned int g_framesize;
extern unsigned char *g_jpeg_frame;
extern int g_algo_enable;
extern int g_imagesavetype;

void jpeg(FILE* dest, unsigned char* rgb, unsigned int width, unsigned int height, int quality)
{
	size_t i, j;
	JSAMPARRAY image;
	image = calloc(height, sizeof (JSAMPROW));
	for (i = 0; i < height; i++) {
		image[i] = calloc(width * 3, sizeof (JSAMPLE));
		for (j = 0; j < width; j++) {
			image[i][j * 3 + 0] = rgb[(i * width + j) * 3 + 0];
			image[i][j * 3 + 1] = rgb[(i * width + j) * 3 + 1];
			image[i][j * 3 + 2] = rgb[(i * width + j) * 3 + 2];
		}
	}
	struct jpeg_compress_struct compress;
	struct jpeg_error_mgr error;
	compress.err = jpeg_std_error(&error);
	jpeg_create_compress(&compress);
	jpeg_stdio_dest(&compress, dest);

	compress.image_width = width;
	compress.image_height = height;
	compress.input_components = 3;
	compress.in_color_space = JCS_RGB;
	jpeg_set_defaults(&compress);
	jpeg_set_quality(&compress, quality, TRUE);
	jpeg_start_compress(&compress, TRUE);
	jpeg_write_scanlines(&compress, image, height);
	jpeg_finish_compress(&compress);
	jpeg_destroy_compress(&compress);

	for (i = 0; i < height; i++) {
		free(image[i]);
	}
	free(image);
}

int minmax(int min, int v, int max)
{
	return (v < min) ? min : (max < v) ? max : v;
}

unsigned char* yuyv2rgb(unsigned char* yuyv, unsigned int width, unsigned int height)
{
	size_t i, j;
	unsigned char* rgb = calloc(width * height * 3, sizeof (unsigned char));
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j += 2) {
			size_t index = i * width + j;
			int y0 = yuyv[index * 2 + 0] << 8;
			int u = yuyv[index * 2 + 1] - 128;
			int y1 = yuyv[index * 2 + 2] << 8;
			int v = yuyv[index * 2 + 3] - 128;
			rgb[index * 3 + 0] = minmax(0, (y0 + 359 * v) >> 8, 255);
			rgb[index * 3 + 1] = minmax(0, (y0 + 88 * v - 183 * u) >> 8, 255);
			rgb[index * 3 + 2] = minmax(0, (y0 + 454 * u) >> 8, 255);
			rgb[index * 3 + 3] = minmax(0, (y1 + 359 * v) >> 8, 255);
			rgb[index * 3 + 4] = minmax(0, (y1 + 88 * v - 183 * u) >> 8, 255);
			rgb[index * 3 + 5] = minmax(0, (y1 + 454 * u) >> 8, 255);
		}
	}
	return rgb;
}

void get_image_filename(char *name, int val)
{
	DATE_TIME current;

	getcurrenttime(&current);

	if(system("mkdir -p records") < 0) {
		printf("Fail to create 'records' directory\n");
	}
	if(system("mkdir -p records/images") < 0) {
		printf("Fail to create 'records/images' directory\n");
	}
	if(val == 0) {
		snprintf(name,50,"records/images/jpeg/IMG_%d%d%d_%d%d%d.jpg", current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		if(system("mkdir -p records/images/jpeg") < 0) {
			printf("Fail to create 'records/images/jpeg' directory\n");
		}
	} else if(val == 1) {
		snprintf(name,50,"records/images/raw/IMG_%d%d%d_%d%d%d.raw", current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		if(system("mkdir -p records/images/raw") < 0) {
			printf("Fail to create 'records/images/raw' directory\n");
		}
	}
}

void *jpegsaveThread()
{
	unsigned char* rgb;
	char outfile[50] = {0};
	FILE *fp;

	while(!KillJpegsaveThread) {
		while(!g_image_save) {
			usleep(10);
		}

		get_image_filename(outfile,g_imagesavetype);
		fp = fopen(outfile, "w");
		switch(g_imagesavetype) {
			case 0:
				apply_algo((char *)g_jpeg_frame,g_algo_enable);
				rgb = yuyv2rgb(g_jpeg_frame, g_capture_width, g_capture_height);
				jpeg(fp, rgb, g_capture_width, g_capture_height, g_jpeg_quality);
				free(rgb);
				free(g_jpeg_frame);
				break;
			case 1:
				fwrite(g_jpeg_frame,g_framesize,1,fp);
				break;
		}
		fclose(fp);
		g_image_save = FALSE;
	}
	return 0;
}
