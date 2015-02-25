# ========================================================================================
#  @file    : Makefile
#
#  @description : Makefile for the video application.
#
#  @author  : Aman Kumar (2014-2015)
#
#  @copyright   : The code contained herein is licensed under the GNU General Public License.
#               You may obtain a copy of the GNU General Public License Version 2 or later
#               at the following locations:
#               http://www.opensource.org/licenses/gpl-license.html
#               http://www.gnu.org/copyleft/gpl.html
# =========================================================================================*/

VERBOSE = @
BASEDIR = $(PWD)
TARGET =video_server
#CC =arm-poky-linux-gnueabi-gcc
CC =gcc
NONE=\033[0m
GREEN=\033[01;32m
RED=\033[01;31m
YELLOW=\033[01;33m
SRC_DIR =$(BASEDIR)/src
BIN_DIR=$(BASEDIR)/bin
LIB_DIR=$(BASEDIR)/lib
UTILS_DIR=$(BASEDIR)/tools
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
RELTARGET=$(BIN_DIR)/$(TARGET)
CFLAGS += -Wall -g -O3
#LD_FLAGS += -L${LIB_DIR}
LD_FLAGS += -lpthread
LD_FLAGS += -ljpeg
#LD_FLAGS += -losdapi -lalgo
CPPFLAGS += -I. \
		-I./inc/ \

.PHONY: clean app utils video_server

all: configure video_server utils

configure:
	${VERBOSE}mkdir -p ${BIN_DIR}
	${VERBOSE}mkdir -p ${BASEDIR}/records
	${VERBOSE}mkdir -p ${BASEDIR}/records/videos
	${VERBOSE}mkdir -p ${BASEDIR}/records/videos/raw
	${VERBOSE}mkdir -p ${BASEDIR}/records/videos/mp4
	${VERBOSE}mkdir -p ${BASEDIR}/records/images
	${VERBOSE}mkdir -p ${BASEDIR}/records/images/raw
	${VERBOSE}mkdir -p ${BASEDIR}/records/images/jpeg

video_server:
	${VERBOSE} ${CC} ${SRC_DIR}/*.c ${CFLAGS} ${CPPFLAGS} ${LD_FLAGS} -o ${RELTARGET}
	${VERBOSE}echo "video_server compilation....${GREEN}SUCCESS${NONE}"
	${VERBOSE}echo "Executable ${GREEN}${TARGET}${NONE} has been created inside ${YELLOW}${BIN_DIR}/${NONE}"

utils:
	${VERBOSE}gcc ${UTILS_DIR}/UYVY_2_YUYV.c -o ${BIN_DIR}/uyvy2yuyv

clean:
	${VERBOSE}rm -f $(BIN_DIR)/*
	${VERBOSE}echo "All Binaries ${RED}removed${NONE}"

clean_records:
	${VERBOSE}rm -f ${BASEDIR}/records/videos/raw/*
	${VERBOSE}rm -f ${BASEDIR}/records/videos/mp4/*
	${VERBOSE}rm -f ${BASEDIR}/records/images/raw/*
	${VERBOSE}rm -f ${BASEDIR}/records/images/jpeg/*
	${VERBOSE}echo "All records ${RED}deleted${NONE}"


