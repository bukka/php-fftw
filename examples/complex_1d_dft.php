<?php

$in = array(
	new fftw_complex(0.3, 0.2),
	new fftw_complex(0.6, 0.8),
);

$n = 100;

// returns fftw_plan object
$p = fftw_plan_dft_1d($n, $in, FFTW_FORWARD, FFTW_ESTIMATE);
fftw_execute($p);

$out = fftw_output($p);

var_dump($out);