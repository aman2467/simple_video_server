/************************************************************************
 * @file   cli_main.c
 * @brief  This file contains main program for cli_app.
 *
 ***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio_ext.h>
#include <cli_app.h>

#define SPACE ' '
#define NUL '\0'
#define ESCAPE 27

#define TRUE 1
#define FALSE 0

char prompt[16] = "CLI Prompt:/>> ";
char g_video_server_ip[16] = {0};

static char  acCmndLine[CMND_LINE_MAX_LEN + 2]; /* Command Line buffer */
static char  *pcCmndLinePtr;	/* Pointer into Cmnd Line buffer */
static UINT16 uwCmndArgCount;	/* Number of Cmnd Line args (incl. Cmnd name) */
static char  *apcCmndArgVal[CLI_MAX_ARGS];	/* Array of pointers to Command Line args */
static const char kszCommandError[] = "command not found\n";

/****************************************************************************
 *   COMMAND TABLE
 *
 *   Application-specific command functions go at top of table...
 *   (optionally in alphabetical order, or some other systematic arrangement)
 *
 ****************************************************************************/
const  struct  CmndTableEntry_t  asCommand[] = {
	/*  Name				Attribute       Address   */
	{"takesnap",			USER_CMD,		CmndTakeSnap},	
	{"recordvideo",			USER_CMD,		CmndRecordVideo},	
	{"help",				USER_CMD,		CmndHelp},
	{"exit",				USER_CMD,		CmndExit},
	{"$",					DEBUG_CMD,      NULL},   /* Dummy last entry */
} ;

/**************************************************************************
 * COMMAND LINE INTERPRETER
 *
 * Processes the received command line when entered.
 *
 * The CommandLineInterpreter() function is called by the main function.
 * When a valid command string is entered it finds & counts
 * "arguments"
 * in the command line and makes them NUL-terminated strings in-situ.
 * It then searches the Command Table for command name, apcCmndArgVal[0],
 * and if the name is found, executes the respective command function.
 *
 * If there is a cmd line argument following the cmd name and it is "-h"
 * (Help option),
 * the argument is converted to "?", which is the (simpler) alternative
 * syntax.
 *
 * A command string is comprised of a command name and one or more
 * "arguments" separated
 * by one or more spaces, or TABs. The ordering of arguments is determined
 * by particular
 * command functions, so may or may not be important. The degree of syntax
 * checking is also
 * the responsibility of command functions.
 *
 ***************************************************************************/
void CommandLineInterpreter(void)
{
	char c;
	UINT8 bArgIndex, bCmndIndex;
	UINT8 yCmndNameFound = FALSE;
	int isInvalid = 0;

	pcCmndLinePtr = acCmndLine;	/* point to start of Cmnd Line buffer */
	uwCmndArgCount = 0;

	for (bArgIndex = 0; bArgIndex < CLI_MAX_ARGS; bArgIndex++) {
		if (!isprint(*pcCmndLinePtr)) {  /* stop at end of line */
			if (isInvalid == 0)
				isInvalid = 2;
			break;
		}

		isInvalid = 1;

		while (*pcCmndLinePtr == SPACE) /* skip leading spaces */
			pcCmndLinePtr++ ;

		if (!isprint(*pcCmndLinePtr)) /* end of line found */
			break;

		apcCmndArgVal[bArgIndex] = pcCmndLinePtr; /* Make ptr to arg */
		uwCmndArgCount++ ;

		while ((c = *pcCmndLinePtr) != SPACE) {  /* find first space after arg */
			if (!isprint(c))      /* end of line found */
				break;
			pcCmndLinePtr++ ;
		}

		if (!isprint(*pcCmndLinePtr)) /* stop at end of line */
			break;

		*pcCmndLinePtr++ = NUL;           /* terminate the arg */

	}

	if (isInvalid != 2) {
		for (bCmndIndex = 0; bCmndIndex < MAX_COMMANDS; bCmndIndex++) {   /* Search cmd table */
			if (*asCommand[bCmndIndex].pzName == '$') /* end of table */
				break;
			if (!strcmp(apcCmndArgVal[0], asCommand[bCmndIndex].pzName)) {
				yCmndNameFound = TRUE;
				break;
			}
		}

		if (uwCmndArgCount > 1) {	   /* There is one or more user-supplied arg(s) */
			if (!strcmp(apcCmndArgVal[1], "-h")) /* help on cmnd wanted... */
				apcCmndArgVal[1] = "?";  /* ... simplify for cmnd func. */
		}

		if (yCmndNameFound)
			(*asCommand[bCmndIndex].Function)(uwCmndArgCount, apcCmndArgVal); /* Do it */
		else
			printf("%s", kszCommandError);
	} else {
		printf("%s", kszCommandError);
	}
}

/**********************************************************************
 * CmndHelp : This function is called when a Command 'help' is
 *			  typed in the CLI. It prints the
 *			  output according to the arguments passed
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndHelp(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	UINT8 yOptionDebug = FALSE;
	UINT8 yOptionAll = FALSE;
	UINT8 yOptionUsage = FALSE;
	char *pzCommandName;
	UINT8 bCmndIndex;

	if (uwCmndArgCount > 1) {
		if (!strcmp(apcCmndArgVal[1], "-a"))
			yOptionAll = TRUE;
		else if (!strcmp(apcCmndArgVal[1], "-d"))
			yOptionDebug = TRUE;
		else
			yOptionUsage = TRUE;
	}

	if (yOptionUsage || uwCmndArgCount == 1) {
		printf("\nUsage:\thelp  { ? | -h | -d | -a }\n");
		printf("Arg's:	 ? : show command usagei (Applicable for all commands)\n");
		printf("\t-h : same as '?'(Applicable for all commands)\n");
		printf("\t-d : list debug commands only\n");
		printf("\t-a : list all commands\n\n");
		return;
	}

	for (bCmndIndex = 0; bCmndIndex < MAX_COMMANDS; bCmndIndex++) {   /* List commands */
		pzCommandName = asCommand[bCmndIndex].pzName;

		if (*pzCommandName == '$')
			break;	/* Reached end of Cmnd Table */

		if (yOptionAll || uwCmndArgCount == 1) {
			printf("%s", pzCommandName);
			printf("\n");
		} else if (yOptionDebug && asCommand[bCmndIndex].yAttribute == DEBUG_CMD) {
			printf("%s", pzCommandName);
			printf("\n");
		}
	}
	return;
}

/**********************************************************************
 * CmndExit :  This function is called when a Command 'exit' is
 *			  typed in the CLI prompt. It exits the CLI interface.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndExit(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	exit(0);
}

/**********************************************************************
 * CmndTakeSnap :  This function is called when a Command 'takesnap' is
 *			  typed in the CLI prompt. It saves a snapshot.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndTakeSnap(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int val;
	if (uwCmndArgCount == 2) {
		val=atoi(apcCmndArgVal[1]);
		if(val == 0 || val == 1) {
			if(savesnap(val) < 0) {
				printf("\nCommand takesnap failed\n");
			}
			return;
		} else {
			printf("\nEnter appropriate argument.\n");
		}
	}

	printf("\nUsage: takesnap <0/1>\n");
	printf("Arg's:\t0 -> JPEG Snapshot\n");
	printf("\t1 -> Raw Snapshot\n");
	return;
}

/**********************************************************************
 * CmndRecordVideo :  This function is called when a Command 'recordvideo' 
 *			  is typed in the CLI prompt. It saves a snapshot.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndRecordVideo(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int val;
	if (uwCmndArgCount == 2) {
		if(strcmp("start",apcCmndArgVal[1]) == 0) {
			val = 1;
		} else if(strcmp("stop",apcCmndArgVal[1]) == 0) {
			val = 0;
		} else {
			printf("\nEnter appropriate argument\n");
			goto usage;
		}
		if(recordvideo(val) < 0) {
			printf("\nCommand recordvideo failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: recordvideo <start/stop>\n");
	printf("Arg's:\tstart -> To start a recording\n");
	printf("\tstop -> To stop a recording\n");
	return;
}


/*******************************************
 * Main function for Command Line Interface
 ******************************************/
int main(int argc, char **argv)
{

	if(argc != 2) {
		printf("\nUsage: %s <video_server_ip>\n",argv[0]);
		exit(0);
	}
	strcpy(g_video_server_ip,argv[1]);

	while (1) {
		printf("%s", prompt);
		scanf("%[^\n]", acCmndLine);

		if (acCmndLine[0] == NUL || acCmndLine[0] == ESCAPE) {
			__fpurge(stdin); /* Clears the stdin buffer */
			continue;
		}

		CommandLineInterpreter();
		acCmndLine[0] = '\0';
		__fpurge(stdin); /* Clears the stdin buffer */
	}
}

