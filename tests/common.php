<?php
function angle_diference($a1,$a2,$degree=false){	// angle "distance"
	if(!$degree){
		$a=rad2deg($a1);
		$b=rad2deg($a2);
	}else{
		// degree
		$a=$a1;
		$b=$a2;
	}
	$d = abs($a - $b);
	$d = $d-floor($d/360)*360;
	$r = ($d > 180 ? 360 - $d : $d);
	if(!$degree)
		$r=deg2rad($r);
	return ($r);
}
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
		i think (3) is the best solution
	
	total error measure
		using (3) from element error measure, could we implement a MSE of magnitude and phase
	
*/
	if(count($return_array) != count($know_return_array)) 
		return("Different array size (".count($return_array).") != (".count($know_return_array).")");
	if(count($return_array)<=0)
		return("Array count equal or less than zero");
	$total_MAG_MSE	=0;
	$total_PHASE_MSE=0;
	foreach($return_array as $k=>$v){
		if(!isset($know_return_array[$k]))
			return("Unknow [$k] index inside result array");
		$v_REAL		=0;
		$v_IMAG		=0;
		$v_MAG		=0;
		$v_PHASE	=0;
		$know_MAG	=0;
		$know_PHASE	=0;
			
		$cur_AE		=abs($know_MAG-$v_MAG);
		$total_MAG_MSE	+=pow($cur_AE,2);
		if ($cur_AE>$expected_individual_MAG_AE)
			return("Absolute Magnitude error ($cur_AE) of [$k] index, bigger than allowed ($expected_individual_MAG_AE)");
		$cur_AE		=angle_diference($know_PHASE,$v_PHASE);
		$total_PHASE_MSE+=pow($cur_AE,2);
		if ($cur_AE>$expected_individual_PHASE_AE)
			return("Phase error ($cur_AE) of [$k] index, bigger than allowed ($expected_individual_PHASE_AE)");
	}
	$total_MAG_MSE	=sqrt( $total_MAG_MSE / count($return_array));
	$total_PHASE_MSE=sqrt( $total_PHASE_MSE / count($return_array));
	if ($total_MAG_MSE>$expected_total_MAG_MSE)
		return("Mean Square Magnitude Error ($total_MAG_MSE) bigger than allowed error ($expected_total_MAG_MSE)");
	if ($total_PHASE_MSE>$expected_total_PHASE_MSE)
		return("Mean Square Phase Error ($total_PHASE_MSE) bigger than allowed error ($expected_total_PHASE_MSE)");
	return true;
}
function check_return_real(
	$return_array,
	$know_return_array,
	$expected_individual_AE,
	$expected_total_MSE)
{
	if(count($return_array) != count($know_return_array)) 
		return("Different array size (".count($return_array).") != (".count($know_return_array).")");
	if(count($return_array)<=0)
		return("Array count equal or less than zero");
	$total_MSE=0;
	foreach($return_array as $k=>$v){
		if(!isset($know_return_array[$k]))
			return("Unknow [$k] index inside result array");
		$cur_AE	=abs($know_return_array[$k]-$v);
		if ($cur_AE>$expected_individual_AE)
			return("Absolute error ($cur_AE) of [$k] index, bigger than allowed ($expected_individual_AE)");
		$total_MSE+=pow($v-$know_return_array[$k],2);
	}
	$total_MSE=sqrt($total_MSE / count($return_array));
	if ($total_MSE>$expected_total_MSE)
		return("Mean Square Error ($total_MSE) bigger than allowed error ($expected_total_MSE)");
	return true;
}

/*

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
