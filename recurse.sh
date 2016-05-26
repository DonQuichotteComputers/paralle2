#testing a canvas for recursive functions

#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=/home/ylav/dev/parallella/buildroot/esdk.2015.1/bsps/current/internal.ldf

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

# Build DEVICE side program
#e-gcc --help=optimizers gives you hints ; -m1reg-r63 -msmall16 -mfp-mode=int
#src/innertile.S 
echo !$1!
e-gcc -T ${ELDF} -O0 -mshort-calls -mfp-mode=int -m1reg-r63 $1 $2  $3 $4 $5 $6 $7 $8 $9 src/recurse.c -o recurse.elf -le-lib
echo !$1!

# Convert ebinary to SREC file
# -S: remove unused sections
# -g: remove debug sections
e-objcopy --srec-forceS3 --output-target srec -S recurse.elf recurse.srec
