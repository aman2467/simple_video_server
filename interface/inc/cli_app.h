#ifndef _CLI_APP_H_
#define _CLI_APP_H_

#define CLI_MAX_ARGS        6  /* Maximum number of Command Line arguments (incl. name) */
#define CMND_LINE_MAX_LEN  40  /* Maximum permitted length of command string (chars) */
#define MAX_COMMANDS      250  /* Maximum number of CLI commands (arbitrary) */
#define MAX_ADC_CHN 4
#define VIDEO_SERVER_PORT 2467

#define UINT16 unsigned long int
#define UINT8 unsigned int

#define  USER_CMD   1
#define  DEBUG_CMD   0

#define FAILURE -1
#define SUCCESS 0

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
void CmndHelp(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);
void CmndExit(UINT16 uwCmndArgCount, char *apcCmndArgVal[]);

int sendCommand(int, char *);
int savesnap(int);
int recordvideo(int);

#endif
