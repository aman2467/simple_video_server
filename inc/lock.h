/* ==========================================================================
 * @file    : lock.h
 *
 * @description : This file contains lock specific declaration and 
 *           prototypes.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *				Public License. You may obtain a copy of the GNU General
 *				Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#ifndef _LOCK_H
#define _LOCK_H

typedef struct lock {
	volatile int lock;
} lock_t;

void lock_init(lock_t *);
void lock(lock_t *);
void unlock(lock_t *);
void spin(void);

#endif
