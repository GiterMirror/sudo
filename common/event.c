/*
 * Copyright (c) 2013 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif /* STDC_HEADERS */
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# include "compat/stdbool.h"
#endif /* HAVE_STDBOOL_H */
#ifdef HAVE_STRING_H
# include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <errno.h>

#include "missing.h"
#include "alloc.h"
#include "fatal.h"
#include "sudo_debug.h"
#include "sudo_event.h"

/* XXX - use non-exiting allocators? */

struct sudo_event_base *
sudo_ev_base_alloc(void)
{
    struct sudo_event_base *base;
    debug_decl(sudo_ev_base_alloc, SUDO_DEBUG_EVENT)

    base = ecalloc(1, sizeof(*base));
    TAILQ_INIT(&base->events);
    TAILQ_INIT(&base->timeouts);
    if (sudo_ev_base_alloc_impl(base) != 0) {
	efree(base);
	base = NULL;
    }

    debug_return_ptr(base);
}

void
sudo_ev_base_free(struct sudo_event_base *base)
{
    struct sudo_event *next;
    debug_decl(sudo_ev_base_free, SUDO_DEBUG_EVENT)

    /* Remove any existing events before freeing the base. */
    TAILQ_FOREACH_SAFE(base->cur, &base->events, entries, next) {
	sudo_ev_del(base, base->cur);
    }
    sudo_ev_base_free_impl(base);
    efree(base);

    debug_return;
}

struct sudo_event *
sudo_ev_alloc(int fd, short events, sudo_ev_callback_t callback, void *closure)
{
    struct sudo_event *ev;
    debug_decl(sudo_ev_alloc, SUDO_DEBUG_EVENT)

    /* XXX - sanity check events value */

    ev = ecalloc(1, sizeof(*ev));
    ev->fd = fd;
    ev->events = events;
    ev->pfd_idx = -1;
    ev->callback = callback;
    ev->closure = closure;

    debug_return_ptr(ev);
}

void
sudo_ev_free(struct sudo_event *ev)
{
    debug_decl(sudo_ev_free, SUDO_DEBUG_EVENT)

    /* Make sure ev is not in use before freeing it. */
    if (ev->base != NULL)
	(void)sudo_ev_del(NULL, ev);
    free(ev);
    debug_return;
}

int
sudo_ev_add(struct sudo_event_base *base, struct sudo_event *ev,
    struct timeval *timo, bool tohead)
{
    debug_decl(sudo_ev_add, SUDO_DEBUG_EVENT)

    /* Only add new events to the events list. */
    if (ev->base == NULL) {
	if (sudo_ev_add_impl(base, ev) != 0)
	    debug_return_int(-1);
	ev->base = base;
	if (tohead) {
	    TAILQ_INSERT_HEAD(&base->events, ev, entries);
	} else {
	    TAILQ_INSERT_TAIL(&base->events, ev, entries);
	}
    } else {
	/* If no base specified, use existing one. */
	if (base == NULL)
	    base = ev->base;

	/* If event no longer has a timeout, remove from timeouts queue. */
	if (timo == NULL && timevalisset(&ev->timeout)) {
	    timevalclear(&ev->timeout);
	    TAILQ_REMOVE(&base->timeouts, ev, timeouts_entries);
	}
    }
    /* Timeouts can be changed for existing events. */
    if (timo != NULL) {
	struct sudo_event *evtmp;
	if (timevalisset(&ev->timeout)) {
	    /* Remove from timeouts list, then add back. */
	    TAILQ_REMOVE(&base->timeouts, ev, timeouts_entries);
	}
	/* Convert to absolute time and insert in sorted order; O(n). */
	gettimeofday(&ev->timeout, NULL);
	ev->timeout.tv_sec += timo->tv_sec;
	ev->timeout.tv_usec += timo->tv_usec;
	TAILQ_FOREACH(evtmp, &base->timeouts, timeouts_entries) {
	    if (timevalcmp(timo, &evtmp->timeout, <))
		break;
	}
	if (evtmp != NULL) {
	    TAILQ_INSERT_BEFORE(evtmp, ev, timeouts_entries);
	} else {
	    TAILQ_INSERT_TAIL(&base->timeouts, ev, timeouts_entries);
	}
    }
    /* Clear pending delete so adding from callback works properly. */
    CLR(ev->flags, SUDO_EV_DELETE);
    debug_return_int(0);
}

int
sudo_ev_del(struct sudo_event_base *base, struct sudo_event *ev)
{
    debug_decl(sudo_ev_del, SUDO_DEBUG_EVENT)

    /* Make sure event is really in the queue. */
    if (ev->base == NULL) {
	sudo_debug_printf(SUDO_DEBUG_INFO, "%s: event %p not in queue",
	    __func__, ev);
	debug_return_int(0);
    }

    /* Check for event base mismatch, if one is specified. */
    if (base == NULL) {
	base = ev->base;
    } else if (base != ev->base) {
	sudo_debug_printf(SUDO_DEBUG_ERROR, "%s: mismatch base %p, ev->base %p",
	    __func__, base, ev->base);
	debug_return_int(-1);
    }

    sudo_debug_printf(SUDO_DEBUG_INFO, "%s: removing event %p from base %p",
	__func__, ev, base);

    /* Call backend. */
    if (sudo_ev_del_impl(base, ev) != 0)
	debug_return_int(-1);

    /* Unlink from event list. */
    TAILQ_REMOVE(&base->events, ev, entries);

    /* Unlink from timeouts list. */
    if (ISSET(ev->events, SUDO_EV_TIMEOUT) && timevalisset(&ev->timeout))
	TAILQ_REMOVE(&base->timeouts, ev, timeouts_entries);

    /* Unlink from active list and update base pointers as needed. */
    if (ISSET(ev->flags, SUDO_EV_ACTIVE)) {
	TAILQ_REMOVE(&base->active, ev, active_entries);
	if (ev == base->pending)
	    base->pending = TAILQ_NEXT(ev, active_entries);
	if (ev == base->cur)
	    base->cur = NULL;
    }

    /* Mark event unused. */
    ev->base = NULL;
    ev->flags = 0;
    ev->pfd_idx = -1;

    debug_return_int(0);
}

/*
 * Run main event loop.
 * Returns 0 on success, 1 if no events registered  and -1 on error 
 */
int
sudo_ev_loop(struct sudo_event_base *base, int flags)
{
    struct timeval now;
    struct sudo_event *ev;
    int nready, rc = 0;
    debug_decl(sudo_ev_loop, SUDO_DEBUG_EVENT)

    /*
     * If sudo_ev_loopexit() was called when events were not running
     * the next invocation of sudo_ev_loop() only runs once.
     * All other base flags are ignored unless we are running events.
     */
    if (ISSET(base->flags, SUDO_EVBASE_LOOPEXIT))
	SET(flags, SUDO_EVLOOP_ONCE);
    base->flags = 0;

    for (;;) {
rescan:
	/* Make sure we have some events. */
	if (TAILQ_EMPTY(&base->events)) {
	    rc = 1;
	    break;
	}

	/* Call backend to scan for I/O events. */
	TAILQ_INIT(&base->active);
	nready = sudo_ev_scan_impl(base, flags);
	switch (nready) {
	case -1:
	    if (errno == EINTR || errno == ENOMEM)
		continue;
	    rc = -1;
	    goto done;
	case 0:
	    /* Timed out, activate timeout events. */
	    gettimeofday(&now, NULL);
	    while ((ev = TAILQ_FIRST(&base->timeouts)) != NULL) {
		if (timevalcmp(&ev->timeout, &now, >))
		    break;
		/* Remove from timeouts list. */
		timevalclear(&ev->timeout);
		TAILQ_REMOVE(&base->timeouts, ev, timeouts_entries);
		/* Make event active. */
		ev->revents = SUDO_EV_TIMEOUT;
		SET(ev->flags, SUDO_EV_ACTIVE);
		TAILQ_INSERT_TAIL(&base->active, ev, active_entries);
	    }
	    break;
	default:
	    /* I/O events active, sudo_ev_scan_impl() already added them. */
	    break;
	}

	/*
	 * Service each event in the active queue.
	 * We store the current event pointer in the base so that
	 * it can be cleared by sudo_ev_del().  This prevents a use
	 * after free if the callback frees its own event.
	 */
	TAILQ_FOREACH_SAFE(base->cur, &base->active, active_entries, base->pending) {
	    if (!ISSET(base->cur->events, SUDO_EV_PERSIST))
		SET(base->cur->flags, SUDO_EV_DELETE);
	    base->cur->callback(base->cur->fd, base->cur->revents,
		base->cur->closure == sudo_ev_self_cbarg() ? base->cur : base->cur->closure);
	    if (base->cur != NULL) {
		CLR(base->cur->flags, SUDO_EV_ACTIVE);
		if (ISSET(base->cur->flags, SUDO_EV_DELETE))
		    sudo_ev_del(base, base->cur);
	    }
	    if (ISSET(base->flags, SUDO_EVBASE_LOOPBREAK)) {
		/* stop processing events immediately */
		SET(base->flags, SUDO_EVBASE_GOT_BREAK);
		base->pending = NULL;
		goto done;
	    }
	    if (ISSET(base->flags, SUDO_EVBASE_LOOPCONT)) {
		/* rescan events and start polling again */
		CLR(base->flags, SUDO_EVBASE_LOOPCONT);
		if (!ISSET(flags, SUDO_EVLOOP_ONCE)) {
		    base->pending = NULL;
		    goto rescan;
		}
	    }
	}
	base->pending = NULL;
	if (ISSET(base->flags, SUDO_EVBASE_LOOPEXIT)) {
	    /* exit loop after once through */
	    SET(base->flags, SUDO_EVBASE_GOT_EXIT);
	    goto done;
	}
	if (flags & (SUDO_EVLOOP_ONCE | SUDO_EVLOOP_NONBLOCK))
	    break;
    }
done:
    base->flags &= SUDO_EVBASE_GOT_MASK;
    debug_return_int(rc);
}

void
sudo_ev_loopexit(struct sudo_event_base *base)
{
    debug_decl(sudo_ev_loopexit, SUDO_DEBUG_EVENT)
    SET(base->flags, SUDO_EVBASE_LOOPEXIT);
    debug_return;
}

void
sudo_ev_loopbreak(struct sudo_event_base *base)
{
    debug_decl(sudo_ev_loopbreak, SUDO_DEBUG_EVENT)
    SET(base->flags, SUDO_EVBASE_LOOPBREAK);
    debug_return;
}

void
sudo_ev_loopcontinue(struct sudo_event_base *base)
{
    debug_decl(sudo_ev_loopcontinue, SUDO_DEBUG_EVENT)
    SET(base->flags, SUDO_EVBASE_LOOPCONT);
    debug_return;
}

bool
sudo_ev_got_exit(struct sudo_event_base *base)
{
    debug_decl(sudo_ev_got_exit, SUDO_DEBUG_EVENT)
    debug_return_bool(ISSET(base->flags, SUDO_EVBASE_GOT_EXIT));
}

bool
sudo_ev_got_break(struct sudo_event_base *base)
{
    debug_decl(sudo_ev_got_break, SUDO_DEBUG_EVENT)
    debug_return_bool(ISSET(base->flags, SUDO_EVBASE_GOT_BREAK));
}
