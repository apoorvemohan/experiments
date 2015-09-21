
for i in {1..16}
do
         perf stat  -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults apps/fmm/FMM < apps/fmm/inputs/input.16384.16 >> FMM.seq.16.p16.log 2>&1
done

for i in {1..16}
do
         perf stat  -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults apps/fmm/FMM < apps/fmm/inputs/input.16384.8 >> FMM.seq.16.p8.log 2>&1
done

for i in {1..16}
do
         perf stat  -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults apps/fmm/FMM < apps/fmm/inputs/input.16384.4 >> FMM.seq.16.p4.log 2>&1
done

for i in {1..16}
do
         perf stat  -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults apps/fmm/FMM < apps/fmm/inputs/input.16384.2 >> FMM.seq.16.p2.log 2>&1
done

for i in {1..16}
do
         perf stat  -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults apps/fmm/FMM < apps/fmm/inputs/input.16384.1 >> FMM.seq.16.p1.log 2>&1
done


for i in {1..16}
do
	perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/lu/contiguous_blocks/LU -n4500 -p16 -b16 -c >> luc.seq.16.p16.log 2>&1
done

for i in {1..16}
do
        perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/lu/contiguous_blocks/LU -n4500 -p8 -b16 -c >> luc.seq.16.p8.log 2>&1
done

for i in {1..16}
do
        perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/lu/contiguous_blocks/LU -n4500 -p4 -b16 -c >> luc.seq.16.p4.log 2>&1
done

for i in {1..16}
do
        perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/lu/contiguous_blocks/LU -n4500 -p2 -b16 -c >> luc.seq.16.p2.log 2>&1
done

for i in {1..16}
do
        perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/lu/contiguous_blocks/LU -n4500 -p1 -b16 -c >> luc.seq.16.p1.log 2>&1
done

for i in {1..16}
do
       perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/radix/RADIX -p16 -n115500000 -r8 -m7 -s  >> radix.seq.16.p16.log 2>&1
done


for i in {1..16}
do
       perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/radix/RADIX -p8 -n115500000 -r8 -m7 -s  >> radix.seq.16.p8.log 2>&1
done

for i in {1..16}
do
       perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/radix/RADIX -p4 -n115500000 -r8 -m7 -s  >> radix.seq.16.p4.log 2>&1
done

for i in {1..16}
do
       perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/radix/RADIX -p2 -n115500000 -r8 -m7 -s  >> radix.seq.16.p2.log 2>&1
done

for i in {1..16}
do
       perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/radix/RADIX -p1 -n115500000 -r8 -m7 -s  >> radix.seq.16.p1.log 2>&1
done

#for i in {1..1}
#do
#	 perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/lu/contiguous_blocks/LU -n4500 -p16 -b16 -c >> $LOGFILE 2>&1 &
#done


#for i in {1..2}
#do
#         perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults kernels/radix/RADIX -p16 -n115500000 -r8 -m7 -s >> $LOGFILE 2>&1 &
#done


