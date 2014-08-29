<?php
/**
 * Calculate the N number from n and fft type
 *
 * For dimension of size n, there is a corresponding “logical” dimension N that determines the 
 * normalization (and the optimal factorization); the formula for N is given for each kind below. 
 * Also, with each transform kind is listed its corrsponding inverse transform. FFTW computes 
 * unnormalized transforms: a transform followed by its inverse will result in the original 
 * data multiplied by N (or the product of the N's for each dimension, in multi-dimensions).
 *
 * for more information check 
 * http://www.fftw.org/fftw3_doc/Real_002dto_002dReal-Transform-Kinds.html#Real_002dto_002dReal-Transform-Kinds
 * http://www.fftw.org/fftw3_doc/1d-Real_002deven-DFTs-_0028DCTs_0029.html#g_t1d-Real_002deven-DFTs-_0028DCTs_0029
 *
 * @param long  $fft_type     fft_r2r constant
 * @param long  $n	      fft array size
 *
 * @access public
 *
 * @return array($inverse_type,$N)    return an array, [0] is the inverse r2r constant, [1] is N "logical" size
 *                                    return false on error
 */
function calculate_N($fft_type,$n){
	$ret=false;
	switch $fft_type{
		case FFTW_R2HC:
			$ret=array(FFTW_HC2R,$n);		break;
		case FFTW_HC2R:
			$ret=array(FFTW_R2HC,$n);		break;
		case FFTW_DHT:
			$ret=array(FFTW_DHT,$n);		break;
		case FFTW_REDFT00:
			$ret=array(FFTW_REDFT00,2*($n-1));	break;
		case FFTW_REDFT10:
			$ret=array(FFTW_REDFT01,2*$n);		break;
		case FFTW_REDFT01:
			$ret=array(FFTW_REDFT10,2*$n);		break;
		case FFTW_REDFT11:
			$ret=array(FFTW_REDFT11,2*$n);		break;
		case FFTW_RODFT00:
			$ret=array(FFTW_RODFT00,2*($n+1));	break;
		case FFTW_RODFT10:
			$ret=array(FFTW_RODFT01,2*$n);		break;
		case FFTW_RODFT01:
			$ret=array(FFTW_RODFT10,2*$n);		break;
		case FFTW_RODFT11:
			$ret=array(FFTW_RODFT11,2*$n);		break;
	}
	return($ret);
}

/**
 * Calculate the absolute difference from two angles, in a range of 0-180 degree
 *
 * @param double  $a1         first angle
 * @param double  $a1         second angle
 * @param boolean $degree     if false, execute rad2deg and deg2rad
 *
 * @access public
 *
 * @return double  The angle diference
 */
function angle_diference($a1,$a2,$degree=false){
	if(!$degree){
		/* radian angles */
		$a=rad2deg($a1);
		$b=rad2deg($a2);
	}else{
		/* degree angles */
		$a=$a1;
		$b=$a2;
	}
	$d = abs($a - $b);			/* absolute diference */
	$d = $d-floor($d/360)*360;		/* range 0-360 */
	$r = ($d > 180 ? 360 - $d : $d);	/* range 0-180 */
	if(!$degree){
		/* radian angles */
		$r=deg2rad($r);
	}
	return ($r);
}
/**
 * Calculate the absolute difference from two angles, in a range of 0-180 degree
 *
 * @param array of fftw_complex   $return_array       Array of complex number to compare
 * @param array of fftw_complex  $know_return_array   Expected values of $return_array
 * @param boolean $expected_individual_MAG_AE         Max Magnitude absolute error per complex number ($return_array[$i])
 * @param boolean $expected_individual_PHASE_AE       Max Phase absolute error per complex number ($return_array[$i])
 * @param boolean $expected_total_MAG_MSE             Max Magnitude Mean Square
 * @param boolean $expected_total_PHASE_MSE           Max Phase Mean Square Error
 *
 * @access public
 *
 * @return mixed  Return true when ok, return string error message when something go wrong
 */
function check_return_complex(
	$return_array,
	$know_return_array,
	$expected_individual_MAG_AE,
	$expected_individual_PHASE_AE,
	$expected_total_MAG_MSE,
	$expected_total_PHASE_MSE)
{
/*
TODO: CHECK ERROR MEASUREMENT METHOD
	// show use error vector magnitude? http://en.wikipedia.org/wiki/Error_vector_magnitude
	
	element error measure
		1) 	square root (Perror/Preference)* 100%
			problem: Preference could be complex (0,0) -> div by zero
		
		2)	diference from complex-complex could be a complex with only imaginary 
		3)	max magnitude diference? and max phase diference?
		4)	max distance from error, number 1- number 2, magnitude from the result and phase?
		i think (3) is the best solution, thinking about bode plot error
	
	total error measure
		using (3) from element error measure, could we implement a MSE of magnitude and phase
	
*/
	/* Array size must be equal */
	if(count($return_array) != count($know_return_array)) 
		return("Different array size (".count($return_array).") != (".count($know_return_array).")");
	/* Array size >= 1 */
	if(count($return_array)<=0)
		return("Array count equal or less than zero");
	$total_MAG_MSE	=0;
	$total_PHASE_MSE=0;
	foreach($return_array as $k=>$v){
		/* Known return value must exists */
		if(!isset($know_return_array[$k]))
			return("Unknow [$k] index inside result array");
		/* TODO: include complex number check */
		if(!(is_int($v) || is_double($v)))
			return("Return Value of [$k] isn't an int or double value or complex number");
		if(!(is_int($know_return_array[$k]) || is_double($know_return_array[$k])))
			return("Known Value of [$k] isn't an int or double value or complex number");
		/* TODO: convert real number to complex */
		$v_REAL		=0;
		$v_IMAG		=0;
		/* TODO: convert rectangular complex number to polar (magnitude/phase) */
		$v_MAG		=0;
		$v_PHASE	=0;
		/* TODO: convert rectangular complex number to polar (magnitude/phase) */
		$know_MAG	=0;
		$know_PHASE	=0;
			
		/* Calculate absolute Magnitude Error, and sum total square error */
		$cur_AE		=abs($know_MAG-$v_MAG);
		$total_MAG_MSE	+=pow($cur_AE,2);
		/* Return if bigger than expected */
		if ($cur_AE>$expected_individual_MAG_AE)
			return("Absolute Magnitude error ($cur_AE) of [$k] index, bigger than allowed ($expected_individual_MAG_AE)");
		/* Calculate angle diference error, and sum total square error */
		$cur_AE		=angle_diference($know_PHASE,$v_PHASE);
		$total_PHASE_MSE+=pow($cur_AE,2);
		/* Return if bigger than expected */
		if ($cur_AE>$expected_individual_PHASE_AE)
			return("Phase error ($cur_AE) of [$k] index, bigger than allowed ($expected_individual_PHASE_AE)");
	}
	/* Mean root of total square sum */
	$total_MAG_MSE	=sqrt( $total_MAG_MSE / count($return_array));
	$total_PHASE_MSE=sqrt( $total_PHASE_MSE / count($return_array));
	/* Return if bigger than expected */
	if ($total_MAG_MSE>$expected_total_MAG_MSE)
		return("Mean Square Magnitude Error ($total_MAG_MSE) bigger than allowed error ($expected_total_MAG_MSE)");
	/* Return if bigger than expected */
	if ($total_PHASE_MSE>$expected_total_PHASE_MSE)
		return("Mean Square Phase Error ($total_PHASE_MSE) bigger than allowed error ($expected_total_PHASE_MSE)");
	/* Return true, ok =) */
	return true;
}
/**
 * Calculate the absolute difference from two angles, in a range of 0-180 degree
 *
 * @param array of double   $return_array        Array of real number (int/double) to compare
 * @param array of double   $know_return_array   Expected values of $return_array
 * @param boolean $expected_individual_AE        Max absolute error per real number ($return_array[$i])
 * @param boolean $expected_total_MSE            Max Mean Square Error
 *
 * @access public
 *
 * @return mixed  Return true when ok, return string error message when something go wrong
 */
function check_return_real(
	$return_array,
	$know_return_array,
	$expected_individual_AE,
	$expected_total_MSE)
{
	/* Array size must be equal */
	if(count($return_array) != count($know_return_array)) 
		return("Different array size (".count($return_array).") != (".count($know_return_array).")");
	/* Array size >= 1 */
	if(count($return_array)<=0)
		return("Array count equal or less than zero");
	$total_MSE=0;
	foreach($return_array as $k=>$v){
		/* Known return value must exists */
		if(!isset($know_return_array[$k]))
			return("Unknow [$k] index inside result array");
		if(!(is_int($v) || is_double($v)))
			return("Return Value of [$k] isn't an int or double value");
		if(!(is_int($know_return_array[$k]) || is_double($know_return_array[$k])))
			return("Known Value of [$k] isn't an int or double value");

		/* Calculate absolute diference error, and sum total square error */
		$cur_AE	=abs($know_return_array[$k]-$v);
		$total_MSE+=pow($v-$know_return_array[$k],2);
		/* Return if bigger than expected */
		if ($cur_AE>$expected_individual_AE)
			return("Absolute error ($cur_AE) of [$k] index, bigger than allowed ($expected_individual_AE)");
	}
	$total_MSE=sqrt($total_MSE / count($return_array));
	/* Return if bigger than expected */
	if ($total_MSE>$expected_total_MSE)
		return("Mean Square Error ($total_MSE) bigger than allowed error ($expected_total_MSE)");
	/* Return true, ok =) */
	return true;
}

/*
Tests:

$input	=array(1);	// an well known input array
$output	=array(1.1);	// an well known output fft from $input array
$element_MAG_ERROR	=10;	// must check a good value
$element_PHASE_ERROR	=10;	// must check a good value
$total_MAG_ERROR	=10;	// must check a good value
$total_PHASE_ERROR	=10;	// must check a good value

var_dump(check_return_real($input, $output, $element_MAG_ERROR, $total_MAG_ERROR ));
var_dump(check_return_complex(
			$input, $output, 
			$element_MAG_ERROR, $element_PHASE_ERROR, 
			$total_MAG_ERROR, $total_PHASE_ERROR));
var_dump(angle_diference(M_PI,M_PI+0.01,false));
var_dump(angle_diference(360,360+0.01,false));
*/
