/* ==========================================================================
 * @file    : common_api.c
 *
 * @description : This file common APIs definition used elsewhere.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <osd_thread.h>
#include <time.h>
#include <sys/time.h>
#include <common.h>

/****************************************************************************
 * @usage : This function fills current system data/time.
 *
 * @arg   : pointer to DATE_TIME structure
 * @return     : void
 * *************************************************************************/
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

/****************************************************************************
 * @usage : This function enables/disables particular OSD window.
 *
 * @arg1   : OSD window number
 * @arg2   : enable/disable variable
 * @return     : void
 * *************************************************************************/
void set_osd_window_enable(int window, int enable)
{
	SERVER_CONFIG *serverConfig = GetServerConfig();

	serverConfig->osdwin[window].enable = enable;
}

/****************************************************************************
 * @usage : This function updates the parameters of given OSD window number.
 *
 * @arg1   : OSD window number
 * @return     : void
 * *************************************************************************/
void update_osd_window(int window)
{


}

