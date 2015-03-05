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
#define BPP 2
#define SER_PORT 2467

/***************************************************************************/

#define CAPTURE_THR 0x01
#define OSD_THR 0x02
#define FILERECORD_THR 0x04
#define JPEGSAVE_THR 0x08

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

typedef struct {
	int hour;
	int min;
	int sec;
	int day;
	int mon;
	int year;
} DATE_TIME;

struct record_control {
	int record;
	int osd_on;
};

struct osd_control {
	int win_enable[OSD_MAX_WINDOW];
	int win_x[OSD_MAX_WINDOW];
	int win_y[OSD_MAX_WINDOW];
	int win_w[OSD_MAX_WINDOW];
	int win_h[OSD_MAX_WINDOW];
	char win_text[OSD_MAX_WINDOW/2][25];
	char win_file[OSD_MAX_WINDOW/2][50];
};

struct algo_control {
	int enable;
	int type;
};

struct nw_control {
	char ip[16];
	int port;
};

struct gen_settings {
	int dummy;
};

typedef struct server_config {
	struct record_control image;
	struct record_control video;
	struct osd_control osd;
	struct algo_control algo;
	struct nw_control nw;
	struct gen_settings settings;
} SERVER_CONFIG;

int KillCaptureThread;
int KillOsdThread;
int KillFilerecordThread;
int KillJpegsaveThread;
void apply_algo(char *, int);
void getcurrenttime(DATE_TIME *);

#endif
