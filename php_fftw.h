/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 Jakub Zelenka                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FFTW_H
#define PHP_FFTW_H

extern zend_module_entry fftw_module_entry;
#define phpext_fftw_ptr &fftw_module_entry

#ifdef PHP_WIN32
#	define PHP_FFTW_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_FFTW_API __attribute__ ((visibility("default")))
#else
#	define PHP_FFTW_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 3 && PHP_RELEASE_VERSION >= 7) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 4) || (PHP_MAJOR_VERSION > 5)
#define PHP_FFTW_FE_END PHP_FE_END
#else
#define PHP_FFTW_FE_END {NULL,NULL,NULL}
#endif

/* fftw version */
#define PHP_FFTW_VERSION "0.1.0"


PHP_MINIT_FUNCTION(fftw);
PHP_MSHUTDOWN_FUNCTION(fftw);
PHP_RINIT_FUNCTION(fftw);
PHP_RSHUTDOWN_FUNCTION(fftw);
PHP_MINFO_FUNCTION(fftw);

/* FFTW functions */
PHP_FUNCTION(fftw_plan_dft_1d);
PHP_FUNCTION(fftw_execute);
PHP_FUNCTION(fftw_output);


#endif	/* PHP_FFTW_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
