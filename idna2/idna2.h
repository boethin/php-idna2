/*
 * Copyright (C) 2011 Sebastian Boethin <sebastian@boethin.eu>
 *
 * This file is part of idna2
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef IDNA2_H
#define IDNA2_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* libidn headers */
#include <stringprep.h>
#include <punycode.h>
#include <idna.h>

/* idnkit-2.0 headers */
#include <idn/api.h>
#include <idn/version.h>



/* idna2 flags */
typedef int32_t idna2_flags;

#define IDNA2_CHECK_ASCII    (1L << 0)
#define IDNA2_CHECK_HYPHEN34 (1L << 1)

#define IDNA2_CHECK_HYPHEN   (1L << 2)
#define IDNA2_CHECK_LENGTH   (1L << 3)
#define IDNA2_CHECK_LDH      (1L << 4)
#define IDNA2_CHECK_STD3  \
  (IDNA2_CHECK_HYPHEN | IDNA2_CHECK_LENGTH | IDNA2_CHECK_LDH)

#define IDNA2_CHECK_PROHIBITED  (1L << 5)
#define IDNA2_CHECK_UNASSIGNED  (1L << 6)
#define IDNA2_CHECK_COMBINING   (1L << 7)
#define IDNA2_CHECK_BIDI        (1L << 9)
#define IDNA2_CHECK_NFC         (1L << 10)
#define IDNA2_CHECK_CONTEXTJ    (1L << 13)
#define IDNA2_CHECK_CONTEXTO_REGIST  (1L << 14)
#define IDNA2_CHECK_CONTEXTO_LOOKUP  (1L << 15)


#define IDNA2_CHECK_2008_LOOKUP ( \
    IDNA2_CHECK_HYPHEN34 | \
    IDNA2_CHECK_LENGTH | \
    IDNA2_CHECK_PROHIBITED | \
    IDNA2_CHECK_UNASSIGNED | \
    IDNA2_CHECK_NFC | \
    IDNA2_CHECK_BIDI | \
    IDNA2_CHECK_COMBINING | \
    IDNA2_CHECK_CONTEXTJ | \
    IDNA2_CHECK_CONTEXTO_LOOKUP )

#define IDNA2_CHECK_2008_REGIST ( \
    IDNA2_CHECK_HYPHEN34 | \
    IDNA2_CHECK_LENGTH | \
    IDNA2_CHECK_HYPHEN | \
    IDNA2_CHECK_PROHIBITED | \
    IDNA2_CHECK_UNASSIGNED | \
    IDNA2_CHECK_NFC | \
    IDNA2_CHECK_BIDI | \
    IDNA2_CHECK_COMBINING | \
    IDNA2_CHECK_CONTEXTJ | \
    IDNA2_CHECK_CONTEXTO_REGIST )

#define IDNA2_MAP_NAMEPREP    (1L << 20)
#define IDNA2_MAP_NFKC        (1L << 21)
#define IDNA2_MAP_NFCLOWER    (1L << 22)

#define IDNA2_MAP_PUNYCODE    (1L << 23)

#define IDNA2_MAP_IDNA2003    ( IDNA2_MAP_NAMEPREP | IDNA2_MAP_PUNYCODE )
#define IDNA2_MAP_IDNA2008    ( IDNA2_MAP_NFCLOWER | IDNA2_MAP_PUNYCODE )

#define IDNA2_ENCODING_ERROR  (1L << 29)
#define IDNA2_MALLOC_ERROR    (1L << 30)
#define IDNA2_UNKNOWN_ERROR   (1L << 31)
#define IDNA2_ERROR           ( IDNA2_ENCODING_ERROR | IDNA2_MALLOC_ERROR | IDNA2_UNKNOWN_ERROR )

#define IDNA2_SUCCESS         0L


/* constants */
#define IDNA2_BUFSZ 256


/* UTF-8 */
#define idna2_utf8_encode(ucs4,ucs4_l,utf8)  idna2_libidn_utf8_encode(ucs4,ucs4_l,utf8)
#define idna2_utf8_decode(utf8,ucs4,ucs4_l)  idna2_libidn_utf8_decode(utf8,ucs4,ucs4_l)

idna2_flags
idna2_libidn_utf8_encode
  (const uint32_t *ucs4, size_t ucs4_l, char **utf8);

idna2_flags
idna2_libidn_utf8_decode
  (const char *utf8, uint32_t **ucs4, size_t *ucs4_l);

/* Puncode */
#define idna2_punycode_encode(ucs4,ucs4_l) idna2_libidn_punycode_encode(ucs4,ucs4_l)
#define idna2_punycode_decode(ucs4,ucs4_l) idna2_libidn_punycode_decode(ucs4,ucs4_l)

idna2_flags
idna2_libidn_punycode_encode
  (uint32_t **ucs4, size_t *ucs4_l);

idna2_flags
idna2_libidn_punycode_decode
  (uint32_t **ucs4, size_t *ucs4_l);

idna2_flags
idna2_idnkit2_punycode_decode
  (uint32_t **ucs4, size_t *ucs4_l);


/* Nameprep */
#define idna2_nameprep(ucs4,ucs4_l,flags) idna2_libidn_nameprep (ucs4,ucs4_l,flags)
idna2_flags
idna2_libidn_nameprep
  (uint32_t **ucs4, size_t *ucs4_l, idna2_flags flags);


/* IDNA2008 encoding */
idna2_flags
idna2_idnkit2_encodename
  (uint32_t **ucs4p, size_t *ucs4_l, idn_action_t actions, idn_result_t *res);

idna2_flags
idna2_idnkit2_encodename_utf8
  (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
  idn_action_t actions, idn_result_t *res);

/* IDNA2008 decoding */
idna2_flags
idna2_idnkit2_decodename
  (uint32_t **ucs4p, size_t *ucs4_l, idn_action_t actions, idn_result_t *res);

idna2_flags
idna2_idnkit2_decodename_utf8
  (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
  idn_action_t actions, idn_result_t *res);



/* IDNA2003 ToAscii */
idna2_flags
idna2_libidn_to_ascii
  (uint32_t **ucs4, size_t *ucs4_l, idna2_flags flags);

/* IDNA2003 ToUnicode */
idna2_flags
idna2_libidn_to_unicode
  (uint32_t **ucs4, size_t *ucs4_l, int flags);



/* global message function pointer */
extern void
(*idna2_set_message)(const char *source, const char *message);
#define IDNA2_SET_MESSAGE(s,t) \
  if (NULL != idna2_set_message) { \
    idna2_set_message(s,t); }

const char *
  idna2_idnkit2_errstr(idn_result_t rc);


#endif	/* IDNA2_H */
