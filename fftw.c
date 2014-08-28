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

	REGISTER_STRING_CONSTANT("FFTW_VERSION", (char *) fftw_version, CONST_PERSISTENT | CONST_CS);

	/* Algorithm-restriction flags (constants) */
	/* Considering declaration order of fftw3 docs 
	  (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags) 
	*/
	REGISTER_FFTW_CONSTANT(FFTW_DESTROY_INPUT);
	REGISTER_FFTW_CONSTANT(FFTW_PRESERVE_INPUT);
	REGISTER_FFTW_CONSTANT(FFTW_UNALIGNED);
	REGISTER_FFTW_CONSTANT(FFTW_ESTIMATE);
	REGISTER_FFTW_CONSTANT(FFTW_MEASURE);
	REGISTER_FFTW_CONSTANT(FFTW_PATIENT);
	REGISTER_FFTW_CONSTANT(FFTW_EXHAUSTIVE);
	REGISTER_FFTW_CONSTANT(FFTW_WISDOM_ONLY);
	
	/* set_timelimit flag */
	REGISTER_FFTW_CONSTANT(FFTW_NO_TIMELIMIT);

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
	php_info_print_table_row(2, "FFTW library version", fftw_version);
	php_info_print_table_end();
}
/* }}} */

/* {{{ */
static fftw_complex *php_fftw_fill_complex_array(fftw_complex *in, zval *z_ht TSRMLS_DC)
{
	zval **ppz_item, *pz_real, *pz_complex, z_tmp;
	HashPosition pos;
	HashTable *ht = Z_ARRVAL_P(z_ht);
	int i = 0;

	for (zend_hash_internal_pointer_reset_ex(ht, &pos);
		zend_hash_get_current_data_ex(ht, (void **) &ppz_item, &pos) == SUCCESS;
		zend_hash_move_forward_ex(ht, &pos), i++
	) {
		if (Z_TYPE_PP(ppz_item) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Complex item %d has to be array", i);
			return NULL;
		}
		if (zend_hash_num_elements(Z_ARRVAL_PP(ppz_item)) != 2) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Complex item %d array has to contain exactly two elements", i);
			return NULL;
		}
		if (zend_hash_index_find(Z_ARRVAL_PP(ppz_item), 0, (void **) &pz_real) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Complex item %d missing index 0 for real part", i);
			return NULL;
		}
		if (zend_hash_index_find(Z_ARRVAL_PP(ppz_item), 0, (void **) &pz_complex) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Complex item %d missing index 1 for complex part", i);
			return NULL;
		}
		INIT_PZVAL_COPY(&z_tmp, pz_real);
		convert_to_double(&z_tmp);
		in[i][0] = Z_DVAL(z_tmp);
		INIT_PZVAL_COPY(&z_tmp, pz_complex);
		convert_to_double(&z_tmp);
		in[i][1] = Z_DVAL(z_tmp);
	}

	return in;
}
/* }}} */

/* {{{ proto fftw_plan fftw_plan_dft_1d(int $n, array $in, int $sign, int $flags)
   Create plan for 1d dft */
PHP_FUNCTION(fftw_plan_dft_1d)
{
	zval *z_in;
	long n, sign, flags;
	int in_size;
	fftw_complex *in, *out;
	fftw_plan plan;
	php_fftw_plan_object *object;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lall", &n, &z_in, &sign, &flags) == FAILURE) {
		return;
	}

	in_size = zend_hash_num_elements(Z_ARRVAL_P(z_in));
	if (in_size != n) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Invalid number of elements in input array (expected: %ld, received: %d)", n, in_size);
		RETURN_NULL();
	}

	in = (fftw_complex *) php_fftw_malloc(n * sizeof(fftw_complex));
	out = (fftw_complex *) php_fftw_malloc(n * sizeof(fftw_complex));

	plan = fftw_plan_dft_1d(n, in, out, sign, flags);

	if (!php_fftw_fill_complex_array(in, z_in TSRMLS_CC)) {
		RETURN_NULL();
	}

	object_init_ex(return_value, php_fftw_plan_ce);
	object = (php_fftw_plan_object *) zend_object_store_get_object(return_value TSRMLS_CC);
	object->plan = plan;
	object->in.c = in;
	object->out.c = out;
}
/* }}} */

/* {{{ proto void fftw_execute(fftw_plan $plan)
   Execute plan */
PHP_FUNCTION(fftw_execute)
{
	zval *z_plan;
	php_fftw_plan_object *object;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_plan, php_fftw_plan_ce) == FAILURE) {
		return;
	}

	object = (php_fftw_plan_object *) zend_object_store_get_object(z_plan TSRMLS_CC);
	fftw_execute(object->plan);

	RETURN_NULL();
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
