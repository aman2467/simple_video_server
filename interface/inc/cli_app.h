/*
 * @file    : cli_app.h
 *
 * @description : This file contains common definitions and declaration for
 *           the CLI application.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _CLI_APP_H_
#define _CLI_APP_H_

/* Maximum number of Command Line arguments (incl. name) */
#define CLI_MAX_ARGS        6

/* Maximum permitted length of command string (chars) */
#define CMND_LINE_MAX_LEN  40

/* Maximum number of CLI commands (arbitrary) */
#define MAX_COMMANDS      250

#define VIDEO_SERVER_PORT 2467
#define USER_PROMPT
//#define DISPLAY_COMMANDS

#define UINT16 unsigned long int
#define UINT8 unsigned int

#define  USER_CMD   1
#define  DEBUG_CMD   0

#define FAILURE -1
#define SUCCESS 0

#define TRUE 1
#define FALSE 0

#define WAIT_TIME 3

/* pointer to CLI function */
typedef void (*CLIfunc)(UINT16 argc, char *argv[]);

/* Command table entry looks like this */
struct  CmndTableEntry_t {
	char   *pzName;      /* command function name */
	UINT8   yAttribute;  /* User Command or Debug Command */
	CLIfunc Function;    /* pointer to CLI function */
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

int sendCommand(int command, char *args);
int takesnap(int value);
int recordvideo(int value);
int setalgotype(int algo);
int osdwinenable(int window, int enable);
int osdwintext(int window, char *data);
int osdwinpos(int window, int x, int y);
int osdonimage(int enable);
int osdonvideo(int enable);
int osdwintrans(int window, int enable);
int nw_stream(int enable);
int enable_date(int enable);
int enable_time(int enable);
int valid_digit(char *digit);
int is_valid_ip(char *ip);
void watchdog(int signal);

#endif /* _CLI_APP_H_ */
