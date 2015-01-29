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

#ifndef PHP_IDNA2_H
#define PHP_IDNA2_H

#define _DEBUG_ 0

#define PHP_IDNA2_VERSION "0.1"

extern zend_module_entry idna2_module_entry;
#define phpext_idna2_ptr &idna2_module_entry

#ifdef PHP_WIN32
#define PHP_IDNA2_API __declspec(dllexport)
#else
#define PHP_IDNA2_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:     
*/

#define IDNA2_ZEND_GLOBAL_BUFSZ    0x4000

/* idna2_error, php_idna2_message */
ZEND_BEGIN_MODULE_GLOBALS(idna2)
  char idna2_error[IDNA2_ZEND_GLOBAL_BUFSZ];
  char php_idna2_message[IDNA2_ZEND_GLOBAL_BUFSZ];
ZEND_END_MODULE_GLOBALS(idna2)

/* In every utility function you add that needs to use variables 
   in php_idna2_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as IDNA2_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define IDNA2_G(v) TSRMG(idna2_globals_id, zend_idna2_globals *, v)
#else
#define IDNA2_G(v) (idna2_globals.v)
#endif



PHP_MINIT_FUNCTION(idna2);
PHP_MSHUTDOWN_FUNCTION(idna2);
PHP_RINIT_FUNCTION(idna2);
PHP_RSHUTDOWN_FUNCTION(idna2);
PHP_MINFO_FUNCTION(idna2);

/* PHP_FUNCTION(confirm_idna2_compiled); */	/* For testing, remove later. */

PHP_FUNCTION(idna2_error);
PHP_FUNCTION(idna2_test);

PHP_FUNCTION(idna2_from_utf8);
PHP_FUNCTION(idna2_to_utf8);

PHP_FUNCTION(idna2_labels);

PHP_FUNCTION(idna2_punycode_encode);
PHP_FUNCTION(idna2_punycode_decode);

PHP_FUNCTION(idna2_nameprep);

PHP_FUNCTION(idna2_to_ascii);
PHP_FUNCTION(idna2_to_unicode);

PHP_FUNCTION(idna2_encode);
PHP_FUNCTION(idna2_decode);


#endif	/* PHP_IDNA2_H */
