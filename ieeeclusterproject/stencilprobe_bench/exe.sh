for i in {1..4}
do
         perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses,page-faults ./probe 512 512 512 8 8 8 2 >> oblivious.p.probe.4.log 2>&1 &
done
