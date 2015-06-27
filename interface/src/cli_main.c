/************************************************************************
 * @file   cli_main.c
 * @brief  This file contains main program for cli_app.
 *
 ***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <stdio_ext.h>
#include <cli_app.h>

#define SPACE ' '
#define NUL '\0'
#define ESCAPE 27

#define TRUE 1
#define FALSE 0

char prompt[16] = "CLI:/> ";
char g_video_server_ip[16] = "127.0.0.1";
int quit_flag = FALSE;

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
	{"setalgotype",			USER_CMD,		CmndSetAlgoType},	
	{"osdwinenable",		USER_CMD,		CmndOsdWinEnable},	
	{"osdwintext",			USER_CMD,		CmndOsdWinText},	
	{"osdwinpos",			USER_CMD,		CmndOsdWinPos},	
	{"osdonimage",			USER_CMD,		CmndOsdOnImage},	
	{"osdonvideo",			USER_CMD,		CmndOsdOnVideo},	
	{"osdwintrans",			USER_CMD,		CmndOsdWinTrans},	
	{"stream",				USER_CMD,		CmndNWStream},	
	{"ls",					USER_CMD,		CmndList},
	{"help",				USER_CMD,		CmndHelp},
	{"clear",				USER_CMD,		CmndClearScreen},
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
 * CmndList :  This function is called when a Command 'ls' is
 *			  typed in the CLI prompt. It lists all commands.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndList(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	char *command;
	int i;

	if(uwCmndArgCount == 1)	{
		for (i = 0; i < MAX_COMMANDS; i++) {   /* List commands */
			command = asCommand[i].pzName;

			if (*command == '$')
				break;  /* Reached end of Cmnd Table */

			printf("%s\n", command);
		}
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
 * CmndClearScreen :  This function is called when a Command 'clear' is
 *			  typed in the CLI prompt. It clears the screen.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndClearScreen(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	system("clear");
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
	int type;
	if (uwCmndArgCount == 2) {
		type = atoi(apcCmndArgVal[1]);
		if(strcmp("jpeg",apcCmndArgVal[1]) == 0) {
			type = 0;
		} else if(strcmp("raw",apcCmndArgVal[1]) == 0) {
			type = 1;
		} else {
			printf("\nEnter appropriate argument\n");
			goto usage;
		}
		if(takesnap(type) < 0) {
			printf("\nCommand takesnap failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: takesnap <jpeg|raw>\n");
	printf("Arg's:\tjpeg -> JPEG Snapshot\n");
	printf("\traw -> Raw Snapshot\n");
	return;
}

/**********************************************************************
 * CmndRecordVideo :  This function is called when a Command 'recordvideo' 
 *			  is typed in the CLI prompt. It saves a video.
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
	printf("\nUsage: recordvideo <start|stop>\n");
	printf("Arg's:\tstart -> To start a recording\n");
	printf("\tstop -> To stop a recording\n");
	return;
}

/**********************************************************************
 * CmndSetAlgoType :  This function is called when a Command 'setalgotype' 
 *			  is typed in the CLI prompt. It sets algo type.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndSetAlgoType(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int type;
	if (uwCmndArgCount == 2) {
		if(strcmp("none",apcCmndArgVal[1]) == 0) {
			type = 0;
		} else if(strcmp("bw",apcCmndArgVal[1]) == 0) {
			type = 1;
		} else if(strcmp("cartoon",apcCmndArgVal[1]) == 0) {
			type = 2;
		} else {
			printf("\nEnter appropriate argument\n");
			goto usage;
		}
		if(setalgotype(type) < 0) {
			printf("\nCommand setalgotype failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: setalgotype <none|bw|cartoon>\n");
	printf("Arg's:\tnone -> To get normal video/image\n");
	printf("\tbw -> To get black&white video/image\n");
	printf("\tcartoon -> To get cartoonised video/image\n");
	return;
}

/**********************************************************************
 * CmndOsdWinEnable :  This function is called when a Command 'osdwinenable' 
 *			  is typed in the CLI prompt. It enables/disables osd windows.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdWinEnable(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win,enable;
	if (uwCmndArgCount == 3) {
		win = atoi(apcCmndArgVal[1]);
		if(win < 0 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		if(strcmp("enable",apcCmndArgVal[2]) == 0) {
			enable = 1;
		} else if(strcmp("disable",apcCmndArgVal[2]) == 0) {
			enable = 0;
		}
		if(osdwinenable(win,enable) < 0) {
			printf("\nCommand osdwinenable failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: osdwinenable <OSD window no.> <enable|disable>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 0 to 9\n");
	printf("\t2. enable -> To enable given OSD Window\n");
	printf("\t   disble -> To disable given OSD Window\n");
	return;
}

/**********************************************************************
 * CmndOsdWinText :  This function is called when a Command 'osdwintext' 
 *			  is typed in the CLI prompt. It adds/changes OSD text.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdWinText(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win;
	char text[25] = {0};
	if (uwCmndArgCount >= 3) {
		win = atoi(apcCmndArgVal[1]);
		if(win < 5 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		if(uwCmndArgCount == 3) {
			if(strlen(apcCmndArgVal[2]) > 20) {
				printf("Maximum allowed text size is 20.");
				return;
			}
			if(osdwintext(win,apcCmndArgVal[2]) < 0) {
				printf("\nCommand osdwintext failed\n");
			}
			return;
		} else if (uwCmndArgCount == 4) {
			if(strlen(apcCmndArgVal[2])+strlen(apcCmndArgVal[3]) > 19) {
				printf("Maximum allowed text size is 20.");
				return;
			}
			strcpy(text,apcCmndArgVal[2]);
			text[strlen(apcCmndArgVal[2])] = ' ';
			strcat(text,apcCmndArgVal[3]);
			if(osdwintext(win,text) < 0) {
				printf("\nCommand osdwintext failed\n");
			}
			return;
		} else {
			printf("\nOnly one space is allowed\n");
		}
	}

usage:
	printf("\nUsage: osdwintext <OSD window no.> <text>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 5 to 9\n");
	printf("\t2. text -> text to be displayed over video\n");
	return;
}

/**********************************************************************
 * CmndOsdWinPos :  This function is called when a Command 'osdwinpos' 
 *			  is typed in the CLI prompt. It changes posiiton of OSD window.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdWinPos(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win, x, y;
	if (uwCmndArgCount == 4) {
		win = atoi(apcCmndArgVal[1]);
		if(win < 0 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		x = atoi(apcCmndArgVal[2]);
		if(x < 0 || x > 1280) {
			printf("\nInvalid Position\n");
			goto usage;
		}
		y = atoi(apcCmndArgVal[3]);
		if(y < 0 || y > 720) {
			printf("\nInvalid Position\n");
			goto usage;
		}
		if(osdwinpos(win, x, y) < 0) {
			printf("\nCommand osdwinpos failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: osdwinpos <OSD window no.> <X-pos> <Y-pos>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 4 to 9\n");
	printf("\t2. x -> x co-ordinate for the given OSD window\n");
	printf("\t3. y -> y co-ordinate for the given OSD window\n");
	return;
}

/**********************************************************************
 * CmndOsdOnImage :  This function is called when a Command 'osdonimage' 
 *			  is typed in the CLI prompt. It enables/disables OSD on saved snapshot.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdOnImage(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;
	if (uwCmndArgCount == 2) {
		if(strcmp("enable",apcCmndArgVal[1]) == 0) {
			enable = 1;
		} else if(strcmp("disable",apcCmndArgVal[1]) == 0) {
			enable = 0;
		}
		if(osdonimage(enable) < 0) {
			printf("\nCommand osdonimage failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: osdonimage <enable|disable>\n");
	printf("Arg's:\tenable -> To enable OSD on recorded snapshot\n");
	printf("\tdisble -> To disable OSD on recorded snapshot\n");
	return;
}

/**********************************************************************
 * CmndOsdOnVideo :  This function is called when a Command 'osdonvideo'
 *            is typed in the CLI prompt. It enables/disables OSD on saved video.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdOnVideo(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;
		if (uwCmndArgCount == 2) {
			if(strcmp("enable",apcCmndArgVal[1]) == 0) {
				enable = 1;
			} else if(strcmp("disable",apcCmndArgVal[1]) == 0) {
				enable = 0;
			}
			if(osdonvideo(enable) < 0) {
				printf("\nCommand osdonvideo failed\n");
			}
			return;
		}

usage:
	printf("\nUsage: osdonvideo <enable|disable>\n");
	printf("Arg's:\tenable -> To enable OSD on recorded video\n");
	printf("\tdisble -> To disable OSD on recorded video\n");
	return;
}

/**********************************************************************
 * CmndOsdWinTrans :  This function is called when a Command 'osdwintrans' 
 *			  is typed in the CLI prompt. It enables/disables osd transparency.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdWinTrans(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win,enable;
	if (uwCmndArgCount == 3) {
		win = atoi(apcCmndArgVal[1]);
		if(win < 0 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		if(strcmp("enable",apcCmndArgVal[2]) == 0) {
			enable = 1;
		} else if(strcmp("disable",apcCmndArgVal[2]) == 0) {
			enable = 0;
		}
		if(osdwintrans(win,enable) < 0) {
			printf("\nCommand osdwintrans failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: osdwintrans <OSD window no.> <enable|disable>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 0 to 9\n");
	printf("\t2. enable -> To enable given OSD Window\n");
	printf("\t   disble -> To disable given OSD Window\n");
	return;
}

/**********************************************************************
 * CmndNWStream :  This function is called when a Command 'stream' 
 *			  is typed in the CLI prompt. It enables/disables network stream.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndNWStream(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;
	if (uwCmndArgCount == 2) {
		if(strcmp("enable",apcCmndArgVal[1]) == 0) {
			enable = 1;
		} else if(strcmp("disable",apcCmndArgVal[1]) == 0) {
			enable = 0;
		}
		if(nw_stream(enable) < 0) {
			printf("\nCommand stream failed\n");
		}
		return;
	}

usage:
	printf("\nUsage: stream <enable|disable>\n");
	printf("Arg's:\tenable -> To enable network streaming\n");
	printf("\tdisble -> To disable network streaming\n");
	return;
}

/***********************************************************************
 * @function : print the usage
 *
 * @arg	 : command name
 * @return	 : void
 * *******************************************************************/
void usage(char *name)
{
	printf("\nUsage: %s <video_server_ip>",name);
	printf("\n<video_server_ip> : (Optional) Required for remote access only\n");
}

/*******************************************
 * Main function for Command Line Interface
 ******************************************/
int main(int argc, char **argv)
{

	if(argc > 2) {
		usage(argv[0]);
		exit(0);
	}

	if((argc == 2) && (strcmp(argv[1],"-h")==0)) {
		usage(argv[0]);
		exit(0);
	}

	if(argc > 1) {
		strcpy(g_video_server_ip,argv[1]);
	}

	signal(SIGALRM, watchdog);
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

