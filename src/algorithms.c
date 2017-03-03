/* ==========================================================================
 * @file    : algorithms.c
 *
 * @description : This file contains the algo to be applied on captured video
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
#include <stdlib.h>
#include <string.h>
#include <common.h>

void test(char *frame, int size)
{
	char *ptr = frame;
	int pix_cnt = 0;

	while (pix_cnt < size) {
		*(ptr + pix_cnt + 0) &= 0x00; /* Y-Data */
		*(ptr + pix_cnt + 1) &= 0x00; /* U-Data */
		*(ptr + pix_cnt + 2) &= 0x00; /* Y-Data */
		*(ptr + pix_cnt + 3) = 0xFF; /* V-Data */
		pix_cnt += 4;
	}
}

/****************************************************************************
 * @usage : This function mirrors the frame horizontaly
 *
 * @arg1  : pointer to the video data
 * @arg2  : size of video data
 * @return     : void
 * *************************************************************************/
void horz_mirror(char *frame, int size)
{
	char *ptr = NULL;
	char *ptr1 = NULL;
	char *line = NULL;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	ptr1 = frame+size/2;
	ptr = ptr1 - serverConfig->capture.width*BPP;
	line = calloc(1, serverConfig->capture.width*BPP);
	while (ptr1 < frame + size) {
		memcpy(ptr1, ptr, serverConfig->capture.width * BPP);
		ptr1 += serverConfig->capture.width * BPP;
		ptr -= serverConfig->capture.width * BPP;
	}
	free(line);
}

/****************************************************************************
 * @usage : This function swaps the chroma component of input video data/frame
 *
 * @arg1  : pointer to the video data
 * @arg2  : size of video data
 * @return     : void
 * *************************************************************************/
void swap_uv(char *data, int size)
{
	char *ptr = NULL;
	unsigned int pix_cnt;
	short int pixel;
	char chy, chuv;

	ptr = data;
	pix_cnt = 0;
	while (pix_cnt < size) {
		chy = *(ptr+pix_cnt);
		pix_cnt++;
		chuv = *(ptr+pix_cnt);
		pixel = 0x0000;
		pixel = chy;
		pixel = (pixel << 8);
		pixel |= chuv;
		*(ptr + pix_cnt-2) = pixel;
	}
}

/****************************************************************************
 * @usage : This function generates thermal equivalent of input video data/frame
 *
 * @arg1  : pointer to the video data
 * @arg2  : size of video data
 * @return     : void
 * *************************************************************************/
void convert_to_thermal(char *data, int size)
{
	memcpy(data, data + 1, size - 1);
}

/****************************************************************************
 * @usage : This function mirrors the frame vertically
 *
 * @arg1  : pointer to the video data
 * @arg2  : size of video data
 * @arg3  : width of video data
 * @arg4  : height of video data
 * @return     : void
 * *************************************************************************/
void vert_mirror(char *frame, int size, int width, int height)
{
	char *ptr = NULL;
	char *line = NULL;
	unsigned int pix_cnt = 0;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	ptr = frame;
	pix_cnt = 0;
	while (ptr < frame + size) {
		line = ptr + width * BPP;
		pix_cnt = 0;
		while (pix_cnt < width) {
			memcpy(line - pix_cnt, ptr + pix_cnt, 1);
			memcpy(line - pix_cnt - 1, ptr + pix_cnt + 1, 1);
			pix_cnt += 2;
		}
		ptr += serverConfig->capture.width * BPP;
	}
	ptr = frame + width;
	while (ptr < frame + size) {
		swap_uv(ptr, width);
		ptr += serverConfig->capture.width * BPP;
	}
}

/****************************************************************************
 * @usage : This function applys algorithms to a frame.
 *
 * @arg1  : pointer to a frame
 * @arg2  : algo type
 * @return     : void
 * *************************************************************************/
void apply_algo(char *frame, int enable)
{
	char *ptr = NULL;
	unsigned int pix_cnt = 0;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	if (enable == 0)
		return;

	switch (serverConfig->algo_type) {
	case ALGO_BW:
		ptr = frame;
		pix_cnt = 1;
		while (pix_cnt < serverConfig->capture.framesize) {
			*(ptr + pix_cnt) = (0xfe) >> 1;
			pix_cnt += 2;
		}
		break;
	case ALGO_CARTOON:
		ptr = frame;
		pix_cnt = 0;
		while (pix_cnt < serverConfig->capture.framesize) {
			pix_cnt++;
			*(ptr + pix_cnt) &= 0xD0;
		}
		break;
	case ALGO_STAMP:
		ptr = frame;
		pix_cnt = 0;
		while (pix_cnt < serverConfig->capture.framesize) {
			pix_cnt++;
			*(ptr + pix_cnt) &= 0xA0;
		}
		pix_cnt = 1;
		ptr = frame;
		while (pix_cnt < serverConfig->capture.framesize) {
			*(ptr + pix_cnt) = (0xfe) >> 1;
			pix_cnt += 2;
		}
		break;
	case ALGO_UVSWAP:
		swap_uv(frame, serverConfig->capture.framesize);
		break;
	case ALGO_DARK_NEON:
		ptr = frame;
		pix_cnt = 1;
		while (pix_cnt < serverConfig->capture.framesize) {
			*(ptr + pix_cnt + 1) = (0x7D >> 1);
			pix_cnt += 2;
		}
		break;
	case ALGO_THERMAL:
		convert_to_thermal(frame, serverConfig->capture.framesize);
		break;
	case ALGO_TEST:
		test(frame, serverConfig->capture.framesize);
		break;
	case ALGO_H_MIRROR:
		horz_mirror(frame, serverConfig->capture.framesize);
		break;
	case ALGO_V_MIRROR:
		vert_mirror(frame, serverConfig->capture.framesize,
			    serverConfig->capture.width,
			    serverConfig->capture.height);
		break;
	case ALGO_MULTI_1:
		horz_mirror(frame, serverConfig->capture.framesize);
		vert_mirror(frame, serverConfig->capture.framesize,
			    serverConfig->capture.width,
			    serverConfig->capture.height);
		break;
	case ALGO_MULTI_2:
		horz_mirror(frame, serverConfig->capture.framesize/2);
		vert_mirror(frame, serverConfig->capture.framesize/2,
			    serverConfig->capture.width/2,
			    serverConfig->capture.height);
		vert_mirror(frame, serverConfig->capture.framesize,
			    serverConfig->capture.width,
			    serverConfig->capture.height);
		horz_mirror(frame, serverConfig->capture.framesize);
		break;
	case ALGO_MULTI_3:
		horz_mirror(frame, serverConfig->capture.framesize/4);
		horz_mirror(frame, serverConfig->capture.framesize/2);
		vert_mirror(frame, serverConfig->capture.framesize/2,
			    serverConfig->capture.width/4,
			    serverConfig->capture.height);
		vert_mirror(frame, serverConfig->capture.framesize/2,
			    serverConfig->capture.width/2,
			    serverConfig->capture.height);
		vert_mirror(frame, serverConfig->capture.framesize,
			    serverConfig->capture.width,
			    serverConfig->capture.height);
		horz_mirror(frame, serverConfig->capture.framesize);
		break;
	case ALGO_NONE:
	default:
		break;
	}
}
