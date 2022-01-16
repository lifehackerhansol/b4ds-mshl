make clean
mkdir -p data
make -C bootloader LOADBIN=$PWD/data/load.bin
make -C bootstub
make -C nds-exception-stub STUBBIN=$PWD/data/exceptionstub.bin
make
rm -rf data
make -C bootloader clean
make -C bootstub clean
make -C libprism clean