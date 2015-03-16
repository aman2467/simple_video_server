/* ==========================================================================
 * @file    : capture_thread.c
 *
 * @description : This file contains the video capture thread.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <common.h>

struct capturebuffer {
	unsigned char *start;
	size_t offset;
	unsigned int length;
};

extern int g_osdflag;
extern int g_writeflag;
extern char *g_framebuff[NUM_BUFFER];
extern int current_task;

/****************************************************************************
 * @function : This is the capture thread main function. It captures video frames
 *          using V4l2 and passes those buffers to other threads.
 *
 * @arg  : void
 * @return     : void
 * *************************************************************************/
void *captureThread(void)
{
	int fd, i;
	unsigned int frame_cnt;
	enum v4l2_buf_type type;
	struct v4l2_capability cap;
	struct v4l2_streamparm parm;
//	struct v4l2_cropcap cropcap;
//	struct v4l2_crop crop;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct capturebuffer buffers[NUM_BUFFER];

	SERVER_CONFIG *serverConfig = GetServerConfig();

	if((fd = open(serverConfig->capture.device, O_RDWR, 0)) < 0) {
		perror("video device open");
		return NULL;
	}

	if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == FAIL) {
		perror("VIDIOC_QUERYCAP");
		return NULL;
	}
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("NO CAPTURE SUPPORT\n");
		return NULL;
	}
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		printf("NO STREAMING SUPPORT\n");
		return NULL;
	}
 
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = 25;
	parm.parm.capture.capturemode = 0;
	if (ioctl(fd, VIDIOC_S_PARM, &parm) < 0) {
		perror("VIDIOC_S_PARM\n");
		return NULL;
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = serverConfig->capture.width;
	fmt.fmt.pix.height = serverConfig->capture.height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == FAIL) {
		perror("VIDIOC_S_FMT");
		return NULL;
	}

	memset(&req, 0, sizeof(req));
	req.count = NUM_BUFFER;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) == FAIL) {
		perror("VIDIOC_REQBUFS");
		return NULL;
	}

	for(i = 0; i < NUM_BUFFER; i++) {
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == FAIL) {
			perror("VIDIOC_QUERYBUF");
			return NULL;
		}

		buffers[i].length = buf.length;
		buffers[i].offset = (size_t) buf.m.offset;
		buffers[i].start = mmap(NULL, buffers[i].length,
									PROT_READ | PROT_WRITE,
									MAP_SHARED, fd, buffers[i].offset);
		memset(buffers[i].start, 0xFF, buffers[i].length);
	}

	for (i = 0; i < NUM_BUFFER; i++) {
		memset(&buf, 0, sizeof (buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.m.offset = buffers[i].offset;
		if (ioctl (fd, VIDIOC_QBUF, &buf) == FAIL) {
			perror("VIDIOC_QBUF");
			return NULL;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl (fd, VIDIOC_STREAMON, &type) == FAIL) {
		perror("VIDIOC_STREAMON");
		return NULL;
	}
	i = 0;
	frame_cnt = 0;
	while(!KillCaptureThread) {
		memset(&buf, 0, sizeof buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if(ioctl(fd, VIDIOC_DQBUF, &buf) == FAIL) {
			perror("VIDIOC_DQBUF");
			return NULL;
		}
		frame_cnt++;
		memcpy(g_framebuff[i],buffers[buf.index].start,serverConfig->capture.framesize);
		if(!serverConfig->enable_osd_thread) {
			if(serverConfig->enable_display_thread) {
				memcpy(serverConfig->disp.display_frame,buffers[buf.index].start,serverConfig->capture.framesize);
				if(serverConfig->algo_type) {
					apply_algo(serverConfig->disp.display_frame,serverConfig->algo_type);
					memcpy(serverConfig->disp.sdl_frame,serverConfig->disp.display_frame,serverConfig->capture.framesize);
				}
			}
		}
		if(serverConfig->enable_imagesave_thread && !serverConfig->image.osd_on) {
			if(serverConfig->image.recordenable) {
				serverConfig->image.recordenable = FALSE;
				serverConfig->jpeg.framebuff = calloc(serverConfig->capture.framesize, 1);
				memcpy(serverConfig->jpeg.framebuff,g_framebuff[i],serverConfig->capture.framesize);
			}
		}
		if(serverConfig->enable_osd_thread) {
			g_osdflag = 1;
		//	current_task = TASK_FOR_OSD;
		} else if(serverConfig->enable_videosave_thread) {
			g_writeflag = 1;
		//	current_task = TASK_FOR_FILERECORD;
		}
		i++;
		if(i > 9) i = 0;
		if(ioctl(fd, VIDIOC_QBUF, &buf) == FAIL) {
			perror("VIDIOC_QBUF");
			return NULL;
		}
		usleep(10);
	}
	return 0;
}
