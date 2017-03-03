/*
 * @file    : face.h
 *
 * @description : This file contains common definitions and declaration for
 *           the application.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _FACE_H_
#define _FACE_H_

#ifdef __cplusplus
extern "C" {
#endif

int face_detect_init(void);
int face_detect_and_draw(unsigned char *frame, int x, int y);

#ifdef __cplusplus
}
#endif
#endif /* _FACE_H_ */
