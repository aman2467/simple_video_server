/* ==========================================================================
 * @file    : algorithms.c
 *
 * @description : This file contains the algo to be applied on captured video
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <common.h>

/****************************************************************************
 * @usage : This function applys algorithms to a frame.
 *
 * @arg1  : pointer to a frame
 * @arg2  : algo type
 * @return     : void
 * *************************************************************************/
void apply_algo(char *frame, int enable)
{
	char *ptr;
	int pix_cnt;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	if(enable == 0) return;

	switch(serverConfig->algo_type) {
		case ALGO_BW:
			ptr = frame;
			pix_cnt = 1;
			while(pix_cnt < serverConfig->capture.framesize) {
				*(ptr+pix_cnt) = (0xfe)>>1;
				pix_cnt+=2;
			}
			break;
		case ALGO_CARTOON:
			ptr = frame;
			pix_cnt = 0;
			while(pix_cnt < serverConfig->capture.framesize) {
				pix_cnt++;
				*(ptr+pix_cnt) &= 0xab;
			}
			break;
		case ALGO_NONE:
			break;
	}
}
