/* ==========================================================================
 * @file    : cli_app.h
 *
 * @description : This file contains common definitions and declaration for
 *           the CLI application.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#ifndef _CLI_APP_H_
#define _CLI_APP_H_

#define CLI_MAX_ARGS        6  /* Maximum number of Command Line arguments (incl. name) */
#define CMND_LINE_MAX_LEN  40  /* Maximum permitted length of command string (chars) */
#define MAX_COMMANDS      250  /* Maximum number of CLI commands (arbitrary) */
#define VIDEO_SERVER_PORT 2467

#define UINT16 unsigned long int
#define UINT8 unsigned int

#define  USER_CMD   1
#define  DEBUG_CMD   0

#define FAILURE -1
#define SUCCESS 0

#define TRUE 1
#define FALSE 0

#define WAIT_TIME 3

typedef  void (*CLIfunc)(UINT16 argc, char *argv[]);      /* pointer to CLI function */

/* Command table entry looks like this */
struct  CmndTableEntry_t {
	char   *pzName;       /* command function name */
	UINT8     yAttribute;  /* User Command or Debug Command */
	CLIfunc  Function;     /* pointer to CLI function */
};

/*****************************************************************
 *   Prototypes of functions ..
 *****************************************************************/
void CommandLineInterpreter(void);

void CmndTakeSnap(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndRecordVideo(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndSetAlgoType(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndOsdWinEnable(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndOsdWinText(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndOsdWinPos(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndOsdOnImage(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndOsdOnVideo(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndOsdWinTrans(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndNWStream(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndList(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndDate(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndTime(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndHelp(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndClearScreen(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndExit(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);

int sendCommand(int, char *);
int takesnap(int);
int recordvideo(int);
int setalgotype(int);
int osdwinenable(int, int);
int osdwintext(int, char *);
int osdwinpos(int, int, int);
int osdonimage(int);
int osdonvideo(int);
int osdwintrans(int, int);
int nw_stream(int);
int enable_date(int);
int enable_time(int);
int valid_digit(char *);
int is_valid_ip(char *);
void watchdog(int);

#endif
