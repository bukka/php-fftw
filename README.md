# PHP FFTW wrapper

This is a PHP wrapper for [FFTW library](http://www.fftw.org/).

## Installation

The extension can be installed on Linux.

#### Manual Installation

First download the source
```
git clone https://github.com/bukka/php-fftw.git
```

Then go to the created source directory and compile the extension. You need to have a php development package installed (command `phpize` must be available).
```
cd php-fftw
phpize
./configure
make
sudo make install
```

Finally you need to add
```
extension=fftw.so
```
to the php.ini
