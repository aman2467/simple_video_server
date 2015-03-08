/* ==========================================================================
 * @file    : osd_api.c
 *
 * @description : This file contains different API definitions used for OSD.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <common.h>
#include <osd_thread.h>

extern char ascii_data[STRING_WIDTH*TEXT_HEIGHT*BPP];
extern char *ascii_string;

/****************************************************************************
 * @func    : This initializes OSD ascii database.
 * @arg    : pointer to osd data pointer for all window.
 * @return  : success/failure of funtion
 ***************************************************************************/
int osd_init(char *osd)
{
	FILE *fp;
	char filename[100]={0};

	strcpy(filename,OSD_DATABASE);
	strcat(filename,"/ascii.yuyv");
	fp = fopen(filename,"rb");
	if(fp == NULL) printf("fail\n");
	if(fread(ascii_data,1,STRING_WIDTH*TEXT_HEIGHT*BPP,fp) != STRING_WIDTH*TEXT_HEIGHT*BPP);
	fclose(fp);
	if((ascii_string = calloc(78,1)) == NULL) {
		printf("Memory not allocated to OSD\n");
		return FAIL;
	}
	memcpy(ascii_string,ASCII_STRING,77);
	if((osd = calloc(400*400*2,1)) == NULL) {
		printf("Memory not allocated to OSD\n");
		return FAIL;
	}
	return SUCCESS;
}

/****************************************************************************
 * @func    : fill_osd_line
 *          This fills a line of video data with osd data.
 * @arg1    : pointer to required pixel of video data.
 * @arg2    : pointer to osd data.
 * @arg3    : size of one line of osd data
 * @arg4    : transparency value
 * @return  : void
 ***************************************************************************/
void fill_osd_data(char *dest, char *src, int size, int transparency)
{
	char *pdest =(char *)dest;
	char *psrc = (char *)src;
	int i;

	int loop = (size / sizeof(char));
	for(i = 0; i < loop; ++i) {
		if((transparency == TRUE) && ((*((char *)psrc)&0xFF) == 0x00)) {
			/* don't do anything */
		} else {
			if(*psrc == 0xff) *psrc = 0x00;
			*((char *)pdest) = *((char *)psrc);
		}
		pdest += sizeof(char);
		psrc += sizeof(char);
	}
}

/****************************************************************************
 * @func    : creates OSD data for a particular string from ASCII database.
 * @arg1    : pointer to string which is to be displayed
 * @arg2    : pointer to osd data buffer for that string
 * @return  : void
 ***************************************************************************/
void get_osd_string(char *str, char *osdstring)
{
	int arr[20] = {'\0'};
	char *ptr, *ptrs;
	int i, j;

	ptrs = ascii_string;
	ptr = str;

	j = 0;
	while(*ptr != '\0') {
		if(*ptr >= 65 && *ptr <= 90 )
			ptrs = ascii_string;
		else if(*ptr >= 97 && *ptr <= 122)
			ptrs = ascii_string + 26;
		else if(*ptr >= 48 && *ptr <= 57)
			ptrs = ascii_string + 52;
		else
			ptrs = ascii_string + 62;
		while(ptrs != NULL) {
			if(*ptrs == *ptr) {
				arr[j] = ptrs - ascii_string;
				j++;
				break;
			}
			ptrs++;
		}
		ptr++;
	}

	i = 0;
	ptrs = osdstring;
	while(i < TEXT_HEIGHT) {
		ptr = ascii_data + i*STRING_WIDTH*BPP;
		for(j = 0;j< strlen(str);j++) {
			memcpy(ptrs,ptr+arr[j]*TEXT_WIDTH*BPP,TEXT_WIDTH*BPP);
			ptrs += TEXT_WIDTH*BPP;
		}
		i++;
	}
}
