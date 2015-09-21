MAIN=$HOME/splash2_benchmark-1.0/codes/
KERNEL=$MAIN/kernel
APPS=$MAIN/apps

cd $KERNEL/lu/contiguous_blocks
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./LU -n4096 -p16 -b8 -c -s -t > ../../../LU.c.out 2>&1 &
cd $MAIN


cd $KERNEL/lu/non_contiguous_blocks
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./LU -n4096 -p16 -b16 -s -t > ../../../LU.nc.out 2>&1 &
cd $MAIN


cd $KERNEL/radix
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./RADIX -p16 -r8 -n1073741824 -m7 -s -t > ../../RADIX.out 2>&1  &
cd $MAIN


cd $APPS/barnes
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./BARNES < input > ../../BARNES.out 2>&1 &
cd $MAIN


cd $APPS/fmm
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./FMM < inputs/input.16384 > ../../FMM.out 2>&1 &
cd $MAIN

