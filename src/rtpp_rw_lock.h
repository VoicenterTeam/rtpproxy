/*
 * Copyright (c) 2023 Sippy Software, Inc., http://www.sippysoft.com
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

#pragma once

struct rtpp_rw_lock;
struct rtpp_refcnt;

enum rtpp_rw_lock_mode {RTPP_RW_LOCK_RD, RTPP_RW_LOCK_WR};

DEFINE_METHOD(rtpp_rw_lock, rtpp_rw_lock_lock, void,
  enum rtpp_rw_lock_mode);
DEFINE_METHOD(rtpp_rw_lock, rtpp_rw_lock_unlock, void,
  enum rtpp_rw_lock_mode);
DEFINE_METHOD(rtpp_rw_lock, rtpp_rw_lock_upgrade, int);

struct rtpp_rw_lock_smethods
{
    METHOD_ENTRY(rtpp_rw_lock_lock, lock);
    METHOD_ENTRY(rtpp_rw_lock_unlock, unlock);
    METHOD_ENTRY(rtpp_rw_lock_upgrade, upgrade);
};

struct rtpp_rw_lock {
    struct rtpp_refcnt *rcnt;
#if defined(RTPP_DEBUG)
    const struct rtpp_rw_lock_smethods * smethods;
#endif
};

struct rtpp_rw_lock *rtpp_rw_lock_ctor(void);
