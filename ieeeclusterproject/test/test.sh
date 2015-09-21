perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/kernels/radix/RADIX -p8 -n2021440000 -r1024 -m524288 &
perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/kernels/radix/RADIX -p8 -n2021440000 -r1024 -m524288 &
perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/kernels/radix/RADIX -p4 -n2021440000 -r1024 -m524288 &
#perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/kernels/radix/RADIX -p4 -n2021440000 -r1024 -m524288 &
#perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/kernels/radix/RADIX -p4 -n2021440000 -r1024 -m524288 &

#perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/kernels/radix/RADIX -p16 -n2021440000 -r1024 -m524288 &
#perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/apps/water-nsquared/WATER-NSQUARED < /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/apps/water-nsquared/input &
perf stat -e cache-misses,instructions /home/jiajun/apoorve/ieeeclusterproject/splash2_bench/codes/apps/ocean/contiguous_partitions/OCEANC -n258 -p4 -e1e-07 -r20000 -t12
