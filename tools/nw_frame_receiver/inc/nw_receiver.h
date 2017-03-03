/*
 * @file    : nw_receiver.h
 *
 * @description : This file contains common definitions and declaration for the
 *			 application.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General Public License.
 *              You may obtain a copy of the GNU General Public License Version 2 or later
 *              at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _NW_RECEIVER_H
#define _NW_RECEIVER_H

#define BPP			2
#define FIRST_LINE  1
#define VALID_DATA   8
#define LINE_CNT 1
#define VIDEO_PORT 5005
#define LINE_NUM_SIZE 4
#define FRAME_NUM_SIZE 4

#define TRUE 1
#define FALSE 0


#define DISP_THR 0x01

#define _DEBUG
#ifdef _DEBUG
#define pr_dbg printf
#else
#define pr_dbg //
#endif

#define LOCAL_DISPLAY
#ifndef LOCAL_DISPLAY
#define NO_OF_FRAMES_TO_SAVE 100
#endif

int KillDisplayThread;

typedef struct video_data {
	char *packetbuff;
	unsigned int line_num;
	unsigned int frame_num;
	struct video_data *next;
} VIDEO_DATA;

typedef struct lock {
	volatile int lock;
} lock_t;

void lock_init(lock_t *);
void lock(lock_t *);
void unlock(lock_t *);
void spin(void);

void addpacket(char *);
void poppacket(VIDEO_DATA *);
int sizeofqueue(void);

#endif
