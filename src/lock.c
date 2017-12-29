/* ==========================================================================
 * @file    : lock.c
 *
 * @description : This file contains APIs definition used for lock.
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
#include <unistd.h>
#include <lock.h>

/*
 * @usage : This function waits for given time
 *
 * @arg1   : void
 * @return     : void
 */
void spin(void)
{
	usleep(10);
}

/*
 * @usage : This function inits given lock
 *
 * @arg1   : lock structure pointer
 * @return     : void
 */
void lock_init(lock_t *lock)
{
	__sync_lock_release(&lock->lock);
}

/*
 * @usage : This function locks given lock
 *
 * @arg1   : lock structure pointer
 * @return     : void
 */
void lock(lock_t *lock)
{
	while (__sync_lock_test_and_set(&lock->lock, 1))
		while (lock->lock)
			spin();
}

/*
 * @usage : This function unlocks given lock
 *
 * @arg1   : lock structure pointer
 * @return     : void
 */
void unlock(lock_t *lock)
{
	__sync_lock_release(&lock->lock);
}
