--TEST--
Check for fftw presence
--SKIPIF--
<?php if (!extension_loaded("fftw")) print "skip"; ?>
--FILE--
<?php 
echo "fftw extension is available";
?>
--EXPECT--
fftw extension is available
