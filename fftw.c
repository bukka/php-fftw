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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_fftw.h"

#include <fftw3.h>


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_fftw_plan_dft_1d, 0)
ZEND_ARG_INFO(0, n)
ZEND_ARG_INFO(0, in)
ZEND_ARG_INFO(0, sign)
ZEND_ARG_INFO(0, flags)	
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fftw_execute, 0)
ZEND_ARG_INFO(0, plan)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fftw_output, 0)
ZEND_ARG_INFO(0, plan)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ php_fftw_functions[] */
zend_function_entry php_fftw_functions[] = {
	PHP_FE(fftw_plan_dft_1d,                    arginfo_fftw_plan_dft_1d)
	PHP_FE(fftw_execute,                        arginfo_fftw_execute)
	PHP_FE(fftw_output,                         arginfo_fftw_output)
	PHP_FFTW_FE_END
};
/* }}} */

/* {{{ fftw_module_entry */
zend_module_entry fftw_module_entry = {
	STANDARD_MODULE_HEADER,
	"fftw",
	php_fftw_functions,
	PHP_MINIT(fftw),
	PHP_MSHUTDOWN(fftw),
	NULL,
	NULL,
	PHP_MINFO(fftw),
	PHP_FFTW_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FFTW
ZEND_GET_MODULE(fftw)
#endif

/* macro for registering FFTW constants */
#define REGISTER_FFTW_CONSTANT(constant) REGISTER_LONG_CONSTANT(#constant, constant, CONST_CS | CONST_PERSISTENT)

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fftw)
{
	/* FFTW version constant (todo: get from lib) */
	REGISTER_STRING_CONSTANT("FFTW_VERSION", "3.4", CONST_PERSISTENT | CONST_CS);
	
	/* Sign constants */
	REGISTER_FFTW_CONSTANT(FFTW_FORWARD);
	REGISTER_FFTW_CONSTANT(FFTW_BACKWARD);
	
	/* Flags constants */
	REGISTER_FFTW_CONSTANT(FFTW_MEASURE);
	REGISTER_FFTW_CONSTANT(FFTW_DESTROY_INPUT);
	REGISTER_FFTW_CONSTANT(FFTW_UNALIGNED);
	REGISTER_FFTW_CONSTANT(FFTW_CONSERVE_MEMORY);
	REGISTER_FFTW_CONSTANT(FFTW_EXHAUSTIVE);
	REGISTER_FFTW_CONSTANT(FFTW_PRESERVE_INPUT);
	REGISTER_FFTW_CONSTANT(FFTW_PATIENT);
	REGISTER_FFTW_CONSTANT(FFTW_ESTIMATE);
	REGISTER_FFTW_CONSTANT(FFTW_WISDOM_ONLY);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(fftw)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fftw)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "FFTW support", "enabled");
	php_info_print_table_row(2, "FFTW version", PHP_FFTW_VERSION);
	php_info_print_table_end();
}
/* }}} */


/* {{{ proto fftw_plan fftw_plan_dft_1d(int $n, array $in, int $sign, int $flags)
   Create plan for 1d dft */
PHP_FUNCTION(fftw_plan_dft_1d)
{
	zval *z_in, **zpp_data;
	HashPosition pos;
	long n, sign, flags;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lall", &n, &z_in, &sign, &flags) == FAILURE) {
		return;
	}
	
	RETURN_NULL();
}
/* }}} */

/* {{{ proto bool fftw_execute(fftw_plan $plan)
   Execute plan */
PHP_FUNCTION(fftw_execute)
{
	zval *z_plan;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &z_plan) == FAILURE) {
		return;
	}
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void fftw_output(fftw_plan $plan)
   Return plan output */
PHP_FUNCTION(fftw_output)
{
	zval *z_plan;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &z_plan) == FAILURE) {
		return;
	}
	
	RETURN_TRUE;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
