/* ==========================================================================
 * @file    : common.h
 *
 * @description : This file contains common definitions and declaration for
 *           the application.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#ifndef _COMMON_H
#define _COMMON_H

/*******************************USEFUL TO USER******************************/

#define COLOURED_OP
#define _DEBUG
#define NUM_BUFFER 10
#define OSD_MAX_WINDOW 10
#define OSD_TEXT_MAX_LENGTH 20 
#define BPP 2
#define SER_PORT 2467

/***************************************************************************/

#define CAPTURE_THR 0x01
#define OSD_THR 0x02
#define FILERECORD_THR 0x04
#define JPEGSAVE_THR 0x08
#define DISPLAY_THR 0x10

#define FALSE 0
#define TRUE 1

#define SUCCESS  0
#define FAIL	-1

#ifdef _DEBUG
#define pr_dbg printf
#else
#define pr_dbg //
#endif

#ifdef COLOURED_OP
#define RED "\x1b[01;31m"
#define GREEN "\x1b[01;32m"
#define YELLOW "\x1b[01;33m"
#define BLUE "\x1b[01;34m"
#define MAGENTA "\x1b[01;35m"
#define CYAN "\x1b[01;36m"
#define NONE "\x1b[00m"
#else
#define RED " "
#define GREEN " "
#define YELLOW " "
#define BLUE " "
#define MAGENTA " "
#define CYAN " "
#define NONE " "
#endif

enum algotype {
	ALGO_NONE,
	ALGO_BW,
	ALGO_CARTOON,
};

enum recordtype {
	TYPE_NONE,
	TYPE_RAW,
	TYPE_ENCODED,
};

typedef struct {
	int hour;
	int min;
	int sec;
	int day;
	int mon;
	int year;
} DATE_TIME;

struct record_settings {
	int recordenable;
	int osd_on;
	int type;
};

struct osdwindow {
	int enable;
	int x;
	int y;
	int width;
	int height;
	int transparency;
	char osdtext[OSD_TEXT_MAX_LENGTH];
	char file[100];
};

struct nw_settings {
	char ip[16];
	int port;
};

struct jpeg_parm {
	int quality;
	unsigned char *framebuff;
};

struct gen_settings {
	int dummy;
};

struct capture_settings {
	char device[30];
	int height;
	int width;
	int framesize;
};

struct display_buff {
	char *display_frame;
	char *sdl_frame;
};

typedef struct server_config {
	int enable_osd_thread;
	int enable_imagesave_thread;
	int enable_videosave_thread;
	int enable_network_thread;
	int enable_display_thread;
	int algo_type;
	struct jpeg_parm jpeg;
	struct capture_settings capture;
	struct nw_settings nw;
	struct record_settings video;
	struct record_settings image;
	struct osdwindow osdwin[OSD_MAX_WINDOW];
	struct gen_settings settings;
	struct display_buff disp;
} SERVER_CONFIG;

int KillCaptureThread;
int KillOsdThread;
int KillFilerecordThread;
int KillJpegsaveThread;
int KillDisplayThread;

void apply_algo(char *, int);
void getcurrenttime(DATE_TIME *);
void set_osd_window_enable(int, int);
void set_osd_window_text(int, char *);
void set_osd_window_position(int, int, int);
void set_osd_window_transparency(int, int);
void update_osd_window(int);
SERVER_CONFIG *GetServerConfig(void);

#endif
