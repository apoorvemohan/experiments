for i in {1..1000};do perf stat -e instructions,cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./WATER-SPATIAL < input ;done > output
