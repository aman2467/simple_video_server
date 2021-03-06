/*
 * @file    : cli_main.c
 *
 * @description : This file contains main program for cli_app.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

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

char prompt[50] = "CLI:/> ";
char g_video_server_ip[16] = "127.0.0.1";
int quit_flag = FALSE;

static char  acCmndLine[CMND_LINE_MAX_LEN + 2];
static char  *pcCmndLinePtr;
static UINT16 uwCmndArgCount;
static char  *apcCmndArgVal[CLI_MAX_ARGS];
static const char kszCommandError[] = "command not found\n";

/*
 * COMMAND TABLE
 *
 * Application-specific command functions go at top of table...
 * (optionally in alphabetical order, or some other systematic arrangement)
 *
 */
const  struct  CmndTableEntry_t  asCommand[] = {
	/*  Name		Attribute       Address   */
	{"clear",		USER_CMD,	CmndClearScreen},
	{"date",		USER_CMD,	CmndDate},
	{"exit",		USER_CMD,	CmndExit},
	{"help",		USER_CMD,	CmndHelp},
	{"ls",			USER_CMD,	CmndList},
	{"osdwinenable",	USER_CMD,	CmndOsdWinEnable},
	{"osdwintext",		USER_CMD,	CmndOsdWinText},
	{"osdwinpos",		USER_CMD,	CmndOsdWinPos},
	{"osdonimage",		USER_CMD,	CmndOsdOnImage},
	{"osdonvideo",		USER_CMD,	CmndOsdOnVideo},
	{"osdwintrans",		USER_CMD,	CmndOsdWinTrans},
	{"recordvideo",		USER_CMD,	CmndRecordVideo},
	{"setalgotype",		USER_CMD,	CmndSetAlgoType},
	{"stream",		USER_CMD,	CmndNWStream},
	{"takesnap",		USER_CMD,	CmndTakeSnap},
	{"time",		USER_CMD,	CmndTime},
	{"$",			DEBUG_CMD,      NULL},   /* Dummy last entry */
};

/*
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
 */
void CommandLineInterpreter(void)
{
	char c;
	UINT8 bArgIndex, bCmndIndex;
	UINT8 yCmndNameFound = FALSE;
	int isInvalid = 0;

	pcCmndLinePtr = acCmndLine; /* point to start of Cmnd Line buffer */
	uwCmndArgCount = 0;

	for (bArgIndex = 0; bArgIndex < CLI_MAX_ARGS; bArgIndex++) {
		if (!isprint(*pcCmndLinePtr)) {  /* stop at end of line */
			if (isInvalid == 0)
				isInvalid = 2;
			break;
		}

		isInvalid = 1;

		while (*pcCmndLinePtr == SPACE) /* skip leading spaces */
			pcCmndLinePtr++;

		if (!isprint(*pcCmndLinePtr)) /* end of line found */
			break;

		apcCmndArgVal[bArgIndex] = pcCmndLinePtr; /* Make ptr to arg */
		uwCmndArgCount++;

		while ((c = *pcCmndLinePtr) != SPACE) {
			if (!isprint(c))
				break;
			pcCmndLinePtr++;
		}

		if (!isprint(*pcCmndLinePtr)) /* stop at end of line */
			break;

		*pcCmndLinePtr++ = NUL; /* terminate the arg */
	}

	if (isInvalid != 2) {
		/* Search cmd table */
		for (bCmndIndex = 0; bCmndIndex < MAX_COMMANDS; bCmndIndex++) {
			if (*asCommand[bCmndIndex].pzName == '$')
				break;
			if (!strcmp(apcCmndArgVal[0],
				    asCommand[bCmndIndex].pzName)) {
				yCmndNameFound = TRUE;
				break;
			}
		}
		/* There is one or more user-supplied arg(s) */
		if (uwCmndArgCount > 1) {
			if (!strcmp(apcCmndArgVal[1], "-h"))
				apcCmndArgVal[1] = "?";
		}

		if (yCmndNameFound)
			(*asCommand[bCmndIndex].Function)(uwCmndArgCount,
							  apcCmndArgVal);
		else
			printf("%s", kszCommandError);
	} else
		printf("%s", kszCommandError);
}

/*
 * CmndList :  This function is called when a Command 'ls' is
 *	  typed in the CLI prompt. It lists all commands.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndList(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	char *command;
	int i;

	if (uwCmndArgCount == 1)	{
		for (i = 0; i < MAX_COMMANDS; i++) { /* List commands */
			command = asCommand[i].pzName;
			if (*command == '$')
				break;
			printf("%s\n", command);
		}
	}
}

/*
 * CmndHelp : This function is called when a Command 'help' is
 *	  typed in the CLI. It prints the
 *	  output according to the arguments passed
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
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
		printf("Arg's:	 ? : show command usage\n");
		printf("\t-h : same as '?'(Applicable for all commands)\n");
		printf("\t-d : list debug commands only\n");
		printf("\t-a : list all commands\n\n");
		return;
	}
	/* List commands */
	for (bCmndIndex = 0; bCmndIndex < MAX_COMMANDS; bCmndIndex++) {
		pzCommandName = asCommand[bCmndIndex].pzName;
		if (*pzCommandName == '$')
			break; /* Reached end of Cmnd Table */

		if ((yOptionAll || uwCmndArgCount) == 1) {
			printf("%s", pzCommandName);
			printf("\n");
		} else if (yOptionDebug &&
			   (asCommand[bCmndIndex].yAttribute == DEBUG_CMD)) {
			printf("%s", pzCommandName);
			printf("\n");
		}
	}
}

/*
 * CmndClearScreen :  This function is called when a Command 'clear' is
 *	  typed in the CLI prompt. It clears the screen.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndClearScreen(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	system("clear");
}

/*
 * CmndExit :  This function is called when a Command 'exit' is
 *			  typed in the CLI prompt. It exits the CLI interface.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndExit(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	exit(0);
}

/*
 * CmndTakeSnap :  This function is called when a Command 'takesnap' is
 *	  typed in the CLI prompt. It saves a snapshot.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndTakeSnap(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int type;

	if (uwCmndArgCount == 2) {
		type = atoi(apcCmndArgVal[1]);
		if (strcmp("jpeg", apcCmndArgVal[1]) == 0)
			type = 0;
		else if (strcmp("raw", apcCmndArgVal[1]) == 0)
			type = 1;
		else {
			printf("\nEnter appropriate argument\n");
			goto usage;
		}
		if (takesnap(type) < 0)
			printf("\nCommand takesnap failed\n");
		return;
	}

usage:
	printf("\nUsage: takesnap <jpeg|raw>\n");
	printf("Arg's:\tjpeg -> JPEG Snapshot\n");
	printf("\traw -> Raw Snapshot\n");
}

/*
 * CmndRecordVideo :  This function is called when a Command 'recordvideo'
 *	  is typed in the CLI prompt. It saves a video.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndRecordVideo(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int val;

	if (uwCmndArgCount == 2) {
		if (strcmp("start", apcCmndArgVal[1]) == 0)
			val = 1;
		else if (strcmp("stop", apcCmndArgVal[1]) == 0)
			val = 0;
		else {
			printf("\nEnter appropriate argument\n");
			goto usage;
		}
		if (recordvideo(val) < 0)
			printf("\nCommand recordvideo failed\n");
		return;
	}

usage:
	printf("\nUsage: recordvideo <start|stop>\n");
	printf("Arg's:\tstart -> To start a recording\n");
	printf("\tstop -> To stop a recording\n");
}

/*
 * CmndSetAlgoType :  This function is called when a Command 'setalgotype'
 *	  is typed in the CLI prompt. It sets algo type.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndSetAlgoType(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int type;

	if (uwCmndArgCount == 2) {
		if (strcmp("none", apcCmndArgVal[1]) == 0)
			type = 0;
		else if (strcmp("bw", apcCmndArgVal[1]) == 0)
			type = 1;
		else if (strcmp("cartoon", apcCmndArgVal[1]) == 0)
			type = 2;
		else {
			printf("\nEnter appropriate argument\n");
			goto usage;
		}
		if (setalgotype(type) < 0)
			printf("\nCommand setalgotype failed\n");
		return;
	}

usage:
	printf("\nUsage: setalgotype <none|bw|cartoon>\n");
	printf("Arg's:\tnone -> To get normal video/image\n");
	printf("\tbw -> To get black&white video/image\n");
	printf("\tcartoon -> To get cartoonised video/image\n");
}

/*
 * CmndOsdWinEnable :  This function is called when a Command 'osdwinenable'
 *	  is typed in the CLI prompt. It enables/disables osd windows.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndOsdWinEnable(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win, enable;

	if (uwCmndArgCount == 3) {
		win = atoi(apcCmndArgVal[1]);
		if (win < 0 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		if (strcmp("enable", apcCmndArgVal[2]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[2]) == 0)
			enable = 0;
		if (osdwinenable(win, enable) < 0)
			printf("\nCommand osdwinenable failed\n");
		return;
	}

usage:
	printf("\nUsage: osdwinenable <OSD window no.> <enable|disable>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 0 to 9\n");
	printf("\t2. enable -> To enable given OSD Window\n");
	printf("\t   disble -> To disable given OSD Window\n");
}

/*
 * CmndOsdWinText :  This function is called when a Command 'osdwintext'
 *	  is typed in the CLI prompt. It adds/changes OSD text.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndOsdWinText(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win;
	char text[25] = {0};

	if (uwCmndArgCount >= 3) {
		win = atoi(apcCmndArgVal[1]);
		if (win < 5 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		if (uwCmndArgCount == 3) {
			if (strlen(apcCmndArgVal[2]) > 20) {
				printf("Maximum allowed text size is 20.");
				return;
			}
			if (osdwintext(win, apcCmndArgVal[2]) < 0)
				printf("\nCommand osdwintext failed\n");
			return;
		} else if (uwCmndArgCount == 4) {
			if (strlen(apcCmndArgVal[2]) +
			    strlen(apcCmndArgVal[3]) > 19) {
				printf("Maximum allowed text size is 20.");
				return;
			}
			strcpy(text, apcCmndArgVal[2]);
			text[strlen(apcCmndArgVal[2])] = ' ';
			strcat(text, apcCmndArgVal[3]);
			if (osdwintext(win, text) < 0)
				printf("\nCommand osdwintext failed\n");
			return;
		}
		printf("\nOnly one space is allowed\n");
	}

usage:
	printf("\nUsage: osdwintext <OSD window no.> <text>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 5 to 9\n");
	printf("\t2. text -> text to be displayed over video\n");
}

/*
 * CmndOsdWinPos :  This function is called when a Command 'osdwinpos'
 *	  is typed in the CLI prompt. It changes posiiton of OSD window.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndOsdWinPos(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win, x, y;

	if (uwCmndArgCount == 4) {
		win = atoi(apcCmndArgVal[1]);
		if ((win < 0) || (win > 9)) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		x = atoi(apcCmndArgVal[2]);
		if ((x < 0) || (x > 1280)) {
			printf("\nInvalid Position\n");
			goto usage;
		}
		y = atoi(apcCmndArgVal[3]);
		if ((y < 0) || (y > 720)) {
			printf("\nInvalid Position\n");
			goto usage;
		}
		if (osdwinpos(win, x, y) < 0)
			printf("\nCommand osdwinpos failed\n");
		return;
	}

usage:
	printf("\nUsage: osdwinpos <OSD window no.> <X-pos> <Y-pos>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 4 to 9\n");
	printf("\t2. x -> x co-ordinate for the given OSD window\n");
	printf("\t3. y -> y co-ordinate for the given OSD window\n");
}

/*
 * CmndOsdOnImage :  This function is called when a Command 'osdonimage'
 *	  is typed in the CLI prompt. It enables/disables OSD on saved snapshot.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndOsdOnImage(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;

	if (uwCmndArgCount == 2) {
		if (strcmp("enable", apcCmndArgVal[1]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[1]) == 0)
			enable = 0;

		if (osdonimage(enable) < 0)
			printf("\nCommand osdonimage failed\n");
		return;
	}

usage:
	printf("\nUsage: osdonimage <enable|disable>\n");
	printf("Arg's:\tenable -> To enable OSD on recorded snapshot\n");
	printf("\tdisble -> To disable OSD on recorded snapshot\n");
}

/**********************************************************************
 * CmndOsdOnVideo :  This function is called when a Command 'osdonvideo'
 *   is typed in the CLI prompt. It enables/disables OSD on saved video.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 * ********************************************************************/
void CmndOsdOnVideo(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;

	if (uwCmndArgCount == 2) {
		if (strcmp("enable", apcCmndArgVal[1]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[1]) == 0)
			enable = 0;
		if (osdonvideo(enable) < 0)
			printf("\nCommand osdonvideo failed\n");
		return;
	}

usage:
	printf("\nUsage: osdonvideo <enable|disable>\n");
	printf("Arg's:\tenable -> To enable OSD on recorded video\n");
	printf("\tdisble -> To disable OSD on recorded video\n");
}

/*
 * CmndOsdWinTrans :  This function is called when a Command 'osdwintrans'
 *	  is typed in the CLI prompt. It enables/disables osd transparency.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndOsdWinTrans(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int win, enable;

	if (uwCmndArgCount == 3) {
		win = atoi(apcCmndArgVal[1]);
		if (win < 0 || win > 9) {
			printf("\nInvalid OSD Window number\n");
			goto usage;
		}
		if (strcmp("enable", apcCmndArgVal[2]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[2]) == 0)
			enable = 0;
		if (osdwintrans(win, enable) < 0)
			printf("\nCommand osdwintrans failed\n");
		return;
	}

usage:
	printf("\nUsage: osdwintrans <OSD window no.> <enable|disable>\n");
	printf("Arg's:\t1. OSD window no -> OSD window between 0 to 9\n");
	printf("\t2. enable -> To enable given OSD Window\n");
	printf("\t   disble -> To disable given OSD Window\n");
}

/*
 * CmndNWStream :  This function is called when a Command 'stream'
 *	  is typed in the CLI prompt. It enables/disables network stream.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndNWStream(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;

	if (uwCmndArgCount == 2) {
		if (strcmp("enable", apcCmndArgVal[1]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[1]) == 0)
			enable = 0;
		if (nw_stream(enable) < 0)
			printf("\nCommand stream failed\n");
		return;
	}

usage:
	printf("\nUsage: stream <enable|disable>\n");
	printf("Arg's:\tenable -> To enable network streaming\n");
	printf("\tdisble -> To disable network streaming\n");
}

/*
 * CmndDate :  This function is called when a Command 'date'
 *	  is typed in the CLI prompt. It enables/disables date stamp.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndDate(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;

	if (uwCmndArgCount == 2) {
		if (strcmp("enable", apcCmndArgVal[1]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[1]) == 0)
			enable = 0;
		if (enable_date(enable) < 0)
			printf("\nCommand date failed\n");
		return;
	}

usage:
	printf("\nUsage: date <enable|disable>\n");
	printf("Arg's:\tenable -> To enable date stamp over video\n");
	printf("\tdisble -> To disable date stamp over video\n");
}

/*
 * CmndTime :  This function is called when a Command 'time'
 *	  is typed in the CLI prompt. It enables/disables time stamp.
 *
 * @uwCmndArgCount   : Number of Cmnd Line args (incl. Cmnd name)
 * @*apcCmndArgVal[] : Array of pointers to Command Line args
 * @return value     : void
 */
void CmndTime(UINT16 uwCmndArgCount, char *apcCmndArgVal[])
{
	int enable;

	if (uwCmndArgCount == 2) {
		if (strcmp("enable", apcCmndArgVal[1]) == 0)
			enable = 1;
		else if (strcmp("disable", apcCmndArgVal[1]) == 0)
			enable = 0;

		if (enable_time(enable) < 0)
			printf("\nCommand time failed\n");
		return;
	}

usage:
	printf("\nUsage: time <enable|disable>\n");
	printf("Arg's:\tenable -> To enable time stamp over video\n");
	printf("\tdisble -> To disable time stamp over video\n");
}

/*
 * @function : print the usage
 *
 * @arg	 : command name
 * @return	 : void
 */
void usage(char *name)
{
	printf("\nUsage: %s <video_server_ip>", name);
	printf("\n<video_server_ip> : (Optional)\n");
}

/**
 * Main function for Command Line Interface
 */
int main(int argc, char **argv)
{
#ifdef USER_PROMPT
	FILE *fp;
	char host[10] = {0};
	char *env = getenv("USER");

	if (env != NULL) {
		strcpy(prompt, env);
		strcat(prompt, "@");
		system("hostname > .dummy");
		fp = fopen(".dummy", "r");
		if (fp != NULL) {
			fread(host, 30, 1, fp);
			strncat(prompt, host, strlen(host) - 1);
			strcat(prompt, " : ");
			fclose(fp);
		}
		system("rm .dummy");
	}
#endif

	if (argc > 2) {
		usage(argv[0]);
		exit(0);
	}

	if ((argc == 2) && (strcmp(argv[1], "-h") == 0)) {
		usage(argv[0]);
		exit(0);
	}

	if (argc > 1) {
		if (is_valid_ip(argv[1]))
			strcpy(g_video_server_ip, argv[1]);
		else {
			printf("\nError : Enter Valid IP");
			usage(argv[0]);
			exit(0);
		}
	}

	signal(SIGALRM, watchdog);
	while (1) {
		int i = 0;

#ifdef DISPLAY_COMMANDS
		printf("Commands :");
		while (asCommand[i].pzName[0] != '$') {
			printf("\x1b[01;32m %s", asCommand[i].pzName);
			i++;
		}
#endif
		printf("\x1b[01;31m%s\x1b[0m", prompt);
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
