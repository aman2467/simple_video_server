# ========================================================================================
#  @file    : Makefile
#
#  @description : Makefile for the application.
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
TARGET =nw_receiver
#CC =arm-poky-linux-gnueabi-gcc
CC =gcc
DIR= $(shell pwd)
S_DIR =$(DIR)/src
B_DIR=$(DIR)/../../bin
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
CFLAGS += -g -O3
LD_FLAGS += -lpthread
LD_FLAGS += -lSDL2
CPPFLAGS += -I. \
		-I./inc/

.PHONY: all nw_receiver clean

all: clean nw_receiver

nw_receiver:
	${VERBOSE} ${CC} ${S_DIR}/*.c ${CFLAGS} ${CPPFLAGS} ${LD_FLAGS} -o ${B_DIR}/${TARGET}

clean:
	${VERBOSE} rm -f ${B_DIR}/${TARGET}
