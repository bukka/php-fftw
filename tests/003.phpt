--TEST--
Check for fftw check function MSE, function: xyz
--SKIPIF--
--FILE--
<?php 
include_once('./common.php');

$input	=array(1);	// an well known input array
$output	=array(1.1);	// an well known output fft from $input array
$element_MAG_ERROR	=10;	// must check a good value
$element_PHASE_ERROR	=10;	// must check a good value
$total_MAG_ERROR	=10;	// must check a good value
$total_PHASE_ERROR	=10;	// must check a good value
/*
var_dump(check_return_real($input, $output, $element_MAG_ERROR, $total_MAG_ERROR ));
var_dump(check_return_complex(
			$input, $output, 
			$element_MAG_ERROR, $element_PHASE_ERROR, 
			$total_MAG_ERROR, $total_PHASE_ERROR));
*/

$plan	=fftw_create_plan();
$execute=fftw_execute();

if(check_return_real($input, $output, $element_MAG_ERROR, $total_MAG_ERROR ))
	echo "ok";

fftw_destroy_plan($plan);
fftw_cleanup();
?>
--EXPECT--
ok
