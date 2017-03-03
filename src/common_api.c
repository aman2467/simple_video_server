/*
 * @file    : common_api.c
 *
 * @description : This file common APIs definition used elsewhere.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

#include <osd_thread.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <common.h>

/*
 * @usage : This function fills current system data/time.
 *
 * @arg   : pointer to DATE_TIME structure
 * @return     : void
 */
void getcurrenttime(DATE_TIME *cur)
{
	time_t timep;
	struct tm *p;

	tzset();
	time(&timep);
	p = localtime(&timep);

	cur->hour = p->tm_hour;
	cur->min = p->tm_min;
	cur->sec = p->tm_sec;
	cur->year = 1900 + p->tm_year;
	cur->mon = p->tm_mon + 1;
	cur->day = p->tm_mday;
}

/*
 * @usage : This function enables/disables particular OSD window.
 *
 * @arg1   : OSD window number
 * @arg2   : enable/disable variable
 * @return     : void
 */
void set_osd_window_enable(int window, int enable)
{
	SERVER_CONFIG *serverConfig = GetServerConfig();

	serverConfig->osdwin[window].enable = enable;
}

/*
 * @usage : This function displays given text on particular OSD window.
 *
 * @arg1   : OSD window number
 * @arg2   : OSD text
 * @return     : void
 */
void set_osd_window_text(int window, char *text)
{
	SERVER_CONFIG *serverConfig = GetServerConfig();

	strcpy(serverConfig->osdwin[window].osdtext, text);

	serverConfig->osdwin[window].width = TEXT_WIDTH*strlen(text);
	serverConfig->osdwin[window].height = TEXT_HEIGHT;
	set_osd_window_position(window, serverConfig->osdwin[window].x,
				serverConfig->osdwin[window].y);
	serverConfig->osdwin[window].enable = TRUE;
}

/*
 * @usage : This function displays given text on particular OSD window.
 *
 * @arg1   : OSD window number
 * @arg2   : position x
 * @arg2   : position y
 * @return     : void
 */
void set_osd_window_position(int window, int x, int y)
{
	SERVER_CONFIG *serverConfig = GetServerConfig();

	if (x + serverConfig->osdwin[window].width >
	    serverConfig->capture.width) {
		serverConfig->osdwin[window].x =
			serverConfig->capture.width -
			serverConfig->osdwin[window].width;
	} else {
		serverConfig->osdwin[window].x = x;
	}
	if (y + serverConfig->osdwin[window].height >
	   serverConfig->capture.height) {
		serverConfig->osdwin[window].y =
			serverConfig->capture.height -
			serverConfig->osdwin[window].height;
	} else {
		serverConfig->osdwin[window].y = y;
	}
}

/*
 * @usage : This function enables/disables particular OSD window's transparency.
 *
 * @arg1   : OSD window number
 * @arg2   : enable/disable variable
 * @return     : void
 */
void set_osd_window_transparency(int window, int enable)
{
	SERVER_CONFIG *serverConfig = GetServerConfig();

	if (serverConfig->osdwin[window].enable)
		serverConfig->osdwin[window].transparency = enable;
}

/*
 * @usage : This function updates the parameters of given OSD window number.
 *
 * @arg1   : OSD window number
 * @return     : void
 */
void update_osd_window(int window)
{
	DATE_TIME cur;
	SERVER_CONFIG *serverConfig = GetServerConfig();
	char str[20] = {0};

	if (!serverConfig->osdwin[window].enable)
		return;

	switch (window) {
	case OSD_WINDOW_THREE:/*display tux*/
		serverConfig->osdwin[window].y =
			serverConfig->capture.height -
			serverConfig->osdwin[window].height - 10;
		break;
	case OSD_WINDOW_EIGHT:/*display time*/
		getcurrenttime(&cur);
		if (cur.hour > 12) {
			snprintf(str, OSD_TEXT_MAX_LENGTH,
				 "TIME:%02d:%02d:%02dPM",
				 cur.hour-12, cur.min, cur.sec);
		} else {
			snprintf(str, OSD_TEXT_MAX_LENGTH,
				 "TIME:%02d:%02d:%02dAM",
				 cur.hour, cur.min, cur.sec);
		}
		strcpy(serverConfig->osdwin[window].osdtext, str);
		serverConfig->osdwin[window].x =
			serverConfig->capture.width -
			OSD_TEXT_MAX_LENGTH*TEXT_WIDTH - 10;
		serverConfig->osdwin[window].y =
			serverConfig->capture.height -
			2 * TEXT_HEIGHT - 12;
		serverConfig->osdwin[window].width = TEXT_WIDTH *
			strlen(serverConfig->osdwin[window].osdtext);
		break;
	case OSD_WINDOW_NINE:/*display date*/
		getcurrenttime(&cur);
		snprintf(str, OSD_TEXT_MAX_LENGTH,
			 "DATE:%02d-%02d-%04d", cur.day, cur.mon, cur.year);
		strcpy(serverConfig->osdwin[window].osdtext, str);
		serverConfig->osdwin[window].x =
			serverConfig->capture.width -
			OSD_TEXT_MAX_LENGTH * TEXT_WIDTH - 10;
		serverConfig->osdwin[window].y =
			serverConfig->capture.height -
			TEXT_HEIGHT - 10;
		serverConfig->osdwin[window].width = TEXT_WIDTH *
			strlen(serverConfig->osdwin[window].osdtext);
		break;
	default:
		break;
	}
}
