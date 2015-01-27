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

/* libidn wrappers */

idna2_flags
idna2_libidn_utf8_encode
  (const uint32_t *ucs4, size_t ucs4_l, char **utf8)
{

  /* Convert a string from a 32-bit fixed width representation as UCS-4. to UTF-8.
  The result will be terminated with a 0 byte.
   char * stringprep_ucs4_to_utf8
     (const uint32_t * str, ssize_t len, size_t * items_read, size_t * items_written) */
  if (!(*utf8 = stringprep_ucs4_to_utf8(ucs4, ucs4_l, NULL, NULL)))
    return IDNA2_ENCODING_ERROR;

  return IDNA2_SUCCESS;
}

idna2_flags
idna2_libidn_utf8_decode
  (const char *utf8, uint32_t **ucs4, size_t *ucs4_l)
{

  /* Convert a string from UTF-8 to a 32-bit fixed width representation as UCS-4,
  assuming valid UTF-8 input. This function does no error checking on the input.
   uint32_t * stringprep_utf8_to_ucs4
    (const char * str, ssize_t len, size_t * items_written) */
	if(!(*ucs4 = stringprep_utf8_to_ucs4(utf8, -1, ucs4_l)))
    return IDNA2_ENCODING_ERROR;

  return IDNA2_SUCCESS;
}


idna2_flags
idna2_libidn_punycode_encode
  (uint32_t **ucs4, size_t *ucs4_l)
{
  int rc;
  char *ascii = NULL;
  uint32_t *ucs4_t;
  size_t ascii_l = 0, ascii_addl = IDNA2_BUFSZ;

  if (ascii_addl < *ucs4_l)
    ascii_addl = *ucs4_l + IDNA2_BUFSZ;

  /*  int punycode_encode
    (size_t input_length, const punycode_uint [] input, const unsigned char [] case_flags,
      size_t * output_length, char [] output); */
  do
  {
    ascii_l += ascii_addl, ascii_addl += IDNA2_BUFSZ;
    if (!(ascii = (char *)realloc(ascii, ascii_l*sizeof(char))))
      return IDNA2_MALLOC_ERROR;
  }
  while (PUNYCODE_BIG_OUTPUT == (rc = punycode_encode
    (*ucs4_l, (punycode_uint*)*ucs4, NULL, &ascii_l, ascii)) );

  if (PUNYCODE_SUCCESS != rc)
  {
    free(ascii);
    IDNA2_SET_MESSAGE("punycode_encode", punycode_strerror (rc));

    /* map #Punycode_status to error codes */
    switch (rc)
    {
      case PUNYCODE_BAD_INPUT:
      case PUNYCODE_OVERFLOW:
        return IDNA2_ENCODING_ERROR;
      default:
        return IDNA2_UNKNOWN_ERROR;
    }
  }
  /* (PUNYCODE_SUCCESS == rc) */

  /* get back to uint32_t */
  if (!(ucs4_t = (uint32_t *)malloc(ascii_l*sizeof(uint32_t))))
    return IDNA2_MALLOC_ERROR;
  {
    register int i = 0;
    for ( ; i < ascii_l; i++)
      ucs4_t[i] = (uint32_t)ascii[i];
  }
  (*ucs4 = ucs4_t, *ucs4_l = ascii_l);

  free(ascii);
  return IDNA2_SUCCESS;
}

idna2_flags
idna2_libidn_punycode_decode
  (uint32_t **ucs4, size_t *ucs4_l)
{
  int rc;
  char *ascii;
  
  if (!(ascii = (char *)malloc(*ucs4_l*sizeof(char))))
    return IDNA2_MALLOC_ERROR;
  /* verify ASCII input */
  {
    register int i = 0;
    while (i < *ucs4_l)
    {
      if (!(0 < (*ucs4)[i] && (*ucs4)[i] < 0x80))
      {
        free(ascii);
        IDNA2_SET_MESSAGE("punycode_decode", "Input contains non-ASCII characters");
        return IDNA2_ENCODING_ERROR;
      }
      ascii[i] = (char)((*ucs4)[i++] & 0x7F);
    }
  }
    
  /* int punycode_decode
    (size_t input_length, const char [] input, size_t * output_length,
    punycode_uint [] output, unsigned char [] case_flags) */
  if (PUNYCODE_SUCCESS != (rc = punycode_decode
    (*ucs4_l, ascii, ucs4_l, (punycode_uint *)(*ucs4), NULL)))
  {
    free(ascii);
    IDNA2_SET_MESSAGE("punycode_decode", punycode_strerror (rc));

    /* map #Punycode_status to error codes */
    switch (rc)
    {
      case PUNYCODE_BAD_INPUT:
      case PUNYCODE_OVERFLOW:
        return IDNA2_ENCODING_ERROR;
      default:
        return IDNA2_UNKNOWN_ERROR;
    }
  }

  return IDNA2_SUCCESS;
}

idna2_flags
idna2_libidn_nameprep
  (uint32_t **ucs4, size_t *ucs4_l, idna2_flags flags)
{
  int rc;
  uint32_t *ucs4_t = *ucs4;
  size_t ucs4_maxl, ucs4_addl = IDNA2_BUFSZ;
  Stringprep_profile_flags f = ((flags & IDNA2_CHECK_UNASSIGNED)?
    STRINGPREP_NO_UNASSIGNED : 0);

  /* int stringprep_4i
      (uint32_t * ucs4, size_t * len, size_t maxucs4len, Stringprep_profile_flags flags,
      const Stringprep_profile * profile)
    */
  for (ucs4_maxl = *ucs4_l;
    STRINGPREP_TOO_SMALL_BUFFER == (
      rc = stringprep_4i (ucs4_t,ucs4_l,ucs4_maxl,f,stringprep_nameprep)); )
  {
    ucs4_maxl += ucs4_addl, ucs4_addl += IDNA2_BUFSZ;
    if (!(ucs4_t = realloc(*ucs4, ucs4_maxl*sizeof(uint32_t))))
      return IDNA2_MALLOC_ERROR;
  }
  *ucs4 = ucs4_t;

  if (STRINGPREP_OK != rc)
  {
    IDNA2_SET_MESSAGE("stringprep", stringprep_strerror(rc));

    /* map #Stringprep_rc error codes */
    switch (rc)
    {
      case STRINGPREP_CONTAINS_UNASSIGNED:
        return IDNA2_CHECK_UNASSIGNED;
      case STRINGPREP_CONTAINS_PROHIBITED:
        return IDNA2_CHECK_PROHIBITED;
      case STRINGPREP_BIDI_BOTH_L_AND_RAL:
      case STRINGPREP_BIDI_LEADTRAIL_NOT_RAL:
        return IDNA2_CHECK_BIDI;
        break;
      case STRINGPREP_BIDI_CONTAINS_PROHIBITED:
        return (IDNA2_CHECK_BIDI | IDNA2_CHECK_PROHIBITED);
      case STRINGPREP_MALLOC_ERROR:
        return IDNA2_MALLOC_ERROR;
      default:
        return IDNA2_UNKNOWN_ERROR;
    }
  }
  
  return IDNA2_SUCCESS;
}

idna2_flags
idna2_libidn_to_ascii
  (uint32_t **ucs4, size_t *ucs4_l, int flags)
{
  int rc;
  uint32_t *ucs4_t;
  char *ascii = NULL;
  size_t ascii_l = 0;

  /* int idna_to_ascii_4z
  (const uint32_t * input, char ** output, int flags)
    */
  *(*ucs4 + *ucs4_l) = 0;
  if (IDNA_SUCCESS != (rc = idna_to_ascii_4z(*ucs4,&ascii,flags)))
  {
    free(ascii);
    IDNA2_SET_MESSAGE("to_ascii", idna_strerror(rc));
    switch (rc)
    {
      case IDNA_PUNYCODE_ERROR:
      case IDNA_ROUNDTRIP_VERIFY_ERROR:
        return IDNA2_ENCODING_ERROR;
      case IDNA_STRINGPREP_ERROR:
        return IDNA2_CHECK_UNASSIGNED;
      case IDNA_CONTAINS_ACE_PREFIX:
        return IDNA2_CHECK_HYPHEN34;
      case IDNA_CONTAINS_NON_LDH:
        return IDNA2_CHECK_LDH;
      case IDNA_CONTAINS_MINUS:
        return IDNA2_CHECK_HYPHEN;
      case IDNA_INVALID_LENGTH:
        return IDNA2_CHECK_LENGTH;
      case IDNA_MALLOC_ERROR:
        return IDNA2_MALLOC_ERROR;
      default:
        return IDNA2_UNKNOWN_ERROR;
    }
  }
  
  for (; *(ascii + ascii_l); ascii_l++);
  if (!(ucs4_t = (uint32_t *)malloc(sizeof(uint32_t)*ascii_l)))
  {
    free(ascii);
    return IDNA2_MALLOC_ERROR;
  }

  for (ascii_l = 0; *(ascii + ascii_l); ascii_l++)
    *(ucs4_t + ascii_l) = (uint32_t)*(ascii + ascii_l);
  free(*ucs4), *ucs4 = ucs4_t, *ucs4_l = ascii_l;

  free(ascii);
  return IDNA2_SUCCESS;
}

idna2_flags
idna2_libidn_to_unicode
  (uint32_t **ucs4, size_t *ucs4_l, int flags)
{
  int rc;
  uint32_t *ucs4_t = NULL;
  size_t unicode_l = 0;

  /* int idna_to_unicode_4z4z
  (const uint32_t * input, uint32_t ** output, int flags)
    */
  *(*ucs4 + *ucs4_l) = 0;
  if (IDNA_SUCCESS != (rc = idna_to_unicode_4z4z(*ucs4,&ucs4_t,flags)))
  {
    IDNA2_SET_MESSAGE("to_unicode", idna_strerror(rc));
    switch (rc)
    {
      case IDNA_PUNYCODE_ERROR:
      case IDNA_ROUNDTRIP_VERIFY_ERROR:
      case IDNA_NO_ACE_PREFIX:
        return IDNA2_ENCODING_ERROR;
      case IDNA_STRINGPREP_ERROR:
        return IDNA2_CHECK_UNASSIGNED;
      case IDNA_CONTAINS_NON_LDH:
        return IDNA2_CHECK_LDH;
      case IDNA_CONTAINS_MINUS:
        return IDNA2_CHECK_HYPHEN;
      case IDNA_INVALID_LENGTH:
        return IDNA2_CHECK_LENGTH;
      case IDNA_MALLOC_ERROR:
        return IDNA2_MALLOC_ERROR;
      default:
        return IDNA2_UNKNOWN_ERROR;
    }
  }

  for (; *(ucs4_t + unicode_l); unicode_l++);
  free(*ucs4), *ucs4 = ucs4_t, *ucs4_l = unicode_l;

  return IDNA2_SUCCESS;
}



