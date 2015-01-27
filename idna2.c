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

#include "php.h"
#include "php_ini.h"
/* #include "ext/standard/info.h" */
#include "php_idna2.h"

#include "idna2/idna2.h"

/* If you declare any globals in php_idna2.h uncomment this:
*/
ZEND_DECLARE_MODULE_GLOBALS(idna2)


/* True global resources - no need for thread safety here */
/*
static int le_idna2;
*/

/* {{{ idna2_functions[]
 *
 * Every user visible function must have an entry in idna2_functions[].
 */
zend_function_entry idna2_functions[] = {
	/* PHP_FE(confirm_idna2_compiled,	NULL)	*/	/* For testing, remove later. */
  PHP_FE(idna2_error, NULL)
  PHP_FE(idna2_test, NULL)
  PHP_FE(idna2_from_utf8, NULL)
  PHP_FE(idna2_to_utf8, NULL)
  PHP_FE(idna2_labels, NULL)
  PHP_FE(idna2_punycode_encode, NULL)
  PHP_FE(idna2_punycode_decode, NULL)
  PHP_FE(idna2_nameprep, NULL)
  PHP_FE(idna2_to_ascii, NULL)
  PHP_FE(idna2_to_unicode, NULL)
  PHP_FE(idna2_encode, NULL)
  PHP_FE(idna2_decode, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in idna2_functions[] */
};
/* }}} */

/* {{{ idna2_module_entry
 */
zend_module_entry idna2_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"idna2",
	idna2_functions,
	PHP_MINIT(idna2),
	PHP_MSHUTDOWN(idna2),
	PHP_RINIT(idna2),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(idna2),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(idna2),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_IDNA2_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_IDNA2
ZEND_GET_MODULE(idna2)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("idna2.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_idna2_globals, idna2_globals)
    STD_PHP_INI_ENTRY("idna2.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_idna2_globals, idna2_globals)
PHP_INI_END()
*/
/* }}} */


#define IDNKIT2_CHECK_FLAGS ( \
  IDNA2_CHECK_PROHIBITED | \
  IDNA2_CHECK_UNASSIGNED | \
  IDNA2_CHECK_COMBINING | \
  IDNA2_CHECK_BIDI | \
  IDNA2_CHECK_NFC | \
  IDNA2_CHECK_CONTEXTJ | \
  IDNA2_CHECK_CONTEXTO_REGIST | \
  IDNA2_CHECK_CONTEXTO_LOOKUP )


/* local macros */
#define IDNA_ERROR_RESET \
  *IDNA2_G(idna2_error) = *IDNA2_G(php_idna2_message) = '\0'
#define IDNA_ERROR_SET(src,msg) { \
  sprintf(IDNA2_G(idna2_error), "(%s) %s", src, msg); \
  RETURN_FALSE; }
#define PHP_MEMFAILURE { \
  php_error(E_ERROR,"idna2: memory allocation failed"); \
  RETURN_NULL(); }

#define IDNA2_HANDLE_ERROR(src,r) { \
  switch(r) \
  { \
    case IDNA2_MALLOC_ERROR: \
      PHP_MEMFAILURE; \
      break; \
    case IDNA2_ENCODING_ERROR: \
      IDNA_ERROR_SET(src,"encoding failed"); \
      break; \
    case IDNA2_UNKNOWN_ERROR: \
      IDNA_ERROR_SET(src,"unexpected error"); \
      break; \
  } \
}


#define HANDLE_IDNA_MALLOC_ERROR(r) \
  if (r==IDNA_MALLOC_ERROR) \
    PHP_MEMFAILURE;

/* static */
static idna2_flags
  __idna2_check_label
  (uint32_t *label, size_t label_l, long flags, idn_result_t *res);

static void
php_idna2_set_message(const char *source, const char *message);





#if _DEBUG_
void zend_printf_ucs4(const uint32_t *ucs4, size_t ucs4_l)
{
  int i;
  char buf[0x4000];
  buf[0] = '\0';
  for (i = 0; i < ucs4_l; i++)
  {
    char buf2[0x100];
    if (0x20 <= ucs4[i] && ucs4[i] < 0x7F)
      sprintf(buf2,"'%c'", (char)ucs4[i]);
    else
      sprintf(buf2,"U+%04X", ucs4[i]);
    if (*buf) strcat(buf," ");
    strcat(buf,buf2);
  }
  zend_printf("ucs4:%s\n", buf);
}
#endif



/* {{{ php_idna2_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_idna2_init_globals(zend_idna2_globals *idna2_globals)
{
	idna2_globals->global_value = 0;
	idna2_globals->global_string = NULL;
}
*/
/* }}} */

static void php_idna2_init_globals(zend_idna2_globals *idna2_globals)
{
  //*IDNA2_G(idna2_error) = '\0';
  //*IDNA2_G(php_idna2_message) = '\0';


}


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(idna2)
{

  ZEND_INIT_MODULE_GLOBALS(idna2, php_idna2_init_globals, NULL);

	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

  /* create php constants */
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_ASCII", IDNA2_CHECK_ASCII, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_HYPHEN34", IDNA2_CHECK_HYPHEN34, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_HYPHEN", IDNA2_CHECK_HYPHEN, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_LENGTH", IDNA2_CHECK_LENGTH, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_LDH", IDNA2_CHECK_LDH, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_STD3", IDNA2_CHECK_STD3, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_PROHIBITED", IDNA2_CHECK_PROHIBITED, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_UNASSIGNED", IDNA2_CHECK_UNASSIGNED, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_COMBINING", IDNA2_CHECK_COMBINING, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_BIDI", IDNA2_CHECK_BIDI, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_NFC", IDNA2_CHECK_NFC, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_CONTEXTJ", IDNA2_CHECK_CONTEXTJ, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_CONTEXTO_REGIST", IDNA2_CHECK_CONTEXTO_REGIST, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_CONTEXTO_LOOKUP", IDNA2_CHECK_CONTEXTO_LOOKUP, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_2008_LOOKUP", IDNA2_CHECK_2008_LOOKUP, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_CHECK_2008_REGIST", IDNA2_CHECK_2008_REGIST, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_MAP_NAMEPREP", IDNA2_MAP_NAMEPREP, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_MAP_NFKC", IDNA2_MAP_NFKC, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_MAP_NFCLOWER", IDNA2_MAP_NFCLOWER, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_MAP_PUNYCODE", IDNA2_MAP_PUNYCODE, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_MAP_IDNA2003", IDNA2_MAP_IDNA2003, CONST_CS | CONST_PERSISTENT);
  REGISTER_LONG_CONSTANT("IDNA2_MAP_IDNA2008", IDNA2_MAP_IDNA2008, CONST_CS | CONST_PERSISTENT);

  /* set global message function pointer */
  idna2_set_message = php_idna2_set_message;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(idna2)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(idna2)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(idna2)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(idna2)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "idna2 support", "enabled");
	php_info_print_table_row(2, "idna2 version", PHP_IDNA2_VERSION);
	php_info_print_table_row(2, "GNU libidn version", STRINGPREP_VERSION);
	php_info_print_table_end();
	
	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_idna2_compiled(string arg)
   Return a string to confirm that the module is compiled in */
   /*
PHP_FUNCTION(confirm_idna2_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "idna2", arg);
	RETURN_STRINGL(strg, len, 0);
} */
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ proto string idna2_error()
   Get error message from last function call */
PHP_FUNCTION( idna2_error )
{
  if (*IDNA2_G(idna2_error))
  {
    char *ret, buf[IDNA2_ZEND_GLOBAL_BUFSZ];
    if (*IDNA2_G(php_idna2_message))
      sprintf(buf, "%s: %s", IDNA2_G(idna2_error), IDNA2_G(php_idna2_message)), ret = buf;
    else
      ret = IDNA2_G(idna2_error);

    //RETVAL_STRING(IDNA2_G(idna2_error), 1);
    RETVAL_STRING(ret, 1);
  }
  else
  {
    RETURN_FALSE;
  }
	return;
}
/* }}} */

/* {{{ proto array idna2_from_utf8(string arg)
  Split a domain name into labels. */
PHP_FUNCTION( idna2_labels )
{
	char *in;
	idna2_flags r;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_l))
	{
    RETURN_NULL();
  }
  //in[in_l] = '\0';

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_labels",r);
  }

  /* create PHP array */
	array_init(return_value);
	{
    size_t i, st = 0;
    for (i = 0; i <= ucs4_l; i++)
    {
      if (i == ucs4_l || (
        /* punctuation characters */
        ucs4[i] == 0x2E || ucs4[i] == 0x3002 ||
        ucs4[i] == 0xFF0E || ucs4[i] == 0xFF61 ))
      {
        if (i > st)
        {
          char *label = NULL;
          if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4 + st, i - st, &label)))
          {
            free(ucs4);
            IDNA_ERROR_SET("idna2_labels", "encoding failed.");
          }
        
          add_next_index_string(return_value, label,1);
          free(label);
        }
        st = i + 1;
      }
    }

  }

	free(ucs4);
	return;
}
/* }}} */


/* {{{ proto array idna2_from_utf8(string arg)
   Convert UTF-8 encoded string to an an array of Unicode codepoints */
PHP_FUNCTION( idna2_from_utf8 )
{
	char *in;
	idna2_flags r;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_l))
	{
    RETURN_NULL();
  }
  //in[in_l] = '\0';

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_from_utf8",r);
  }

  /* create PHP array */
	array_init(return_value);
	{
    int i;
    for (i = 0; i < ucs4_l; i++)
      add_next_index_long(return_value, *(ucs4 + i));
  }

	free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_from_utf8(array codepoints)
   Get UTF-8 encoded string from an array of Unicode codepoints.  */
PHP_FUNCTION( idna2_to_utf8 )
{
  char *ret = NULL;
  int in_bad = 0;
	idna2_flags r;
	size_t array_c, ucs4_l = 0;
  uint32_t *ucs4;
  zval *arr, **data;
  HashTable *arr_hash;
  HashPosition p;

  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr))
	{
    RETURN_NULL();
  }

  /* get the number of elements passed in */
  arr_hash = Z_ARRVAL_P(arr);
  array_c = zend_hash_num_elements(arr_hash);
  if (!(array_c > 0))
    RETURN_EMPTY_STRING();

  if (!(ucs4 = (uint32_t *)malloc(array_c*sizeof(uint32_t))))
    PHP_MEMFAILURE;

  /* read php array */
  for(zend_hash_internal_pointer_reset_ex(arr_hash, &p);
      (SUCCESS == zend_hash_get_current_data_ex(arr_hash,(void**)&data,&p));
      zend_hash_move_forward_ex(arr_hash, &p))
    if (Z_TYPE_PP(data) == IS_LONG)
    {
      uint32_t u = (uint32_t)Z_LVAL_PP(data);
      if (0L < u && u < 0x110000L)
        ucs4[ucs4_l++] = u;
      else
        in_bad = 1; /* skip non-codepoint */
    }
    else
      in_bad = 1; /* skip non-Integer */


  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA_ERROR_SET("idna2_to_utf8", "Encoding failed.");
  }

  /* warn on bad input */
  if (in_bad)
    php_error(E_WARNING,
      "idna2: (idna2_to_utf8) Input array contains invalid elements.");
  if (ucs4_l < array_c)
    php_error(E_WARNING,
      "idna2: (idna2_to_utf8) Some codepoints couldn't have been converted");

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */


/* {{{ proto string idna2_punycode_encode(string name)
   Apply Puncode algorithm to a UTF-8 string */
PHP_FUNCTION( idna2_punycode_encode )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_l))
	{
    RETURN_NULL();
  }
  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_punycode_encode",r);
  }

  if (IDNA2_ERROR & (r = idna2_punycode_encode(&ucs4, &ucs4_l)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_punycode_encode",r);
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_punycode_encode",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_punycode_decode(string name)
   Decode an ASCII string by way of the Puncode algorithm. */
PHP_FUNCTION( idna2_punycode_decode )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_l))
	{
    RETURN_NULL();
  }
  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_punycode_decode",r);
  }

  if (IDNA2_ERROR & (r = idna2_punycode_decode(&ucs4, &ucs4_l)))
  {
    free(ucs4);
    
    /* having php_idna2_message */
    IDNA_ERROR_SET("idna2_punycode_decode","decoding failed");
    RETURN_FALSE;
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_punycode_decode",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_nameprep(string domain, bool allowUnassigned)
   Apply the Nameprep Algorithm (IDNA 2003). */
PHP_FUNCTION( idna2_nameprep )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;
	zend_bool allow_unassigned = 0;

	IDNA_ERROR_RESET;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &in, &in_l, &allow_unassigned))
	{
		RETURN_NULL();
  }

  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_nameprep",r);
  }
  
  /* idna2_flags idna2_libidn_nameprep
  (uint32_t **ucs4, size_t *ucs4_l, idna2_flags flags); */
  if (IDNA2_ERROR & (r = idna2_nameprep (&ucs4,&ucs4_l,
    (allow_unassigned)? 0 : IDNA2_CHECK_UNASSIGNED)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_nameprep",r);
  }
  
  if (IDNA2_SUCCESS != r)
  {
    /* having php_idna2_message */
    IDNA_ERROR_SET("idna2_nameprep","nameprep failed");
    RETURN_FALSE;
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_nameprep",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_to_ascii(string domain, bool allowUnassigned, bool useStd2Ascii)
   IDNA2003 ToAscii method */
PHP_FUNCTION( idna2_to_ascii )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;
	int flags = 0;
	zend_bool allow_unassigned = 0;
	zend_bool use_std3_ascii = 1;

	IDNA_ERROR_RESET;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bb", &in, &in_l, &allow_unassigned, &use_std3_ascii))
	{
		RETURN_NULL();
  }

  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_to_ascii",r);
  }
  
  if (allow_unassigned)
    flags |= IDNA_ALLOW_UNASSIGNED;
  if (use_std3_ascii)
    flags |= IDNA_USE_STD3_ASCII_RULES;

  /* idna2_flags idna2_libidn_to_ascii
  (uint32_t **ucs4, size_t *ucs4_l, idna2_flags flags); */
  if (IDNA2_ERROR & (r = idna2_libidn_to_ascii (&ucs4,&ucs4_l,flags)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_to_ascii",r);
  }

  if (IDNA2_SUCCESS != r)
  {
    /* having php_idna2_message */
    IDNA_ERROR_SET("idna2_to_ascii","encoding failure");
    RETURN_FALSE;
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_to_ascii",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_to_unicode(string domain, bool allowUnassigned, bool useStd2Ascii)
   IDNA2003 ToUnicode method */
PHP_FUNCTION( idna2_to_unicode )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;
	int flags = 0;
	zend_bool allow_unassigned = 0;
	zend_bool use_std3_ascii = 1;

	IDNA_ERROR_RESET;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bb", &in, &in_l, &allow_unassigned, &use_std3_ascii))
	{
		RETURN_NULL();
  }

  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_to_unicode",r);
  }

  if (allow_unassigned)
    flags |= IDNA_ALLOW_UNASSIGNED;
  if (use_std3_ascii)
    flags |= IDNA_USE_STD3_ASCII_RULES;

  /* idna2_flags idna2_libidn_to_unicode
  (uint32_t **ucs4, size_t *ucs4_l, idna2_flags flags); */
  if (IDNA2_ERROR & (r = idna2_libidn_to_unicode (&ucs4,&ucs4_l,flags)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_to_unicode",r);
  }

  if (IDNA2_SUCCESS != r)
  {
    /* having php_idna2_message */
    IDNA_ERROR_SET("idna2_to_unicode","decoding failure");
    RETURN_FALSE;
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_to_unicode",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_encode(string domain, long flags)
   Generic IDNA 2008 domain encoding. */
PHP_FUNCTION( idna2_encode )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;
	idn_action_t actions;
	idn_result_t res;

	IDNA_ERROR_RESET;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_l))
	{
		RETURN_NULL();
  }

  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_encode",r);
  }
  
  actions = IDN_MAP | IDN_IDNCONV;

  /* idna2_flags idna2_idnkit2_encodename
    (uint32_t **ucs4p, size_t *ucs4_l, idn_action_t actions, idn_result_t *res)
  */
  if (IDNA2_ERROR & (r = idna2_idnkit2_encodename(&ucs4,&ucs4_l,actions,&res)))
  {
    free(ucs4);
    
    if (IDNA2_MALLOC_ERROR == r)
      PHP_MEMFAILURE;

    IDNA2_HANDLE_ERROR("idna2_encode",r);
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_encode",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto string idna2_decode(string domain, long flags)
   generic domain encoding */
PHP_FUNCTION( idna2_decode )
{

	char *in, *ret;
	size_t in_l, ucs4_l;
  uint32_t *ucs4 = NULL;
	idna2_flags r;
	idn_action_t actions;
	idn_result_t res;

	IDNA_ERROR_RESET;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_l))
	{
		RETURN_NULL();
  }

  if (!(in_l > 0))
  {
    RETURN_EMPTY_STRING();
  }

  if (IDNA2_ERROR & (r = idna2_utf8_decode(in, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_decode",r);
  }

  actions = IDN_MAP | IDN_IDNCONV;

  /* idna2_flags idna2_idnkit2_decodename
    (uint32_t **ucs4p, size_t *ucs4_l, idn_action_t actions, idn_result_t *res)
  */
  if (IDNA2_ERROR & (r = idna2_idnkit2_decodename(&ucs4,&ucs4_l,actions,&res)))
  {
    free(ucs4);

    if (IDNA2_MALLOC_ERROR == r)
      PHP_MEMFAILURE;

    IDNA2_HANDLE_ERROR("idna2_decode",r);
  }

  if (IDNA2_ERROR & (r = idna2_utf8_encode(ucs4, ucs4_l, &ret)))
  {
    free(ucs4);
    IDNA2_HANDLE_ERROR("idna2_decode",r);
  }

 	RETVAL_STRING(ret, 1);
  free(ret), free(ucs4);
	return;
}
/* }}} */

/* {{{ proto int idna2_test(string arg, long flags)
   Test UTF-8 encoded string against various IDNA checks. */
PHP_FUNCTION( idna2_test )
{
	char *input, *ascii = NULL;
	int free_ascii = 0;
	size_t input_l, ucs4_l, ascii_l;
	long flags = 0, ret = 0;
	idna2_flags r;
  uint32_t *ucs4 = NULL;

	IDNA_ERROR_RESET;

	if(FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &input, &input_l, &flags))
		RETURN_NULL();

	if (flags == 0 || !(input_l > 0))
    RETURN_LONG(0);

  /* get codepoints */
  if (IDNA2_ERROR & (r = idna2_utf8_decode(input, &ucs4, &ucs4_l)))
  {
    IDNA2_HANDLE_ERROR("idna2_test",r);
  }

  {    /* perform checks on each label */
  	size_t i, st = 0;
  	//long r;
  	idn_result_t unexp;
    for (i = 0; i <= ucs4_l; i++)
    {
      if (i == ucs4_l || (
        /* punctuation characters */
        ucs4[i] == 0x2E || ucs4[i] == 0x3002 ||
        ucs4[i] == 0xFF0E || ucs4[i] == 0xFF61 ))
      {
        if (i > st)
        {
          if (IDNA2_ERROR & (r = __idna2_check_label(ucs4 + st, i - st, flags, &unexp)))
          {
            if (IDNA2_MALLOC_ERROR == r)
              PHP_MEMFAILURE;
            switch (r)
            {
              case IDNA2_ENCODING_ERROR:
                IDNA_ERROR_SET("idna2_test", "encoding failed");
                break;
              case IDNA2_UNKNOWN_ERROR:
                IDNA_ERROR_SET("idna2_test", "unknown error");
                break;
              default:
                IDNA_ERROR_SET("idna2_test", idna2_idnkit2_errstr(unexp));
                break;
            }
          }
          ret |= r;
        }
        else if (i < ucs4_l - 1) /* zero length */
          ret |= IDNA2_CHECK_LENGTH;
        st = i + 1;
      }
    }

  }
  
  /* non-LDH's are prohibited */
  if (ret & IDNA2_CHECK_LDH)
    ret |= IDNA2_CHECK_PROHIBITED;

	RETURN_LONG((ret & flags));
	return;
}
/* }}} */



idna2_flags
__idna2_check_label
  (uint32_t *label, size_t label_l, long flags, idn_result_t *res)
{
  uint32_t *ascii = NULL;
	size_t ascii_l = 0;
	int free_ascii = 0;
	idna2_flags ret = 0L;

#if _DEBUG_
   zend_printf("__idna2_check_label(%d)\n", label_l);
   zend_printf_ucs4(label, label_l);
#endif

  /* check ASCII range */
  {
    register int i = 0;
    while (i < label_l)
      if (label[i++] > 0x7F)
      {
        ret |= IDNA2_CHECK_ASCII;
        break;
      }
  }
  

  /* check for leading or trailing hyphen */
  if (label[0] == 0x2D || (label_l > 1 && label[label_l - 1] == 0x2D))
    ret |= IDNA2_CHECK_HYPHEN;

  if (ret & IDNA2_CHECK_ASCII) /* input contains non-ASCII */
  {
    /* u-label */

#if _DEBUG_
    zend_printf("u-label(%d)\n", label_l);
#endif


    /* check for "--" in the 3rd and 4th characters */
    if ((label_l >= 4) && (label[2] == 0x2D && label[3] == 0x2D))
      ret |= IDNA2_CHECK_HYPHEN34;

    /* IDNA2008 checks */
    if (flags & (IDNA2_MAP_IDNA2008 | IDNKIT2_CHECK_FLAGS))
    {

#if _DEBUG_
      zend_printf("IDNA2008\n");
#endif

      /* perform IDNA2008 checks using libidnkit's idn_checkname */
      idna2_flags r;
      char *utf8 = NULL;
      idn_result_t idn_checkname(idn_action_t actions, const char *name);
      
      /* to utf8 */
      if (IDNA2_ERROR & (r = idna2_utf8_encode(label, label_l, &utf8)))
      {
        return r; /* encoding failed */
      }
      
#define __idna2008_check(idna2_flag,idnkit2_flag,expected) \
      if ((flags & idna2_flag) && \
        (idn_success != (*res = idn_checkname(idnkit2_flag,utf8)))) \
      { \
        if (*res == expected) \
          ret |= idna2_flag; \
        else \
          return ((*res == idn_nomemory)? IDNA2_MALLOC_ERROR : IDNA2_UNKNOWN_ERROR); \
      }
      __idna2008_check(IDNA2_CHECK_PROHIBITED,IDN_PROHCHECK,idn_prohcheck_error)
      __idna2008_check(IDNA2_CHECK_UNASSIGNED,IDN_UNASCHECK,idn_unascheck_error)
      __idna2008_check(IDNA2_CHECK_NFC,IDN_NFCCHECK,idn_nfccheck_error)
      __idna2008_check(IDNA2_CHECK_COMBINING,IDN_COMBCHECK,idn_combcheck_error)
      __idna2008_check(IDNA2_CHECK_CONTEXTJ,IDN_CTXJCHECK,idn_ctxjcheck_error)
      __idna2008_check(IDNA2_CHECK_CONTEXTO_REGIST,IDN_CTXOCHECK,idn_ctxocheck_error)
      __idna2008_check(IDNA2_CHECK_CONTEXTO_LOOKUP,IDN_CTXOLITECHECK,idn_ctxocheck_error)
      __idna2008_check(IDNA2_CHECK_BIDI,IDN_BIDICHECK,idn_bidicheck_error)
      
      /* on IDNA2_MAP_IDNA2008, convert to ACE w/o checks using idnkit2 */
      if (flags & IDNA2_MAP_IDNA2008)
      {

         /* idna2_flags idna2_idnkit2_encodename_utf8
            (const char *utf8in, uint32_t **ucs4p, size_t *ucs4_l,
            idn_action_t actions, idn_result_t *res);
           */
        r = idna2_idnkit2_encodename_utf8(utf8, &ascii, &ascii_l, (IDN_MAP | IDN_IDNCONV), res);
        free(utf8);
        if (r & IDNA2_ERROR)
        {
          free(ascii);
          return r;
        }
        free_ascii = 1;
      }
      
    }
    
    if (!ascii) /* try libidn */
    {
      idna2_flags r;
      int rc;
      char * enc;
      size_t np_l = label_l, enc_l;
      uint32_t *np;

#if _DEBUG_
      zend_printf("try libidn\n");
#endif

      /* apply libidn's nameprep (may increase the size) */
      if (!(np = (uint32_t *)malloc(np_l*sizeof(uint32_t))))
        return IDNA2_MALLOC_ERROR;
      memcpy(np, label, label_l*sizeof(uint32_t));
      if (IDNA2_ERROR & (r = idna2_nameprep(&np, &np_l, flags)))
      {
        /* nameprep failed fatally */
        free(np);
        return r;
      }
      ret |= r;

#if _DEBUG_
      zend_printf("nameprep applied: %d\n", np_l);
      zend_printf_ucs4(np, np_l);
#endif

      /* apply punycode */
      if (IDNA2_ERROR & (r = idna2_punycode_encode(&np, &np_l)))
      {
        free(np);
        return r;
      }
      (ascii = np, ascii_l = np_l, free_ascii = 1);


#if _DEBUG_
      zend_printf("punycode applied: %d\n", ascii_l);
      zend_printf_ucs4(ascii, ascii_l);
#endif
      

      if (ascii_l > 63 - 4)
        ret |= IDNA2_CHECK_LENGTH;

    }
    
  }

  else /* ASCII input */
  {

#if _DEBUG_
    zend_printf("a-label(%d)\n", label_l);
#endif

    /* check for "--" in the 3rd and 4th characters */
    int is_hyph34 = ((label_l >= 4) && (label[2] == 0x2D && label[3] == 0x2D));
    int is_ace = (is_hyph34 && label_l > 4 && ( /* check for ACE prefix */
      (label[0] == 0x58 || label[0] == 0x78) &&
      (label[1] == 0x4E || label[1] == 0x6E) ));

    /* check ASCII length */
    if ( label_l > 63)
      ret |= IDNA2_CHECK_LENGTH;

    if (is_ace)
    {
      /* copy ucs4-ASCII to (char *) and try to decode */
      int i, r;
      char *enc;
      size_t enc_l = label_l-4, dec_l = IDNA2_BUFSZ;
      punycode_uint *dec; /* compatible uint32_t */
      
      if (!(enc = (char *)malloc((enc_l+1)*sizeof(char))))
        return IDNA2_MALLOC_ERROR;
      for(i = 0; i < enc_l; i++)
        enc[i] = (char)label[i+4];
      enc[enc_l] = '\0';
        
      if (!(dec = (punycode_uint*)malloc(dec_l*sizeof(uint32_t))))
      {
        free(enc);
        return IDNA2_MALLOC_ERROR;
      }

      /* int punycode_decode
        (size_t input_length, const char [] input,
        size_t * output_length, punycode_uint [] output, unsigned char [] case_flags) */
      if (PUNYCODE_SUCCESS == (r = punycode_decode(enc_l, enc, &dec_l, dec, NULL)))
      {
        /* check u-label */
        long ret2;
        
        if (IDNA2_ERROR & (ret2 = __idna2_check_label(
          (uint32_t *)dec, dec_l, flags, res)))
        {
          free(dec),free(enc);
          return ret2;
        }
        ret |= ret2;
      }
      else
        /* PUNYCODE_BAD_INPUT PUNYCODE_BIG_OUTPUT PUNYCODE_OVERFLOW */
        ret |= IDNA2_MAP_PUNYCODE;

      free(dec), free(enc);

    }
    else /* not encoded */
      if (is_hyph34)
        ret |= IDNA2_CHECK_HYPHEN34;

    (ascii = label, ascii_l = label_l);
  }
  
  if (ascii)
  {

    /* check for non-LDH */
    if (flags & IDNA2_CHECK_LDH)
    {
      int i;
      for (i = 0; i < ascii_l; i++)
        if ( /* the absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F. */
            ascii[i] <= 0x2C || ascii[i] == 0x2E || ascii[i] == 0x2F ||
            (ascii[i] >= 0x3A && ascii[i] <= 0x40) ||
            (ascii[i] >= 0x5B && ascii[i] <= 0x60) ||
            (ascii[i] >= 0x7B && ascii[i] <= 0x7F))
        {
          ret |= IDNA2_CHECK_LDH;
          break;
        }
    }

    /* check ASCII length */
    if ( (flags & IDNA2_CHECK_LENGTH) && ascii_l > 63)
      ret |= IDNA2_CHECK_LENGTH;
      

#if _DEBUG_
    zend_printf("free_ascii: %d\n", free_ascii);
#endif


    if (free_ascii)
      free(ascii);
  }

#if _DEBUG_
  zend_printf("return: %d\n", ret);
#endif

  return ret;
}


void
php_idna2_set_message(const char *src, const char *msg)
{
  sprintf(IDNA2_G(php_idna2_message), "(%s) %s", src, msg);
  /* zend_printf("php_idna2_set_message: (%s) %s\n", message); */
}


