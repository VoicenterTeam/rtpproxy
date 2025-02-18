/*
 * Copyright (c) 2004-2006 Maxim Sobolev <sobomax@FreeBSD.org>
 * Copyright (c) 2006-2015 Sippy Software, Inc., http://www.sippysoft.com
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

struct rtpp_weakref;
struct rtpp_refcnt;

#define RTPP_WR_MATCH_BRK  RTPP_HT_MATCH_BRK
#define RTPP_WR_MATCH_CONT RTPP_HT_MATCH_CONT
#define RTPP_WR_MATCH_DEL  RTPP_HT_MATCH_DEL

DEFINE_RAW_METHOD(rtpp_weakref_foreach, int, void *, void *);
DEFINE_RAW_METHOD(rtpp_weakref_cb, void, void *);

DEFINE_METHOD(rtpp_weakref, rtpp_wref_reg, int,
  struct rtpp_refcnt *, uint64_t);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_unreg, struct rtpp_refcnt *,
  uint64_t);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_move, struct rtpp_refcnt *,
  uint64_t, struct rtpp_weakref *);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_get_by_idx, void *,
  uint64_t);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_foreach, void,
  rtpp_weakref_foreach_t, void *);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_get_length, int);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_purge, int);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_set_on_first,
  rtpp_weakref_cb_t, rtpp_weakref_cb_t, void *);
DEFINE_METHOD(rtpp_weakref, rtpp_wref_set_on_last,
  rtpp_weakref_cb_t, rtpp_weakref_cb_t, void *);

struct rtpp_weakref_smethods
{
    METHOD_ENTRY(rtpp_wref_reg, reg);
    METHOD_ENTRY(rtpp_wref_unreg, unreg);
    METHOD_ENTRY(rtpp_wref_move, move);
    METHOD_ENTRY(rtpp_wref_get_by_idx, get_by_idx);
    METHOD_ENTRY(rtpp_wref_foreach, foreach);
    METHOD_ENTRY(rtpp_wref_get_length, get_length);
    METHOD_ENTRY(rtpp_wref_purge, purge);
    METHOD_ENTRY(rtpp_wref_set_on_first, set_on_first);
    METHOD_ENTRY(rtpp_wref_set_on_last, set_on_last);
};

struct rtpp_weakref
{
    struct rtpp_refcnt *rcnt;
    struct rtpp_hash_table *ht;
#if defined(RTPP_DEBUG)
    const struct rtpp_weakref_smethods * smethods;
#endif
};

struct rtpp_weakref *rtpp_weakref_ctor(void);
