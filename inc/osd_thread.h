/* ==========================================================================
 * @file    : osd_thread.h
 *
 * @description : This file contains common definitions and declaration for
 *           osd thread.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#ifndef _OSD_THREAD_H
#define _OSD_THREAD_H

#define OSD_WINDOW_ZERO 0
#define OSD_WINDOW_ONE 1
#define OSD_WINDOW_TWO 2
#define OSD_WINDOW_THREE 3
#define OSD_WINDOW_FOUR 4
#define OSD_WINDOW_FIVE 5
#define OSD_WINDOW_SIX 6
#define OSD_WINDOW_SEVEN 7
#define OSD_WINDOW_EIGHT 8
#define OSD_WINDOW_NINE 9

#define OSD_DATABASE "PATH"
#define OSD_TEXT_MAX_LENGTH 20
#define TEXT_WIDTH 10
#define TEXT_HEIGHT 16
#define STRING_WIDTH 770

/* Icon Windows */
#define OSD_WINDOW_ZERO_X 10
#define OSD_WINDOW_ZERO_Y 10
#define OSD_WINDOW_ZERO_W 60
#define OSD_WINDOW_ZERO_H 40

#define OSD_WINDOW_ONE_X 680
#define OSD_WINDOW_ONE_Y 10
#define OSD_WINDOW_ONE_W 60
#define OSD_WINDOW_ONE_H 40

#define OSD_WINDOW_TWO_X 10
#define OSD_WINDOW_TWO_Y 366
#define OSD_WINDOW_TWO_W 60
#define OSD_WINDOW_TWO_H 40

#define OSD_WINDOW_THREE_X 680
#define OSD_WINDOW_THREE_Y 366
#define OSD_WINDOW_THREE_W 60
#define OSD_WINDOW_THREE_H 40

#define OSD_WINDOW_FOUR_X 80
#define OSD_WINDOW_FOUR_Y 66
#define OSD_WINDOW_FOUR_W 60
#define OSD_WINDOW_FOUR_H 40

/* Text Windows */
#define OSD_WINDOW_FIVE_X 0
#define OSD_WINDOW_FIVE_Y 0
#define OSD_WINDOW_FIVE_STR " "
#define OSD_WINDOW_FIVE_STR_LEN 1
#define OSD_WINDOW_FIVE_W TEXT_WIDTH*OSD_WINDOW_FIVE_STR_LEN
#define OSD_WINDOW_FIVE_H TEXT_HEIGHT

#define OSD_WINDOW_SIX_X 0
#define OSD_WINDOW_SIX_Y 0
#define OSD_WINDOW_SIX_STR " "
#define OSD_WINDOW_SIX_STR_LEN 1
#define OSD_WINDOW_SIX_W TEXT_WIDTH*OSD_WINDOW_SIX_STR_LEN
#define OSD_WINDOW_SIX_H TEXT_HEIGHT

#define OSD_WINDOW_SEVEN_X 0
#define OSD_WINDOW_SEVEN_Y 0
#define OSD_WINDOW_SEVEN_STR " "
#define OSD_WINDOW_SEVEN_STR_LEN 1
#define OSD_WINDOW_SEVEN_W TEXT_WIDTH*OSD_WINDOW_SEVEN_STR_LEN
#define OSD_WINDOW_SEVEN_H TEXT_HEIGHT

#define OSD_WINDOW_EIGHT_X 0
#define OSD_WINDOW_EIGHT_Y 0
#define OSD_WINDOW_EIGHT_STR " "
#define OSD_WINDOW_EIGHT_STR_LEN 1
#define OSD_WINDOW_EIGHT_W TEXT_WIDTH*OSD_WINDOW_EIGHT_STR_LEN
#define OSD_WINDOW_EIGHT_H TEXT_HEIGHT

#define OSD_WINDOW_NINE_X 0
#define OSD_WINDOW_NINE_Y 0
#define OSD_WINDOW_NINE_STR " "
#define OSD_WINDOW_NINE_STR_LEN 1
#define OSD_WINDOW_NINE_W TEXT_WIDTH*OSD_WINDOW_NINE_STR_LEN
#define OSD_WINDOW_NINE_H TEXT_HEIGHT

#define ASCII_STRING "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789:; '/\\+-_()&$#,"

struct osdwindow {
	int enable;
	int x;
	int y;
	int width;
	int height;
	int transparency;
	char osdtext[OSD_TEXT_MAX_LENGTH];
	char file[80];
};

int osd_init(char *);
void fill_osd_data(char *,char *, int, int);
void get_osd_string(char *, char *);
void update_osd_window(int);
#endif
