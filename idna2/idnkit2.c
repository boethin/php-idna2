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

#include "idna2.h"

idna2_flags
idna2_idnkit2_encodename
  (uint32_t **ucs4p, size_t *ucs4_l, idn_action_t actions, idn_result_t *res)
{
  idna2_flags r;
  char *utf8 = NULL;

  if (IDNA2_ERROR & (r = idna2_utf8_encode(*ucs4p, *ucs4_l, &utf8)))
  {
    return r; /* encoding failed */
  }

  /* idna2_flags
    idna2_idnkit2_encodename_utf8
    (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
      idn_action_t actions, idn_result_t *res);
  */
  r = idna2_idnkit2_encodename_utf8(utf8, ucs4p, ucs4_l, actions, res);
  free(utf8);

  return r;
}

idna2_flags
idna2_idnkit2_encodename_utf8
  (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
    idn_action_t actions, idn_result_t *res)
{
  char *utf8_to;
  idna2_flags r;
  size_t utf8_l = IDNA2_BUFSZ, utf8_addl = IDNA2_BUFSZ;

  if (!(utf8_to = (char *)malloc(utf8_l*sizeof(char))))
    return IDNA2_MALLOC_ERROR;

  /*
  The function encodes a domain name 'from' and writes the result on 'to',
  at most 'tolen' bytes. Note that 'from' must be terminated by NUL, and
  'tolen' includes room for a NUL character.
    idn_result_t
      idn_encodename(idn_action_t actions, const char *from, char *to, size_t tolen); */
  while (idn_buffer_overflow == (*res = idn_encodename(actions, utf8in, utf8_to, utf8_l)))
  {
    utf8_l += utf8_addl, utf8_addl += IDNA2_BUFSZ;
    if (!(utf8_to = (char *)realloc(utf8_to, utf8_l*sizeof(char))))
      return IDNA2_MALLOC_ERROR;
  }

  if (*res != idn_success)
  {
    /* idn_encodename failed */
    return (*res == idn_nomemory) ?
      IDNA2_MALLOC_ERROR : IDNA2_UNKNOWN_ERROR;
  }

  /* utf8 result back to ucs4 */
  if (IDNA2_ERROR & (r = idna2_utf8_decode(utf8_to, ucs4p, ucs4_l)))
  {
    free(utf8_to);
    return r;
  }

  free(utf8_to);
  return IDNA2_SUCCESS;
}

idna2_flags
idna2_idnkit2_decodename
  (uint32_t **ucs4p, size_t *ucs4_l, idn_action_t actions, idn_result_t *res)
{
  idna2_flags r;
  char *utf8 = NULL;

  if (IDNA2_ERROR & (r = idna2_utf8_encode(*ucs4p, *ucs4_l, &utf8)))
  {
    return r; /* encoding failed */
  }

  /* idna2_flags
    idna2_idnkit2_encodename_utf8
    (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
      idn_action_t actions, idn_result_t *res);
  */
  r = idna2_idnkit2_decodename_utf8(utf8, ucs4p, ucs4_l, actions, res);
  free(utf8);

  return r;
}

idna2_flags
idna2_idnkit2_decodename_utf8
  (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
    idn_action_t actions, idn_result_t *res)
{
  char *utf8_to;
  idna2_flags r;
  size_t utf8_l = IDNA2_BUFSZ, utf8_addl = IDNA2_BUFSZ;

  if (!(utf8_to = (char *)malloc(utf8_l*sizeof(char))))
    return IDNA2_MALLOC_ERROR;

  /*
  The function encodes a domain name 'from' and writes the result on 'to',
  at most 'tolen' bytes. Note that 'from' must be terminated by NUL, and
  'tolen' includes room for a NUL character.
    idn_result_t
      idn_encodename(idn_action_t actions, const char *from, char *to, size_t tolen); */
  while (idn_buffer_overflow == (*res = idn_decodename(actions, utf8in, utf8_to, utf8_l)))
  {
    utf8_l += utf8_addl, utf8_addl += IDNA2_BUFSZ;
    if (!(utf8_to = (char *)realloc(utf8_to, utf8_l*sizeof(char))))
      return IDNA2_MALLOC_ERROR;
  }

  if (*res != idn_success)
  {
    /* idn_encodename failed */
    return (*res == idn_nomemory) ?
      IDNA2_MALLOC_ERROR : IDNA2_UNKNOWN_ERROR;
  }

  /* utf8 result back to ucs4 */
  if (IDNA2_ERROR & (r = idna2_utf8_decode(utf8_to, ucs4p, ucs4_l)))
  {
    free(utf8_to);
    return r;
  }

  free(utf8_to);
  return IDNA2_SUCCESS;
}



const char *
  idna2_idnkit2_errstr(idn_result_t rc)
/*
 *   idn_success            -- Ok.
 *   idn_invalid_action	    -- Invalid action flag specified.
 *   idn_invalid_encoding   -- The given string has invalid byte sequence.
 *   idn_buffer_overflow    -- 'tolen' is too small.
 *   idn_nomemory           -- malloc() failed.
 *   idn_nomapping          -- At IDN_LOCALCONV or IDN_UNICODECONV, the
 *                             corresponding character doesn't exist in
 *                             the destination encoding.
 *   idn_prohcheck_error    -- IDN_PROHCHECK failed.
 *   idn_unascheck_error    -- IDN_UNASCHECK failed.
 *   idn_nfccheck_error     -- IDN_NFCCHECK failed.
 *   idn_prefcheck_error    -- IDN_PREFCHECK failed.
 *   idn_hyphcheck_error    -- IDN_HYPHCHECK failed.
 *   idn_combcheck_error    -- IDN_COMBCHECK failed.
 *   idn_ctxjcheck_error    -- IDN_CTXJCHECK failed.
 *   idn_ctxocheck_error    -- IDN_CTXOCHECK or IDN_CTXOLITECHECK failed.
 *   idn_bidicheck_error    -- IDN_BIDICHECK failed.
 *   idn_localcheck_error   -- IDN_LOCALCHECK failed.
 *   idn_lencheck_error     -- IDN_LENCHECK failed.
 *   idn_rtcheck_error      -- IDN_RTCHECK failed.
 */
{
  switch (rc)
  {
    case idn_invalid_action:
      return "Invalid action flag specified.";
    case idn_invalid_encoding:
      return "The given string has invalid byte sequence.";
    case idn_buffer_overflow:
      return "'tolen' is too small.";
    case idn_nomemory:
      return "malloc() failed.";
    case idn_prohcheck_error:
      return "IDNKIT2_PROHCHECK failed.";
    case idn_unascheck_error:
      return "IDNKIT2_UNASCHECK failed.";
    case idn_nfccheck_error:
      return "IDNKIT2_NFCCHECK failed.";
    case idn_prefcheck_error:
      return "IDNKIT2_PREFCHECK failed.";
    case idn_hyphcheck_error:
      return "IDNKIT2_HYPHCHECK failed.";
    case idn_combcheck_error:
      return "IDNKIT2_COMBCHECK failed.";
    case idn_ctxjcheck_error:
      return "IDNKIT2_CTXJCHECK failed.";
    case idn_ctxocheck_error:
      return "IDNKIT2_CTXOCHECK or IDNKIT2_CTXOLITECHECK failed.";
    case idn_bidicheck_error:
      return "IDNKIT2_BIDICHECK failed.";
    case idn_localcheck_error:
      return "IDNKIT2_LOCALCHECK failed.";
    case idn_lencheck_error:
      return "IDNKIT2_LENCHECK failed.";
    case idn_rtcheck_error:
      return "IDNKIT2_RTCHECK failed.";
  }
  return "Unknown error.";
}
