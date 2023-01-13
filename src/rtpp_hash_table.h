/*
 * Copyright (c) 2004-2006 Maxim Sobolev <sobomax@FreeBSD.org>
 * Copyright (c) 2006-2014 Sippy Software, Inc., http://www.sippysoft.com
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

struct rtpp_hash_table;
struct rtpp_hash_table_entry;
struct rtpp_refcnt;

DEFINE_RAW_METHOD(rtpp_hash_table_match, int, void *, void *);

struct rtpp_ht_opstats {
    unsigned first:1;
    unsigned last:1;
};

#if 0
DEFINE_METHOD(rtpp_hash_table, hash_table_append, struct rtpp_hash_table_entry *, const void *, void *);
DEFINE_METHOD(rtpp_hash_table, hash_table_remove_nc, void, struct rtpp_hash_table_entry *sp);
#endif
DEFINE_METHOD(rtpp_hash_table, hash_table_append_refcnt, struct rtpp_hash_table_entry *,
  const void *, struct rtpp_refcnt *, struct rtpp_ht_opstats *);
DEFINE_METHOD(rtpp_hash_table, hash_table_remove, void, const void *key, struct rtpp_hash_table_entry *sp);
DEFINE_METHOD(rtpp_hash_table, hash_table_remove_by_key, struct rtpp_refcnt *,
  const void *key, struct rtpp_ht_opstats *);
#if 0
DEFINE_METHOD(rtpp_hash_table, hash_table_findfirst, struct rtpp_hash_table_entry *,
  const void *key, void **);
DEFINE_METHOD(rtpp_hash_table, hash_table_findnext,  struct rtpp_hash_table_entry *,
  struct rtpp_hash_table_entry *, void **);
#endif
DEFINE_METHOD(rtpp_hash_table, hash_table_find, struct rtpp_refcnt *, const void *);
DEFINE_METHOD(rtpp_hash_table, hash_table_foreach, void, rtpp_hash_table_match_t,
  void *, struct rtpp_ht_opstats *);
DEFINE_METHOD(rtpp_hash_table, hash_table_foreach_key, void, const void *, rtpp_hash_table_match_t, void *);
DEFINE_METHOD(rtpp_hash_table, hash_table_dtor, void);
DEFINE_METHOD(rtpp_hash_table, hash_table_get_length, int);
DEFINE_METHOD(rtpp_hash_table, hash_table_purge, int);

struct rtpp_hash_table_priv;

enum rtpp_ht_key_types {rtpp_ht_key_str_t = 0, rtpp_ht_key_u64_t,
  rtpp_ht_key_u32_t, rtpp_ht_key_u16_t};

#define RTPP_HT_NODUPS    0x1
#define RTPP_HT_DUP_ABRT  0x2

#define RTPP_HT_MATCH_CONT  (0 << 0)
#define RTPP_HT_MATCH_BRK   (1 << 0)
#define RTPP_HT_MATCH_DEL   (1 << 1)

struct rtpp_hash_table
{
#if 0
    METHOD_ENTRY(hash_table_append, append);
    METHOD_ENTRY(hash_table_remove_nc, remove_nc);
#endif
    METHOD_ENTRY(hash_table_append_refcnt, append_refcnt);
    METHOD_ENTRY(hash_table_remove, remove);
    METHOD_ENTRY(hash_table_remove_by_key, remove_by_key);
#if 0
    METHOD_ENTRY(hash_table_findfirst, findfirst);
    METHOD_ENTRY(hash_table_findnext, findnext);
#endif
    METHOD_ENTRY(hash_table_find, find);
    METHOD_ENTRY(hash_table_foreach, foreach);
    METHOD_ENTRY(hash_table_foreach_key, foreach_key);
    METHOD_ENTRY(hash_table_dtor, dtor);
    METHOD_ENTRY(hash_table_get_length, get_length);
    METHOD_ENTRY(hash_table_purge, purge);
};

struct rtpp_hash_table *rtpp_hash_table_ctor(enum rtpp_ht_key_types, int);
