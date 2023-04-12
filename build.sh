mkdir -p data
make -C libands
make -C bootloader LOADBIN=$PWD/data/load.bin
make
