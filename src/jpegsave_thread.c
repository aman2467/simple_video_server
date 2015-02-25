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
#include <time.h>
#include <sys/time.h>
#include <jpeglib.h>
#include <common.h>

typedef struct {
	int hour;
	int min;
	int sec;
	int day;
	int mon;
	int year;
} DATE_TIME;

extern int g_capture_width;
extern int g_capture_height;
extern int g_jpeg_quality;
extern int g_jpeg_save;
extern unsigned int g_framesize;
extern unsigned char *g_jpeg_frame;
extern int g_algo_enable;

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

void getcurrenttime(DATE_TIME *cur)
{
	time_t timep;
	struct tm *p;

	tzset();
	time(&timep);
	p=localtime(&timep);

	cur->hour = p->tm_hour;
	cur->min = p->tm_min;
	cur->sec = p->tm_sec;
	cur->year = 1900+p->tm_year;
	cur->mon = p->tm_mon+1;
	cur->day = p->tm_mday;
}

void get_filename(char *filename)
{
	DATE_TIME current;

	getcurrenttime(&current);
	snprintf(filename,50,"records/images/jpeg/IMG_%d%d%d_%d%d%d.jpg", current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
}

void *jpegsaveThread()
{
	unsigned char* rgb;
	char outfile[50] = {0};
	FILE *fp;

	while(!KillJpegsaveThread) {
		while(!g_jpeg_save) {
			usleep(10);
		}

		get_filename(outfile);
		fp = fopen(outfile, "w");
		apply_algo((char *)g_jpeg_frame,g_algo_enable);
		rgb = yuyv2rgb(g_jpeg_frame, g_capture_width, g_capture_height);
		jpeg(fp, rgb, g_capture_width, g_capture_height, g_jpeg_quality);
		free(rgb);
		free(g_jpeg_frame);
		fclose(fp);
		g_jpeg_save = FALSE;
	}
	return 0;
}
