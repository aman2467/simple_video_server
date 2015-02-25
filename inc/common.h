/* ==========================================================================
 * @file    : common.h
 *
 * @description : This file contains common definitions and declaration for
 *           the application.
 *
 * @author  : Aman Kumar (2014-2015)
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

int KillCaptureThread;
int KillOsdThread;
int KillFilerecordThread;
int KillJpegsaveThread;
void apply_algo(char *, int);
#endif
