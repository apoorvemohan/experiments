MAIN= ~/splash2_benchmark-1.0/codes
KERNEL=$(MAIN)/kernels
APPS=$(MAIN)/apps
LOGFILE=logfile
test=-l | wc



all:
	make luc &
	make lunc &
	make radix &
	make fmm &
	make barnes & 

luc_lunc_radix_fmm:
	make luc &
	make lunc &
	make radix &
	make fmm &

lunc_fmm_luc_radix:
	make lunc &
	make fmm &
	make luc &
	make radix &

luc_lunc_radix_barnes:
	make luc &
	make lunc &
	make radix &
	make barnes &

luc_lunc_fmm_barnes:
	make luc &
	make lunc &
	make fmm &
	make barnes &

luc_radix_fmm_barnes:
	make luc &
	make radix &
	make fmm &
	make barnes &

lunc_radix_fmm_barnes:
	make lunc &
	make radix &
	make fmm &
	make barnes &




luc_lunc:
	make luc &
	make lunc &


barnes_radix:
	make radix &
	make barnes &


barnes_fmm:
	make barnes &
	make fmm &


radix_fmm_barnes:
	make radix &
	make fmm &
	make barnes &

luc_barnes:
	make lunc &
	make barnes &

lunc_barnes:
	make lunc &
	make barnes &

lunc_fmm:
	make lunc &
	make fmm &

luc_radix:
	make luc &
	make radix &


luc_fmm:
	make luc &
	make fmm &


lunc_radix:
	make lunc &
	make radix &


lunc_radix_fmm:
	make lunc &
	make radix &
	make fmm &


luc_radix_fmm:
	make luc &
	make radix &
	make fmm &


lunc_fmm_barnes:
	make lunc &
	make fmm &
	make barnes &
test1:
	ls $(test)

luc:
	perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses $(KERNEL)/lu/contiguous_blocks/LU -n8032 -p16 -b8 -c -s -t >> $(MAIN)/$(LOGFILE).log 2>&1 


lunc:
	perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses $(KERNEL)/lu/non_contiguous_blocks/LU -n8296 -p16 -b16 -s -t >> $(MAIN)/$(LOGFILE).log 2>&1 


radix:
	perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses $(KERNEL)/radix/RADIX -p16 -r8 -n2147483647 -m7 -s  >> $(MAIN)/$(LOGFILE).log 2>&1  


fmm:
	perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses $(APPS)/fmm/FMM < $(APPS)/fmm/inputs/input.16384 >> $(MAIN)/$(LOGFILE).log 2>&1 


#barnes:
#	perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses  $(APPS)/barnes/BARNES < $(APPS)/barnes/input >> $(MAIN)/$(LOGFILE).log 2>&1 




