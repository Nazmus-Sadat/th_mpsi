##libssl-dev installation: libssl is the portion of OpenSSL which supports TLS ( SSL and TLS #Protocols ), and depends on libcrypto. 
sudo apt-get install libssl-dev

#gcc5
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-5 g++-5

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 1

sudo apt-get install gcc-multilib

#cmake 3
sudo apt-get install build-essential
wget http://www.cmake.org/files/v3.2/cmake-3.2.2.tar.gz
tar xf cmake-3.2.2.tar.gz
cd cmake-3.2.2
./configure
make
sudo make install
sudo apt-get install cmake-curses-gui


###for NFLlib

##install GMP
wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz
tar -xf gmp-6.1.2.tar.xz
cd gmp-6.1.2/
sudo apt-get install m4
./configure
make
sudo make install

##install MPFR (wget http://www.mpfr.org/mpfr-current/mpfr-3.1.5.tar.xz)
wget http://www.mpfr.org/mpfr-3.1.5/mpfr-3.1.5.tar.xz
tar -xf mpfr-3.1.5.tar.xz
cd mpfr-3.1.5
./configure
make
sudo make install

##NFL
sudo apt-get install libgmp-dev
git clone https://github.com/quarkslab/NFLlib.git
cd NFLlib
mkdir _build
cd _build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/nfllib
make
sudo make install

#execution of an assembly
as lib/prng/nfl_crypto_stream_salsa20_amd64_xmm6.s

