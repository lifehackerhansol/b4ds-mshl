mkdir -p data
make -C libnds32
make -C bootloader LOADBIN=$PWD/data/load.bin
make
