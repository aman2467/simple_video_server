/* ==========================================================================
 * @file    : command_list.h
 *
 * @description : This file contains command list.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#ifndef _COMMAND_LIST_H
#define _COMMAND_LIST_H

enum command_list {
	COMMAND_SET_TAKE_SNAPSHOT,		/* Takes a jpeg/raw snapshot */
	COMMAND_SET_RECORD_VIDEO,		/* start/stop video recording */
	COMMAND_SET_VIDEO_TYPE,			/* sets video type as raw/encoded */ 
	COMMAND_SET_ALGO_TYPE,			/* sets algo type */
	COMMAND_SET_OSD_ENABLE,			/* enables given OSD window */
	COMMAND_SET_OSD_ON_IMAGE,		/* enables/disables OSD on snapshot to be saved */
	COMMAND_SET_OSD_ON_VIDEO,		/* enables/disables OSD on video to be saved */
};

#endif

