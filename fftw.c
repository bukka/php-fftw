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
#include "ext/standard/info.h"

#include <fftw3.h>

/* malloc and free wrapper */
#ifdef PHP_FFTW_DEBUG
#define php_fftw_malloc emalloc
#define php_fftw_free   efree
#else
#define php_fftw_malloc fftw_malloc
#define php_fftw_free   fftw_free
#endif

/* data for input or output*/
typedef union {
	fftw_complex *c;
	double *r;
} php_fftw_io;

/* fftw plan */
typedef struct {
	zend_object zo;
	fftw_plan plan;
	php_fftw_io in;
	php_fftw_io out;
} php_fftw_plan_object;

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

/* {{{ php_fftw_plan_object_methods[] */
zend_function_entry php_fftw_plan_object_methods[] = {
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

/* class entry */
PHP_FFTW_API zend_class_entry *php_fftw_plan_ce;

/* object handler */
static zend_object_handlers php_fftw_plan_object_handlers;

/* {{{ */
static void php_fftw_plan_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	zend_objects_destroy_object(object, handle TSRMLS_CC);
}
/* }}} */

/* {{{ */
static void php_fftw_plan_object_free(zend_object *object TSRMLS_DC)
{
	php_fftw_plan_object *intern = (php_fftw_plan_object *) object;
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	if (intern->out.r && intern->in.r != intern->out.r) {
		php_fftw_free(intern->out.r);
	}
	if (intern->in.r) {
		fftw_destroy_plan(intern->plan);
		php_fftw_free(intern->in.r);
	}
	efree(intern);
}
/* }}} */

/* {{{ */
static zend_object_value php_fftw_plan_object_create_ex(zend_class_entry *class_type, php_fftw_plan_object **ptr TSRMLS_DC)
{
	zend_object_value retval;
	php_fftw_plan_object *intern;

	/* Allocate memory for it */
	intern = (php_fftw_plan_object *) emalloc(sizeof(php_fftw_plan_object));
	memset(intern, 0, sizeof(php_fftw_plan_object));
	if (ptr) {
		*ptr = intern;
	}
	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	PHP_FFTW_OBJECT_PROPERTIES_INIT(&intern->zo, class_type);

	retval.handlers = &php_fftw_plan_object_handlers;
	retval.handle = zend_objects_store_put(
		intern,
		(zend_objects_store_dtor_t) php_fftw_plan_object_dtor,
		(zend_objects_free_object_storage_t) php_fftw_plan_object_free,
		NULL TSRMLS_CC);

	return retval;
}
/* }}} */

/* {{{ */
static zend_object_value php_fftw_plan_object_create(zend_class_entry *class_type TSRMLS_DC)
{
	return php_fftw_plan_object_create_ex(class_type, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ */
zend_object_value php_fftw_plan_object_clone(zval *this_ptr TSRMLS_DC)
{
	php_fftw_plan_object *new_obj = NULL;
	php_fftw_plan_object *old_obj = (php_fftw_plan_object *) zend_object_store_get_object(this_ptr TSRMLS_CC);
	zend_object_value new_ov = php_fftw_plan_object_create_ex(old_obj->zo.ce, &new_obj TSRMLS_CC);

	zend_objects_clone_members(&new_obj->zo, new_ov, &old_obj->zo, Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);

	/*TODO: copy plan fftw data */

	return new_ov;
}
/* }}} */


/* macro for registering FFTW constants */
#define REGISTER_FFTW_CONSTANT(constant) REGISTER_LONG_CONSTANT(#constant, constant, CONST_CS | CONST_PERSISTENT)

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fftw)
{
	zend_class_entry ce;

	/* fftw_plan class */
	INIT_CLASS_ENTRY(ce, "fftw_plan", php_fftw_plan_object_methods);
	ce.create_object = php_fftw_plan_object_create;
	memcpy(&php_fftw_plan_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_fftw_plan_object_handlers.clone_obj = php_fftw_plan_object_clone;
	php_fftw_plan_ce = zend_register_internal_class(&ce TSRMLS_CC);

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
