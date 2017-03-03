/*
 * @file    : command_list.h
 *
 * @description : This file contains command list.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _COMMAND_LIST_H_
#define _COMMAND_LIST_H_

enum command_list {
	COMMAND_SET_TAKE_SNAPSHOT, /* Takes a jpeg/raw snapshot */
	COMMAND_SET_RECORD_VIDEO, /* start/stop video recording */
	COMMAND_SET_VIDEO_TYPE,	/* sets video type as raw/encoded */
	COMMAND_SET_ALGO_TYPE, /* sets algo type */
	COMMAND_SET_OSD_ENABLE,	/* enables given OSD window */
	COMMAND_SET_OSD_TEXT, /* displays given text on given OSD window */
	COMMAND_SET_OSD_POSITION, /* changes position of given OSD window */
	COMMAND_SET_OSD_ON_IMAGE, /* on/off OSD on snapshot */
	COMMAND_SET_OSD_ON_VIDEO, /* on/off OSD on video */
	COMMAND_SET_OSD_TRANSPARENCY, /* on/off OSD window's transparency */
	COMMAND_SET_ENABLE_STREAM, /* on/off network streaming */
	COMMAND_SET_ENABLE_DATE, /* on/off date stamp over video */
	COMMAND_SET_ENABLE_TIME, /* on/off time stamp over video */
};

#endif /* _COMMAND_LIST_H_ */
