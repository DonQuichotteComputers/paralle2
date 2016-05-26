#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

CROSS_PREFIX=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac

# Build HOST side application
${CROSS_PREFIX}gcc -Ofast src/$1.c -o Debug/$1.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
#e-gcc --help=optimizers gives you hints ; -m1reg-r63 -msmall16 -mfp-mode=int
#e-gcc -T ${ELDF} -Ofast -mfp-mode=int -m1reg-r63 -mshort-calls $2 $3 $4 $5 $6 $7 $8 $9 src/e_$1.c  src/borderwest_sdk2015.S -o Debug/e_$1.elf -le-lib
e-gcc -T ${ELDF} -Ofast -mfp-mode=int -m1reg-r63 $2 $3 $4 $5 $6 $7 $8 $9 src/e_$1.c src/borderwest_sdk2015.S -o Debug/e_$1.elf -le-lib

# Convert ebinary to SREC file
# -S: remove unused sections
# -g: remove debug sections
e-objcopy --srec-forceS3 --output-target srec -S Debug/e_$1.elf Debug/e_$1.srec
