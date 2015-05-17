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
#include <pwd.h>

/****************************************************************************
 * @func    : saves a jpeg frame
 * @arg1    : File descriptor
 * @arg2    : pointer to rgb data holder
 * @arg3    : width of image
 * @arg4    : height of image
 * @arg5    : jpeg quality value
 * @return  : void
 ***************************************************************************/
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

/****************************************************************************
 * @func    : returns value after checking a range for it
 * @arg1    : minimum allowed range
 * @arg2    : maximum allowed range
 * @return  : returns checked value
 ***************************************************************************/
int minmax(int min, int v, int max)
{
	return (v < min) ? min : (max < v) ? max : v;
}

/****************************************************************************
 * @func    : creates rgb equivalent of yuv data
 * @arg1    : pointer to yuv data
 * @arg2    : width of image
 * @arg3    : height of image
 * @return  : returns rgb data pointer
 ***************************************************************************/
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

/****************************************************************************
 * @usage : This function creates a file name based on timestamp.
 *
 * @arg1  : pointer to file name
 * @arg2  : image type
 * @return     : void
 * *************************************************************************/
void get_image_filename(char *name, int val)
{
	DATE_TIME current;
	char path[50] = {0};
	char cmd[100] = {0};
	struct passwd *pwd;

	getcurrenttime(&current);
	pwd = getpwuid(getuid());

	if(pwd) {
		strcpy(path,pwd->pw_dir);	
	}
	snprintf(cmd,100,"mkdir -p %s/Pictures/svs",path);
	if(system(cmd) < 0) {
		printf("Fail to create 'svs' directory\n");
	}
	if(val == 0) {
		snprintf(name,100,"%s/Pictures/svs/jpeg/IMG_%d%d%d_%d%d%d.jpg", path, current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		strcat(cmd,"/jpeg");
		if(system(cmd) < 0) {
			printf("Fail to create 'records/images/jpeg' directory\n");
		}
	} else if(val == 1) {
		snprintf(name,100,"%s/Pictures/svs/raw/IMG_%d%d%d_%d%d%d.raw", path, current.year, current.mon,
			current.day, current.hour, current.min, current.sec);
		strcat(cmd,"/raw");
		if(system(cmd) < 0) {
			printf("Fail to create 'Pictures/svs/raw' directory\n");
		}
	}
}

/****************************************************************************
 * @func    : JPEG save Thread main funtion
 * @arg1    : void
 * @return  : void
 ***************************************************************************/
void *jpegsaveThread(void)
{
	unsigned char* rgb;
	char outfile[100] = {0};
	FILE *fp;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	while(!KillJpegsaveThread) {
		while(serverConfig->jpeg.framebuff == NULL) {
			usleep(100);
		}

		get_image_filename(outfile,serverConfig->image.type);
		fp = fopen(outfile, "w");
		switch(serverConfig->image.type) {
			case 0:
				apply_algo((char *)serverConfig->jpeg.framebuff,serverConfig->algo_type);
				rgb = yuyv2rgb(serverConfig->jpeg.framebuff, serverConfig->capture.width, serverConfig->capture.height);
				jpeg(fp, rgb, serverConfig->capture.width, serverConfig->capture.height, serverConfig->jpeg.quality);
				free(rgb);
				break;
			case 1:
				fwrite(serverConfig->jpeg.framebuff,serverConfig->capture.framesize,1,fp);
				break;
		}
		free(serverConfig->jpeg.framebuff);
		serverConfig->jpeg.framebuff = NULL;
		fclose(fp);
	}
	return 0;
}
