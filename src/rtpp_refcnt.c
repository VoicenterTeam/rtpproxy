/*
 * Copyright (c) 2015 Sippy Software, Inc., http://www.sippysoft.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#if defined(LINUX_XXX) && !defined(_GNU_SOURCE)
/* Apparently needed for asprintf(3) */
#define _GNU_SOURCE
#endif

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "rtpp_debug.h"
#include "rtpp_types.h"
#include "rtpp_mallocs.h"
#include "rtpp_refcnt.h"
#include "rtpp_refcnt_fin.h"

#if RTPP_DEBUG_refcnt
#include <stdio.h>
#ifdef RTPP_DEBUG
#include "rtpp_stacktrace.h"
#endif
#endif

static void rtpp_refcnt_incref(struct rtpp_refcnt *);
static void rtpp_refcnt_decref(struct rtpp_refcnt *);

/*
 * Somewhat arbitrary cap on the maximum value of the references. Just here
 * to catch any runaway situations, i.e. bugs in the code.
 */
#define RC_ABS_MAX 2000000

#define RC_FLAG_PA         (1 << 0)
#define RC_FLAG_TRACE      (1 << 1)
#define RC_FLAG_HASDTOR    (1 << 2)
#define RC_FLAG_HASPRDTOR  (1 << 3)
#define RC_FLAG_PA_STDFREE (1 << 4)

struct rtpp_refcnt_priv
{
    struct rtpp_refcnt pub;
    atomic_int cnt;
    rtpp_refcnt_dtor_t dtor_f;
    void *data;
    rtpp_refcnt_dtor_t pre_dtor_f;
    void *pd_data;
    int flags;
};

static void rtpp_refcnt_attach(struct rtpp_refcnt *, rtpp_refcnt_dtor_t,
  void *);
static void *rtpp_refcnt_getdata(struct rtpp_refcnt *);
static void rtpp_refcnt_reg_pd(struct rtpp_refcnt *, rtpp_refcnt_dtor_t,
  void *);
static void rtpp_refcnt_use_stdfree(struct rtpp_refcnt *, void *);

#if RTPP_DEBUG_refcnt
static void rtpp_refcnt_traceen(struct rtpp_refcnt *);
#endif

static const struct rtpp_refcnt_smethods _rtpp_refcnt_smethods = {
    .incref = &rtpp_refcnt_incref,
    .decref = &rtpp_refcnt_decref,
    .getdata = &rtpp_refcnt_getdata,
    .reg_pd = &rtpp_refcnt_reg_pd,
#if RTPP_DEBUG_refcnt
    .traceen = rtpp_refcnt_traceen,
#endif
    .attach = &rtpp_refcnt_attach,
    .use_stdfree = &rtpp_refcnt_use_stdfree,
};
const struct rtpp_refcnt_smethods * const rtpp_refcnt_smethods = &_rtpp_refcnt_smethods;

#if defined(RTPP_CHECK_LEAKS)
static void
rtpp_refcnt_free(void *p)
{

    free(p);
}
#endif

struct rtpp_refcnt *
rtpp_refcnt_ctor(void *data, rtpp_refcnt_dtor_t dtor_f)
{
    struct rtpp_refcnt_priv *pvt;

    pvt = rtpp_zmalloc(sizeof(struct rtpp_refcnt_priv));
    if (pvt == NULL) {
        return (NULL);
    }
    pvt->data = data;
    if (dtor_f != NULL) {
        pvt->dtor_f = dtor_f;
        pvt->flags |= RC_FLAG_HASDTOR;
    }
#if defined(RTPP_DEBUG)
    pvt->pub.smethods = rtpp_refcnt_smethods;
#endif
    atomic_init(&pvt->cnt, 1);
    return (&pvt->pub);
}

const unsigned int
rtpp_refcnt_osize(void)
{

    return (sizeof(struct rtpp_refcnt_priv));
}

struct rtpp_refcnt *
rtpp_refcnt_ctor_pa(void *pap)
{
    struct rtpp_refcnt_priv *pvt;

    pvt = (struct rtpp_refcnt_priv *)pap;
#if defined(RTPP_DEBUG)
    pvt->pub.smethods = rtpp_refcnt_smethods;
#endif
    atomic_init(&pvt->cnt, 1);
    pvt->flags |= RC_FLAG_PA;
    return (&pvt->pub);
}

static void
rtpp_refcnt_attach(struct rtpp_refcnt *pub, rtpp_refcnt_dtor_t dtor_f,
  void *data)
{
    struct rtpp_refcnt_priv *pvt;

    PUB2PVT(pub, pvt);
    RTPP_DBG_ASSERT(pvt->data == NULL && pvt->dtor_f == NULL &&
      !(pvt->flags & RC_FLAG_HASDTOR));
    pvt->data = data;
    pvt->dtor_f = dtor_f;
    pvt->flags |= RC_FLAG_HASDTOR;
}

static void
rtpp_refcnt_incref(struct rtpp_refcnt *pub)
{
    struct rtpp_refcnt_priv *pvt;

    PUB2PVT(pub, pvt);
#if RTPP_DEBUG_refcnt
    if (pvt->flags & RC_FLAG_TRACE) {
        char *dbuf;
        asprintf(&dbuf, "rtpp_refcnt(%p, %u).incref()", pub,
          atomic_load(&pvt->cnt));
        if (dbuf != NULL) {
#ifdef RTPP_DEBUG
            rtpp_stacktrace_print(dbuf);
#else
            fprintf(stderr, "%s\n", dbuf);
#endif
            free(dbuf);
        }
    }
#endif
    RTPP_DBG_ASSERT(atomic_load(&pvt->cnt) > 0 && atomic_load(&pvt->cnt) < RC_ABS_MAX);
    atomic_fetch_add_explicit(&pvt->cnt, 1, memory_order_relaxed);
}

static void
rtpp_refcnt_decref(struct rtpp_refcnt *pub)
{
    struct rtpp_refcnt_priv *pvt;
    int oldcnt;

    PUB2PVT(pub, pvt);
    oldcnt = atomic_fetch_sub_explicit(&pvt->cnt, 1, memory_order_release);
#if RTPP_DEBUG_refcnt
    if (pvt->flags & RC_FLAG_TRACE) {
        char *dbuf;
        asprintf(&dbuf, "rtpp_refcnt(%p, %u).decref()", pub, oldcnt);
        if (dbuf != NULL) {
#ifdef RTPP_DEBUG
            rtpp_stacktrace_print(dbuf);
#else
            fprintf(stderr, "%s\n", dbuf);
#endif
            free(dbuf);
        }
    }
#endif
    if (oldcnt == 1) {
        atomic_thread_fence(memory_order_acquire);
        int flags = pvt->flags;
        if ((flags & RC_FLAG_PA) == 0) {
            if (flags & RC_FLAG_HASPRDTOR) {
                pvt->pre_dtor_f(pvt->pd_data);
            }
            if (flags & RC_FLAG_HASDTOR) {
                pvt->dtor_f(pvt->data);
            } else {
#if !defined(RTPP_CHECK_LEAKS)
                free(pvt->data);
#else
                rtpp_refcnt_free(pvt->data);
#endif
            }
            rtpp_refcnt_fin(pub);
            free(pvt);
        } else {
            rtpp_refcnt_fin(pub);
            if (flags & RC_FLAG_HASPRDTOR) {
                pvt->pre_dtor_f(pvt->pd_data);
            }
            if (flags & RC_FLAG_HASDTOR) {
                pvt->dtor_f(pvt->data);
            }
            if (flags & RC_FLAG_PA_STDFREE) {
#if !defined(RTPP_CHECK_LEAKS)
                free(pvt->data);
#else
                rtpp_refcnt_free(pvt->data);
#endif
            }
        }

        return;
    }
}

static void *
rtpp_refcnt_getdata(struct rtpp_refcnt *pub)
{
    struct rtpp_refcnt_priv *pvt;

    PUB2PVT(pub, pvt);
    RTPP_DBG_ASSERT(atomic_load(&pvt->cnt) > 0);
    return (pvt->data);
}

static void
rtpp_refcnt_reg_pd(struct rtpp_refcnt *pub, rtpp_refcnt_dtor_t pre_dtor_f,
  void *pd_data)
{
    struct rtpp_refcnt_priv *pvt;

    PUB2PVT(pub, pvt);
    RTPP_DBG_ASSERT(pvt->pd_data == NULL && pvt->pre_dtor_f == NULL &&
      !(pvt->flags & RC_FLAG_HASPRDTOR));
    RTPP_DBG_ASSERT(pvt->pre_dtor_f == NULL);
    pvt->pre_dtor_f = pre_dtor_f;
    pvt->pd_data = pd_data;
    pvt->flags |= RC_FLAG_HASPRDTOR;
}

#if RTPP_DEBUG_refcnt
static void
rtpp_refcnt_traceen(struct rtpp_refcnt *pub)
{
    struct rtpp_refcnt_priv *pvt;

    PUB2PVT(pub, pvt);
    pvt->flags |= RC_FLAG_TRACE;
}
#endif

static void
rtpp_refcnt_use_stdfree(struct rtpp_refcnt *pub, void *data)
{
    struct rtpp_refcnt_priv *pvt;

    PUB2PVT(pub, pvt);
    RTPP_DBG_ASSERT(pvt->data == NULL && (pvt->flags & RC_FLAG_PA) &&
      !(pvt->flags & RC_FLAG_PA_STDFREE));
    pvt->flags |= RC_FLAG_PA_STDFREE;
    pvt->data = data;
}
