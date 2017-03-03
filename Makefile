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
TARGET_T1=test_player
TARGET_T2=nw_frame_receiver
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
TOOLS_DIR=$(BASEDIR)/tools
UTILS_DIR=$(BASEDIR)/utils
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
RELTARGET=$(BIN_DIR)/$(TARGET)
CFLAGS += -g -O3
#LD_FLAGS += -L${LIB_DIR}
LD_FLAGS += -lpthread
LD_FLAGS += -ljpeg
LD_FLAGS += -lSDL2
CPPFLAGS += -I. \
		-I./inc/ \
		-I./interface/inc \

.PHONY: clean all video_server player cli_app info

all: video_server cli_app player nw_receiver info

video_server:
	${VERBOSE} sed -i "s,PATH,$(DATABASE_DIR),g"  $(BASEDIR)/inc/osd_thread.h
	${VERBOSE} ${CC} ${SRC_DIR}/*.c ${CFLAGS} ${CPPFLAGS} ${LD_FLAGS} -o ${RELTARGET}
	${VERBOSE} sed -i "s,${DATABASE_DIR},"PATH",g"  $(BASEDIR)/inc/osd_thread.h

player:
	${VERBOSE}gcc ${TOOLS_DIR}/${TARGET_T1}/${TARGET_T1}.c ${LD_FLAGS} ${CFLAGS} -o ${BIN_DIR}/${TARGET_T1}

nw_receiver:
	${VERBOSE}make -s -C ${TOOLS_DIR}/${TARGET_T2}/ all

cli_app:
	${VERBOSE}gcc ${INTERFACE_DIR}/*.c ${CPPFLAGS} -o ${BIN_DIR}/cli_app

clean:
	${VERBOSE}rm -f $(BIN_DIR)/*
	${VERBOSE}echo -e "All Binaries ${RED}removed${NONE}"

info:
	${VERBOSE}echo -e "                                         ${YELLOW}<<#|#>> "
	${VERBOSE}echo -e "                                         -(${RED}o o${YELLOW})-${NONE}"
	${VERBOSE}echo -e "${CYAN}======================================${YELLOW}oOO${CYAN}==${YELLOW}(${RED}^${YELLOW})${CYAN}==${YELLOW}OOo${CYAN}=====================================${NONE}"
	${VERBOSE}echo -e " "
	${VERBOSE}echo -e "${YELLOW}Binary Path :"
	${VERBOSE}echo -e "        ${GREEN}${BIN_DIR}/${NONE}"
	${VERBOSE}echo -e "${YELLOW}Binaries :"
	${VERBOSE}echo -e "        ${GREEN}1. ${TARGET}${NONE}"
	${VERBOSE}echo -e "        ${GREEN}2. cli_app${NONE}"
	${VERBOSE}echo -e "        ${GREEN}3. ${TARGET_T1}${NONE}"
	${VERBOSE}echo -e "        ${GREEN}4. ${TARGET_T2}${NONE}"
	${VERBOSE}echo -e " "
	${VERBOSE}echo -e "${CYAN}========================================================================================"
	${VERBOSE}echo -e "                                                                         \ "
	${VERBOSE}echo -e "                                                                          \ "
	${VERBOSE}echo -e "                                                                            ${GREEN}.--. "
	${VERBOSE}echo -e "                                                                           |${RED}o${YELLOW}_${RED}o${GREEN} | "
	${VERBOSE}echo -e "                                                                           |${YELLOW}:_/${GREEN} | "
	${VERBOSE}echo -e "                                                                          //   \ \ "
	${VERBOSE}echo -e "                                                                         (|     | ) "
	${VERBOSE}echo -e "                                                                        /'\_   _/'\ "
	${VERBOSE}echo -e "                                                                        \___)=(___/${NONE} "
	${VERBOSE}echo -e " "

install:
	${VERBOSE}sudo mkdir -p /usr/share/svs
	${VERBOSE}sudo cp ${UTILS_DIR}/logo.png /usr/share/svs/
	${VERBOSE}sudo cp ${BIN_DIR}/${TARGET} /usr/local/bin/
	${VERBOSE}sudo cp ${TARGET}.desktop /usr/share/applications/
