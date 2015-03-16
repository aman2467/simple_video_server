# ========================================================================================
#  @file    : Makefile
#
#  @description : Makefile for the video application.
#
#  @author  : Aman Kumar (2015)
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
CYAN=\033[01;36m
SRC_DIR =$(BASEDIR)/src
INTERFACE_DIR=$(BASEDIR)/interface/src
DATABASE_DIR=${BASEDIR}/database
BIN_DIR=$(BASEDIR)/bin
LIB_DIR=$(BASEDIR)/lib
UTILS_DIR=$(BASEDIR)/tools
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
RELTARGET=$(BIN_DIR)/$(TARGET)
CFLAGS += -g -O3
#LD_FLAGS += -L${LIB_DIR}
LD_FLAGS += -lpthread
LD_FLAGS += -ljpeg
LD_FLAGS += -lSDL2
#LD_FLAGS += -losdapi -lalgo
CPPFLAGS += -I. \
		-I./inc/ \
		-I./interface/inc \

.PHONY: clean all video_server player cli_app info

all: video_server cli_app player info

video_server:
	${VERBOSE} sed -i "s,PATH,$(DATABASE_DIR),g"  $(BASEDIR)/inc/osd_thread.h
	${VERBOSE} ${CC} ${SRC_DIR}/*.c ${CFLAGS} ${CPPFLAGS} ${LD_FLAGS} -o ${RELTARGET}

player:
	${VERBOSE}gcc ${UTILS_DIR}/video_player.c ${LD_FLAGS} ${CFLAGS} -o ${BIN_DIR}/raw_player

cli_app:
	${VERBOSE}gcc ${INTERFACE_DIR}/*.c ${CPPFLAGS} -o ${BIN_DIR}/cli_app

clean:
	${VERBOSE}rm -f $(BIN_DIR)/*
	${VERBOSE}echo "All Binaries ${RED}removed${NONE}"

info:
	${VERBOSE}echo "                                         ${YELLOW}<<#|#>> "
	${VERBOSE}echo "                                         -(${RED}o o${YELLOW})-${NONE}"
	${VERBOSE}echo "${CYAN}======================================${YELLOW}oOO${CYAN}==${YELLOW}(${RED}^${YELLOW})${CYAN}==${YELLOW}OOo${CYAN}=====================================${NONE}"
	${VERBOSE}echo " "
	${VERBOSE}echo "${YELLOW}Binary Path :" 
	${VERBOSE}echo "        ${GREEN}${BIN_DIR}/${NONE}"
	${VERBOSE}echo "${YELLOW}Binaries :"
	${VERBOSE}echo "        ${GREEN}1. ${TARGET}${NONE}"
	${VERBOSE}echo "        ${GREEN}2. cli_app${NONE}"
	${VERBOSE}echo "        ${GREEN}3. raw_player${NONE}"
	${VERBOSE}echo " "
	${VERBOSE}echo "${CYAN}========================================================================================"
	${VERBOSE}echo " "
