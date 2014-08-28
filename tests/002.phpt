--TEST--
Check for fftw execute/destroy/cleanup methods
--SKIPIF--
--FILE--
<?php 
// here we test a create/execute/destroy with a single input array() for each function
$size	=16;	// should test others sizes?
$input	=array();
for ($i=0;$i<$size;$i++)
	$input[$i]=mt_rand(-mt_getrandmax(),mt_getrandmax());
$plan=fftw_create_plan();
if(!$plan){
	echo "Can't create fftw plan\n";
}else{
	$execute=fftw_execute();
	if(!$execute){
		echo "Can't execute fftw plan\n";
	}else{
		fftw_destroy_plan($plan);
		if($plan){
			echo "can't destroy plan";
		}else{
			$cleanup=fftw_cleanup();
			if(!$cleanup){
				echo "can't cleanup fftw";
			}else{
				echo "ok\n";
			}
		}
		
	}
}
?>
--EXPECT--
ok
