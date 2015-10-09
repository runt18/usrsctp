/*-
 * Copyright (c) 2001-2007, by Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2008-2012, by Randall Stewart. All rights reserved.
 * Copyright (c) 2008-2012, by Michael Tuexen. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef __FreeBSD__
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");
#endif

#ifndef _NETINET_SCTP_CALLOUT_
#define _NETINET_SCTP_CALLOUT_

/*
 * NOTE: the following MACROS are required for locking the callout
 * queue along with a lock/mutex in the OS specific headers and
 * implementation files::
 * - SCTP_TIMERQ_LOCK()
 * - SCTP_TIMERQ_UNLOCK()
 * - SCTP_TIMERQ_LOCK_INIT()
 * - SCTP_TIMERQ_LOCK_DESTROY()
 */

#define _SCTP_NEEDS_CALLOUT_ 1

#define SCTP_TICKS_PER_FASTTIMO 20	/* called about every 20ms */

#if defined(__Userspace__)
#if defined(__Userspace_os_Windows)
#define SCTP_TIMERQ_LOCK()          EnterCriticalSection(&SCTP_BASE_VAR(timer_mtx))
#define SCTP_TIMERQ_UNLOCK()        LeaveCriticalSection(&SCTP_BASE_VAR(timer_mtx))
#define SCTP_TIMERQ_LOCK_INIT()     InitializeCriticalSection(&SCTP_BASE_VAR(timer_mtx))
#define SCTP_TIMERQ_LOCK_DESTROY()  DeleteCriticalSection(&SCTP_BASE_VAR(timer_mtx))
#else
#define SCTP_TIMERQ_LOCK()          (void)pthread_mutex_lock(&SCTP_BASE_VAR(timer_mtx))
#define SCTP_TIMERQ_UNLOCK()        (void)pthread_mutex_unlock(&SCTP_BASE_VAR(timer_mtx))
#define SCTP_TIMERQ_LOCK_INIT()     (void)pthread_mutex_init(&SCTP_BASE_VAR(timer_mtx), NULL)
#define SCTP_TIMERQ_LOCK_DESTROY()  (void)pthread_mutex_destroy(&SCTP_BASE_VAR(timer_mtx))
#endif

#endif

int sctp_get_tick_count();

TAILQ_HEAD(calloutlist, sctp_callout);

struct sctp_callout {
	TAILQ_ENTRY(sctp_callout) tqe;
	int c_time;		/* ticks to the event */
	void *c_arg;		/* function argument */
	void (*c_func)(void *);	/* function to call */
	int c_flags;		/* state of this entry */
};
typedef struct sctp_callout sctp_os_timer_t;

#define	SCTP_CALLOUT_ACTIVE	0x0002	/* callout is currently active */
#define	SCTP_CALLOUT_PENDING	0x0004	/* callout is waiting for timeout */

void sctp_os_timer_init(sctp_os_timer_t *tmr);
void sctp_os_timer_start(sctp_os_timer_t *, int, void (*)(void *), void *);
int sctp_os_timer_stop(sctp_os_timer_t *);

#define SCTP_OS_TIMER_INIT	sctp_os_timer_init
#define SCTP_OS_TIMER_START	sctp_os_timer_start
#define SCTP_OS_TIMER_STOP	sctp_os_timer_stop
/* MT FIXME: Is the following correct? */
#define SCTP_OS_TIMER_STOP_DRAIN SCTP_OS_TIMER_STOP
#define	SCTP_OS_TIMER_PENDING(tmr) ((tmr)->c_flags & SCTP_CALLOUT_PENDING)
#define	SCTP_OS_TIMER_ACTIVE(tmr) ((tmr)->c_flags & SCTP_CALLOUT_ACTIVE)
#define	SCTP_OS_TIMER_DEACTIVATE(tmr) ((tmr)->c_flags &= ~SCTP_CALLOUT_ACTIVE)

#if defined(__Userspace__)
void sctp_start_timer(void);
#endif
#if defined(__APPLE__)
void sctp_timeout(void *);
#endif

#endif
