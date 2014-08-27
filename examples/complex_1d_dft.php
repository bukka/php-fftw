<?php

$in = array(
	array(0.3, 0.2),
	array(0.6, 0.8),
);

$n = 2;

// returns fftw_plan object
$p = fftw_plan_dft_1d($n, $in, FFTW_FORWARD, FFTW_ESTIMATE);
var_dump($p);
//fftw_execute($p);

//$out = fftw_output($p);

//var_dump($out);