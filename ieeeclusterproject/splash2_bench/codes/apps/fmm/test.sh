for i in {1..1000};do perf stat -e instructions,cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./FMM < inputs/input.256 ;done > output
