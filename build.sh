make clean
rm _vh.extlink2argv.nds _hn.extlink2argv.nds
mkdir -p data
make -C libands
make -C bootloader LOADBIN=$PWD/data/load.bin
make -C bootstub
make -C nds-exception-stub STUBBIN=$PWD/data/exceptionstub.bin
make
cp extlink2argv.nds _vh.extlink2argv.nds
cp extlink2argv.nds _hn.extlink2argv.nds
rm -rf data
make -C bootloader clean
make -C bootstub clean
# make -C libands clean
