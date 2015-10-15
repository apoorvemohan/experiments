#line 228 "/home/mohan.ap/ipdps/ieeeclusterproject/splash2_bench/codes/null_macros/c.m4.null.POSIX"

#line 1 "radix.C"
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/*  Integer radix sort of non-negative integers.                         */
/*                                                                       */
/*  Command line options:                                                */
/*                                                                       */
/*  -pP : P = number of processors.                                      */
/*  -rR : R = radix for sorting.  Must be power of 2.                    */
/*  -nN : N = number of keys to sort.                                    */
/*  -mM : M = maximum key value.  Integer keys k will be generated such  */
/*        that 0 <= k <= M.                                              */
/*  -s  : Print individual processor timing statistics.                  */
/*  -t  : Check to make sure all keys are sorted correctly.              */
/*  -o  : Print out sorted keys.                                         */
/*  -h  : Print out command line options.                                */
/*                                                                       */
/*  Default: RADIX -p1 -n262144 -r1024 -m524288                          */
/*                                                                       */
/*  Note: This version works under both the FORK and SPROC models        */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define DEFAULT_P                    1
#define DEFAULT_N               262144
#define DEFAULT_R                 1024 
#define DEFAULT_M               524288
#define MAX_PROCESSORS              64    
#define RADIX_S                8388608.0e0
#define RADIX           70368744177664.0e0
#define SEED                 314159265.0e0
#define RATIO               1220703125.0e0
#define PAGE_SIZE                 4096
#define PAGE_MASK     (~(PAGE_SIZE-1))
#define MAX_RADIX                 4096


#line 56
#include <pthread.h>
#line 56
#include <sys/time.h>
#line 56
#include <unistd.h>
#line 56
#include <stdlib.h>
#line 56
#define MAX_THREADS 32
#line 56
pthread_t PThreadTable[MAX_THREADS];
#line 56


struct prefix_node {
   long long densities[MAX_RADIX];
   long long ranks[MAX_RADIX];
   
#line 61
struct {
#line 61
	pthread_mutex_t	Mutex;
#line 61
	pthread_cond_t	CondVar;
#line 61
	unsigned long	Flag;
#line 61
} done;
#line 61

   char pad[PAGE_SIZE];
};

struct global_memory {
   long long Index;                             /* process ID */
   pthread_mutex_t (lock_Index);                    /* for fetch and add to get ID */
   pthread_mutex_t (rank_lock);                     /* for fetch and add to get ID */
/*   pthread_mutex_t section_lock[MAX_PROCESSORS];*/  /* key locks */
   
#line 70
struct {
#line 70
	pthread_mutex_t	mutex;
#line 70
	pthread_cond_t	cv;
#line 70
	unsigned long	counter;
#line 70
	unsigned long	cycle;
#line 70
} (barrier_rank);
#line 70
                   /* for ranking process */
   
#line 71
struct {
#line 71
	pthread_mutex_t	mutex;
#line 71
	pthread_cond_t	cv;
#line 71
	unsigned long	counter;
#line 71
	unsigned long	cycle;
#line 71
} (barrier_key);
#line 71
                    /* for key sorting process */
   double *ranktime;
   double *sorttime;
   double *totaltime;
   long long final;
   unsigned long long starttime;
   unsigned long long rs;
   unsigned long long rf;
   struct prefix_node prefix_tree[2 * MAX_PROCESSORS];
} *global;

struct global_private {
  char pad[PAGE_SIZE];
  long long *rank_ff;         /* overall processor ranks */
} gp[MAX_PROCESSORS];

long long *key[2];            /* sort from one index into the other */
long long **rank_me;          /* individual processor ranks */
long long *key_partition;     /* keys a processor works on */
long long *rank_partition;    /* ranks a processor works on */

long long number_of_processors = DEFAULT_P;
long long max_num_digits;
long long radix = DEFAULT_R;
long long num_keys = DEFAULT_N;
long long max_key = DEFAULT_M;
long long log2_radix;
long long log2_keys;
long long dostats = 0;
long long test_result = 0;
long long doprint = 0;

void slave_sort(void);
double product_mod_46(double t1, double t2);
double ran_num_init(unsigned long long k, double b, double t);
long long get_max_digits(long long max_key);
long long get_log2_radix(long long rad);
long long get_log2_keys(long long num_keys);
long long log_2(long long number);
void printerr(char *s);
void init(long long key_start, long long key_stop, long long from);
void test_sort(long long final);
void printout(void);

int main(int argc, char *argv[])
{
   long long i;
   long long p;
   long long quotient;
   long long remainder;
   long long sum_i; 
   long long sum_f;
   long long size;
   long long **temp;
   long long **temp2;
   long long *a;
   long long c;
   double mint, maxt, avgt;
   double minrank, maxrank, avgrank;
   double minsort, maxsort, avgsort;
   unsigned long long start;


   {
#line 134
	struct timeval	FullTime;
#line 134

#line 134
	gettimeofday(&FullTime, NULL);
#line 134
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 134
};

   while ((c = getopt(argc, argv, "p:r:n:m:stoh")) != -1) {
     switch(c) {
      case 'p': number_of_processors = atoi(optarg);
                if (number_of_processors < 1) {
                  printerr("P must be >= 1\n");
                  exit(-1);
                }
                if (number_of_processors > MAX_PROCESSORS) {
                  printerr("Maximum processors (MAX_PROCESSORS) exceeded\n"); 
		  exit(-1);
		}
                break;
      case 'r': radix = atoi(optarg);
                if (radix < 1) {
                  printerr("Radix must be a power of 2 greater than 0\n");
                  exit(-1);
                }
                log2_radix = log_2(radix);
                if (log2_radix == -1) {
                  printerr("Radix must be a power of 2\n");
                  exit(-1);
                }
                break;
      case 'n': num_keys = atoll(optarg);
                if (num_keys < 1) {
                  printerr("Number of keys must be >= 1\n");
                  exit(-1);
                }
                break;
      case 'm': max_key = atoi(optarg);
                if (max_key < 1) {
                  printerr("Maximum key must be >= 1\n");
                  exit(-1);
                }
                break;
      case 's': dostats = !dostats;
                break;
      case 't': test_result = !test_result;
                break;
      case 'o': doprint = !doprint;
                break;
      case 'h': printf("Usage: RADIX <options>\n\n");
                printf("   -pP : P = number of processors.\n");
                printf("   -rR : R = radix for sorting.  Must be power of 2.\n");
                printf("   -nN : N = number of keys to sort.\n");
                printf("   -mM : M = maximum key value.  Integer keys k will be generated such\n");
                printf("         that 0 <= k <= M.\n");
                printf("   -s  : Print individual processor timing statistics.\n");
                printf("   -t  : Check to make sure all keys are sorted correctly.\n");
                printf("   -o  : Print out sorted keys.\n");
                printf("   -h  : Print out command line options.\n\n");
                printf("Default: RADIX -p%1d -n%1d -r%1d -m%1d\n",
                        DEFAULT_P,DEFAULT_N,DEFAULT_R,DEFAULT_M);
		exit(0);
     }
   }

   {;}

   log2_radix = log_2(radix); 
   log2_keys = log_2(num_keys);
posix_memalign(&global ,sizeof(struct global_memory *) ,(sizeof(struct global_memory)));
//   global = (struct global_memory *) valloc(sizeof(struct global_memory));;
   if (global == NULL) {
	   fprintf(stderr,"ERROR: Cannot malloc enough memory for global\n");
	   exit(-1);
   }
//   key[0] = (long long *) valloc(num_keys*sizeof(long long));;
//   key[1] = (long long *) valloc(num_keys*sizeof(long long));;
 //  key_partition = (long long *) valloc((number_of_processors+1)*sizeof(long long));;
  // rank_partition = (long long *) valloc((number_of_processors+1)*sizeof(long long));;
  // global->ranktime = (double *) valloc(number_of_processors*sizeof(double));;
  // global->sorttime = (double *) valloc(number_of_processors*sizeof(double));;
  // global->totaltime = (double *) valloc(number_of_processors*sizeof(double));;

posix_memalign(&key[0] ,sizeof(long long *) ,(num_keys*sizeof(long long)));
posix_memalign(&key[1] ,sizeof(long long *) ,(num_keys*sizeof(long long)));
posix_memalign(&key_partition ,sizeof(long long *) ,((number_of_processors+1)*sizeof(long long)));
posix_memalign(&rank_partition ,sizeof(long long *) ,((number_of_processors+1)*sizeof(long long)));
posix_memalign(&global->ranktime ,sizeof(double *) ,(number_of_processors*sizeof(double)));
posix_memalign(&global->sorttime ,sizeof(double *) ,(number_of_processors*sizeof(double)));
posix_memalign(&global->totaltime ,sizeof(double *) ,(number_of_processors*sizeof(double)));

   size = number_of_processors*(radix*sizeof(long long)+sizeof(long long *));
//   rank_me = (long long **) valloc(size);;
posix_memalign(&rank_me ,sizeof(long long **) ,(size));
   if ((key[0] == NULL) || (key[1] == NULL) || (key_partition == NULL) || (rank_partition == NULL) || 
       (global->ranktime == NULL) || (global->sorttime == NULL) || (global->totaltime == NULL) || (rank_me == NULL)) {
     fprintf(stderr,"ERROR: Cannot malloc enough memory\n");
     exit(-1); 
   }

   temp = rank_me;
   temp2 = temp + number_of_processors;
   a = (long long *) temp2;
   for (i=0;i<number_of_processors;i++) {
     *temp = (long long *) a;
     temp++;
     a += radix;
   }
   for (i=0;i<number_of_processors;i++) {
//     gp[i].rank_ff = (long long *) valloc(radix*sizeof(long long)+PAGE_SIZE);;

posix_memalign(&gp[i].rank_ff ,sizeof(long long *) ,(radix*sizeof(long long)+PAGE_SIZE));

   }
   {pthread_mutex_init(&(global->lock_Index), NULL);}
   {pthread_mutex_init(&(global->rank_lock), NULL);}
/*   {
#line 244
	unsigned long	i, Error;
#line 244

#line 244
	for (i = 0; i < MAX_PROCESSORS; i++) {
#line 244
		Error = pthread_mutex_init(&global->section_lock[i], NULL);
#line 244
		if (Error != 0) {
#line 244
			printf("Error while initializing array of locks.\n");
#line 244
			exit(-1);
#line 244
		}
#line 244
	}
#line 244
}*/
   {
#line 245
	unsigned long	Error;
#line 245

#line 245
	Error = pthread_mutex_init(&(global->barrier_rank).mutex, NULL);
#line 245
	if (Error != 0) {
#line 245
		printf("Error while initializing barrier.\n");
#line 245
		exit(-1);
#line 245
	}
#line 245

#line 245
	Error = pthread_cond_init(&(global->barrier_rank).cv, NULL);
#line 245
	if (Error != 0) {
#line 245
		printf("Error while initializing barrier.\n");
#line 245
		pthread_mutex_destroy(&(global->barrier_rank).mutex);
#line 245
		exit(-1);
#line 245
	}
#line 245

#line 245
	(global->barrier_rank).counter = 0;
#line 245
	(global->barrier_rank).cycle = 0;
#line 245
}
   {
#line 246
	unsigned long	Error;
#line 246

#line 246
	Error = pthread_mutex_init(&(global->barrier_key).mutex, NULL);
#line 246
	if (Error != 0) {
#line 246
		printf("Error while initializing barrier.\n");
#line 246
		exit(-1);
#line 246
	}
#line 246

#line 246
	Error = pthread_cond_init(&(global->barrier_key).cv, NULL);
#line 246
	if (Error != 0) {
#line 246
		printf("Error while initializing barrier.\n");
#line 246
		pthread_mutex_destroy(&(global->barrier_key).mutex);
#line 246
		exit(-1);
#line 246
	}
#line 246

#line 246
	(global->barrier_key).counter = 0;
#line 246
	(global->barrier_key).cycle = 0;
#line 246
}
   
   for (i=0; i<2*number_of_processors; i++) {
     {
#line 249
	pthread_mutex_init(&global->prefix_tree[i].done.Mutex, NULL);
#line 249
	pthread_cond_init(&global->prefix_tree[i].done.CondVar, NULL);
#line 249
	global->prefix_tree[i].done.Flag = 0;
#line 249
}
#line 249
;
   }

   global->Index = 0;
   max_num_digits = get_max_digits(max_key);
   printf("\n");
   printf("Integer Radix Sort\n");
   printf("     %lld Keys\n",num_keys);
   printf("     %lld Processors\n",number_of_processors);
   printf("     Radix = %lld\n",radix);
   printf("     Max key = %lld\n",max_key);
   printf("\n");

   quotient = num_keys / number_of_processors;
   remainder = num_keys % number_of_processors;
   sum_i = 0;
   sum_f = 0;
   p = 0;
   while (sum_i < num_keys) {
      key_partition[p] = sum_i;
      p++;
      sum_i = sum_i + quotient;
      sum_f = sum_f + remainder;
      sum_i = sum_i + sum_f / number_of_processors;
      sum_f = sum_f % number_of_processors;
   }
   key_partition[p] = num_keys;

   quotient = radix / number_of_processors;
   remainder = radix % number_of_processors;
   sum_i = 0;
   sum_f = 0;
   p = 0;
   while (sum_i < radix) {
      rank_partition[p] = sum_i;
      p++;
      sum_i = sum_i + quotient;
      sum_f = sum_f + remainder;
      sum_i = sum_i + sum_f / number_of_processors;
      sum_f = sum_f % number_of_processors;
   }
   rank_partition[p] = radix;

/* POSSIBLE ENHANCEMENT:  Here is where one might distribute the key,
   rank_me, rank, and gp data structures across physically 
   distributed memories as desired. 
   
   One way to place data is as follows:  
      
   for (i=0;i<number_of_processors;i++) {
     Place all addresses x such that:
       &(key[0][key_partition[i]]) <= x < &(key[0][key_partition[i+1]])
            on node i
       &(key[1][key_partition[i]]) <= x < &(key[1][key_partition[i+1]])
            on node i
       &(rank_me[i][0]) <= x < &(rank_me[i][radix-1]) on node i
       &(gp[i]) <= x < &(gp[i+1]) on node i
       &(gp[i].rank_ff[0]) <= x < &(gp[i].rank_ff[radix]) on node i
   }
   start_p = 0;
   i = 0;

   for (toffset = 0; toffset < number_of_processors; toffset ++) {
     offset = toffset;
     level = number_of_processors >> 1;
     base = number_of_processors;
     while ((offset & 0x1) != 0) {
       offset >>= 1;
       index = base + offset;
       Place all addresses x such that:
         &(global->prefix_tree[index]) <= x < 
              &(global->prefix_tree[index + 1]) on node toffset
       base += level;
       level >>= 1;
     }  
   }  */

   /* Fill the random-number array. */
   
   {
#line 328
	long	i, Error;
#line 328

#line 328
	for (i = 0; i < (number_of_processors) - 1; i++) {
#line 328
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(slave_sort), NULL);
#line 328
		if (Error != 0) {
#line 328
			printf("Error in pthread_create().\n");
#line 328
			exit(-1);
#line 328
		}
#line 328
	}
#line 328

#line 328
	slave_sort();
#line 328
};
   {
#line 329
	unsigned long	i, Error;
#line 329
	for (i = 0; i < (number_of_processors) - 1; i++) {
#line 329
		Error = pthread_join(PThreadTable[i], NULL);
#line 329
		if (Error != 0) {
#line 329
			printf("Error in pthread_join().\n");
#line 329
			exit(-1);
#line 329
		}
#line 329
	}
#line 329
};

   printf("\n");
   printf("                 PROCESS STATISTICS\n");
   printf("               Total            Rank            Sort\n");
   printf(" Proc          Time             Time            Time\n");
   printf("    0     %10.0f      %10.0f      %10.0f\n",
           global->totaltime[0],global->ranktime[0],
           global->sorttime[0]);
   if (dostats) {
     maxt = avgt = mint = global->totaltime[0];
     maxrank = avgrank = minrank = global->ranktime[0];
     maxsort = avgsort = minsort = global->sorttime[0];
     for (i=1; i<number_of_processors; i++) {
       if (global->totaltime[i] > maxt) {
         maxt = global->totaltime[i];
       }
       if (global->totaltime[i] < mint) {
         mint = global->totaltime[i];
       }
       if (global->ranktime[i] > maxrank) {
         maxrank = global->ranktime[i];
       }
       if (global->ranktime[i] < minrank) {
         minrank = global->ranktime[i];
       }
       if (global->sorttime[i] > maxsort) {
         maxsort = global->sorttime[i];
       }
       if (global->sorttime[i] < minsort) {
         minsort = global->sorttime[i];
       }
       avgt += global->totaltime[i];
       avgrank += global->ranktime[i];
       avgsort += global->sorttime[i];
     }
     avgt = avgt / number_of_processors;
     avgrank = avgrank / number_of_processors;
     avgsort = avgsort / number_of_processors;
     for (i=1; i<number_of_processors; i++) {
       printf("  %3lld     %10.0f      %10.0f      %10.0f\n",
               i,global->totaltime[i],global->ranktime[i],
               global->sorttime[i]);
     }
     printf("  Avg     %10.0f      %10.0f      %10.0f\n",avgt,avgrank,avgsort);
     printf("  Min     %10.0f      %10.0f      %10.0f\n",mint,minrank,minsort);
     printf("  Max     %10.0f      %10.0f      %10.0f\n",maxt,maxrank,maxsort);
     printf("\n");
   }

   printf("\n");
   global->starttime = start;
   printf("                 TIMING INFORMATION\n");
   printf("Start time                        : %16llu\n",
           global->starttime);
   printf("Initialization finish time        : %16llu\n",
           global->rs);
   printf("Overall finish time               : %16llu\n",
           global->rf);
   printf("Total time with initialization    : %16llu\n",
           global->rf-global->starttime);
   printf("Total time without initialization : %16llu\n",
           global->rf-global->rs);
   printf("\n");

   if (doprint) {
     printout();
   }
   if (test_result) {
     test_sort(global->final);  
   }
  
   {exit(0);};
}

void slave_sort()
{
   long long i;
   long long MyNum;
   long long this_key;
   long long tmp;
   long long loopnum;
   long long shiftnum;
   long long bb;
   long long my_key;
   long long key_start;
   long long key_stop;
   long long rank_start;
   long long rank_stop;
   long long from=0;
   long long to=1;
   long long *key_density;       /* individual processor key densities */
   unsigned long long time1;
   unsigned long long time2;
   unsigned long long time3;
   unsigned long long time4;
   unsigned long long time5;
   unsigned long long time6;
   double ranktime=0;
   double sorttime=0;
   long long *key_from;
   long long *key_to;
   long long *rank_me_mynum;
   long long *rank_ff_mynum;
   long long stats;
   struct prefix_node* n;
   struct prefix_node* r;
   struct prefix_node* l;
   struct prefix_node* my_node;
   struct prefix_node* their_node;
   long long index;
   long long level;
   long long base;
   long long offset;

   stats = dostats;

   {pthread_mutex_lock(&(global->lock_Index));}
     MyNum = global->Index;
     global->Index++;
   {pthread_mutex_unlock(&(global->lock_Index));}

   {;};
   {;};

/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration */

//   key_density = (long long *) valloc(radix*sizeof(long long));;

posix_memalign(&key_density ,sizeof(long long *) ,(radix*sizeof(long long)));

   /* Fill the random-number array. */

   key_start = key_partition[MyNum];
   key_stop = key_partition[MyNum + 1];
   rank_start = rank_partition[MyNum];
   rank_stop = rank_partition[MyNum + 1];
   if (rank_stop == radix) {
     rank_stop--;
   }

   init(key_start,key_stop,from);

   {
#line 473
	unsigned long	Error, Cycle;
#line 473
	long		Cancel, Temp;
#line 473

#line 473
	Error = pthread_mutex_lock(&(global->barrier_key).mutex);
#line 473
	if (Error != 0) {
#line 473
		printf("Error while trying to get lock in barrier.\n");
#line 473
		exit(-1);
#line 473
	}
#line 473

#line 473
	Cycle = (global->barrier_key).cycle;
#line 473
	if (++(global->barrier_key).counter != (number_of_processors)) {
#line 473
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 473
		while (Cycle == (global->barrier_key).cycle) {
#line 473
			Error = pthread_cond_wait(&(global->barrier_key).cv, &(global->barrier_key).mutex);
#line 473
			if (Error != 0) {
#line 473
				break;
#line 473
			}
#line 473
		}
#line 473
		pthread_setcancelstate(Cancel, &Temp);
#line 473
	} else {
#line 473
		(global->barrier_key).cycle = !(global->barrier_key).cycle;
#line 473
		(global->barrier_key).counter = 0;
#line 473
		Error = pthread_cond_broadcast(&(global->barrier_key).cv);
#line 473
	}
#line 473
	pthread_mutex_unlock(&(global->barrier_key).mutex);
#line 473
} 

/* POSSIBLE ENHANCEMENT:  Here is where one might reset the
   statistics that one is measuring about the parallel execution */

   {
#line 478
	unsigned long	Error, Cycle;
#line 478
	long		Cancel, Temp;
#line 478

#line 478
	Error = pthread_mutex_lock(&(global->barrier_key).mutex);
#line 478
	if (Error != 0) {
#line 478
		printf("Error while trying to get lock in barrier.\n");
#line 478
		exit(-1);
#line 478
	}
#line 478

#line 478
	Cycle = (global->barrier_key).cycle;
#line 478
	if (++(global->barrier_key).counter != (number_of_processors)) {
#line 478
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 478
		while (Cycle == (global->barrier_key).cycle) {
#line 478
			Error = pthread_cond_wait(&(global->barrier_key).cv, &(global->barrier_key).mutex);
#line 478
			if (Error != 0) {
#line 478
				break;
#line 478
			}
#line 478
		}
#line 478
		pthread_setcancelstate(Cancel, &Temp);
#line 478
	} else {
#line 478
		(global->barrier_key).cycle = !(global->barrier_key).cycle;
#line 478
		(global->barrier_key).counter = 0;
#line 478
		Error = pthread_cond_broadcast(&(global->barrier_key).cv);
#line 478
	}
#line 478
	pthread_mutex_unlock(&(global->barrier_key).mutex);
#line 478
} 

   if ((MyNum == 0) || (stats)) {
     {
#line 481
	struct timeval	FullTime;
#line 481

#line 481
	gettimeofday(&FullTime, NULL);
#line 481
	(time1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 481
}
   }

/* Do 1 iteration per digit.  */

   rank_me_mynum = rank_me[MyNum];
   rank_ff_mynum = gp[MyNum].rank_ff;
   for (loopnum=0;loopnum<max_num_digits;loopnum++) {
     shiftnum = (loopnum * log2_radix);
     bb = (radix-1) << shiftnum;

/* generate histograms based on one digit */

     if ((MyNum == 0) || (stats)) {
       {
#line 495
	struct timeval	FullTime;
#line 495

#line 495
	gettimeofday(&FullTime, NULL);
#line 495
	(time2) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 495
}
     }

     for (i = 0; i < radix; i++) {
       rank_me_mynum[i] = 0;
     }  
     key_from = (long long *) key[from];
     key_to = (long long *) key[to];
     for (i=key_start;i<key_stop;i++) {
       my_key = key_from[i] & bb;
       my_key = my_key >> shiftnum;  
       rank_me_mynum[my_key]++;
     }
     key_density[0] = rank_me_mynum[0]; 
     for (i=1;i<radix;i++) {
       key_density[i] = key_density[i-1] + rank_me_mynum[i];  
     }

     {
#line 513
	unsigned long	Error, Cycle;
#line 513
	long		Cancel, Temp;
#line 513

#line 513
	Error = pthread_mutex_lock(&(global->barrier_rank).mutex);
#line 513
	if (Error != 0) {
#line 513
		printf("Error while trying to get lock in barrier.\n");
#line 513
		exit(-1);
#line 513
	}
#line 513

#line 513
	Cycle = (global->barrier_rank).cycle;
#line 513
	if (++(global->barrier_rank).counter != (number_of_processors)) {
#line 513
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 513
		while (Cycle == (global->barrier_rank).cycle) {
#line 513
			Error = pthread_cond_wait(&(global->barrier_rank).cv, &(global->barrier_rank).mutex);
#line 513
			if (Error != 0) {
#line 513
				break;
#line 513
			}
#line 513
		}
#line 513
		pthread_setcancelstate(Cancel, &Temp);
#line 513
	} else {
#line 513
		(global->barrier_rank).cycle = !(global->barrier_rank).cycle;
#line 513
		(global->barrier_rank).counter = 0;
#line 513
		Error = pthread_cond_broadcast(&(global->barrier_rank).cv);
#line 513
	}
#line 513
	pthread_mutex_unlock(&(global->barrier_rank).mutex);
#line 513
}  

     n = &(global->prefix_tree[MyNum]);
     for (i = 0; i < radix; i++) {
        n->densities[i] = key_density[i];
        n->ranks[i] = rank_me_mynum[i];
     }
     offset = MyNum;
     level = number_of_processors >> 1;
     base = number_of_processors;
     if ((MyNum & 0x1) == 0) {
        {
#line 524
	pthread_mutex_lock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);
#line 524
	global->prefix_tree[base + (offset >> 1)].done.Flag = 1;
#line 524
	pthread_cond_broadcast(&global->prefix_tree[base + (offset >> 1)].done.CondVar);
#line 524
	pthread_mutex_unlock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);}
#line 524
;
     }
     while ((offset & 0x1) != 0) {
       offset >>= 1;
       r = n;
       l = n - 1;
       index = base + offset;
       n = &(global->prefix_tree[index]);
       {
#line 532
	pthread_mutex_lock(&n->done.Mutex);
#line 532
	if (n->done.Flag == 0) {
#line 532
		pthread_cond_wait(&n->done.CondVar, &n->done.Mutex);
#line 532
	}
#line 532
};
       {
#line 533
	n->done.Flag = 0;
#line 533
	pthread_mutex_unlock(&n->done.Mutex);}
#line 533
;
       if (offset != (level - 1)) {
         for (i = 0; i < radix; i++) {
           n->densities[i] = r->densities[i] + l->densities[i];
           n->ranks[i] = r->ranks[i] + l->ranks[i];
         }
       } else {
         for (i = 0; i < radix; i++) {
           n->densities[i] = r->densities[i] + l->densities[i];
         }
       }
       base += level;
       level >>= 1;
       if ((offset & 0x1) == 0) {
         {
#line 547
	pthread_mutex_lock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);
#line 547
	global->prefix_tree[base + (offset >> 1)].done.Flag = 1;
#line 547
	pthread_cond_broadcast(&global->prefix_tree[base + (offset >> 1)].done.CondVar);
#line 547
	pthread_mutex_unlock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);}
#line 547
;
       }
     }
     {
#line 550
	unsigned long	Error, Cycle;
#line 550
	long		Cancel, Temp;
#line 550

#line 550
	Error = pthread_mutex_lock(&(global->barrier_rank).mutex);
#line 550
	if (Error != 0) {
#line 550
		printf("Error while trying to get lock in barrier.\n");
#line 550
		exit(-1);
#line 550
	}
#line 550

#line 550
	Cycle = (global->barrier_rank).cycle;
#line 550
	if (++(global->barrier_rank).counter != (number_of_processors)) {
#line 550
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 550
		while (Cycle == (global->barrier_rank).cycle) {
#line 550
			Error = pthread_cond_wait(&(global->barrier_rank).cv, &(global->barrier_rank).mutex);
#line 550
			if (Error != 0) {
#line 550
				break;
#line 550
			}
#line 550
		}
#line 550
		pthread_setcancelstate(Cancel, &Temp);
#line 550
	} else {
#line 550
		(global->barrier_rank).cycle = !(global->barrier_rank).cycle;
#line 550
		(global->barrier_rank).counter = 0;
#line 550
		Error = pthread_cond_broadcast(&(global->barrier_rank).cv);
#line 550
	}
#line 550
	pthread_mutex_unlock(&(global->barrier_rank).mutex);
#line 550
};

     if (MyNum != (number_of_processors - 1)) {
       offset = MyNum;
       level = number_of_processors;
       base = 0;
       while ((offset & 0x1) != 0) {
         offset >>= 1;
         base += level;
         level >>= 1;
       }
       my_node = &(global->prefix_tree[base + offset]);
       offset >>= 1;
       base += level;
       level >>= 1;
       while ((offset & 0x1) != 0) {
         offset >>= 1;
         base += level;
         level >>= 1;
       }
       their_node = &(global->prefix_tree[base + offset]);
       {
#line 571
	pthread_mutex_lock(&my_node->done.Mutex);
#line 571
	if (my_node->done.Flag == 0) {
#line 571
		pthread_cond_wait(&my_node->done.CondVar, &my_node->done.Mutex);
#line 571
	}
#line 571
};
       {
#line 572
	my_node->done.Flag = 0;
#line 572
	pthread_mutex_unlock(&my_node->done.Mutex);}
#line 572
;
       for (i = 0; i < radix; i++) {
         my_node->densities[i] = their_node->densities[i];
       }
     } else {
       my_node = &(global->prefix_tree[(2 * number_of_processors) - 2]);
     }
     offset = MyNum;
     level = number_of_processors;
     base = 0;
     while ((offset & 0x1) != 0) {
       {
#line 583
	pthread_mutex_lock(&global->prefix_tree[base + offset - 1].done.Mutex);
#line 583
	global->prefix_tree[base + offset - 1].done.Flag = 1;
#line 583
	pthread_cond_broadcast(&global->prefix_tree[base + offset - 1].done.CondVar);
#line 583
	pthread_mutex_unlock(&global->prefix_tree[base + offset - 1].done.Mutex);}
#line 583
;
       offset >>= 1;
       base += level;
       level >>= 1;
     }
     offset = MyNum;
     level = number_of_processors;
     base = 0;
     for(i = 0; i < radix; i++) {
       rank_ff_mynum[i] = 0;
     }
     while (offset != 0) {
       if ((offset & 0x1) != 0) {
         /* Add ranks of node to your left at this level */
         l = &(global->prefix_tree[base + offset - 1]);
         for (i = 0; i < radix; i++) {
           rank_ff_mynum[i] += l->ranks[i];
         }
       }
       base += level;
       level >>= 1;
       offset >>= 1;
     }
     for (i = 1; i < radix; i++) {
       rank_ff_mynum[i] += my_node->densities[i - 1];
     }

     if ((MyNum == 0) || (stats)) {
       {
#line 611
	struct timeval	FullTime;
#line 611

#line 611
	gettimeofday(&FullTime, NULL);
#line 611
	(time3) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 611
};
     }

     {
#line 614
	unsigned long	Error, Cycle;
#line 614
	long		Cancel, Temp;
#line 614

#line 614
	Error = pthread_mutex_lock(&(global->barrier_rank).mutex);
#line 614
	if (Error != 0) {
#line 614
		printf("Error while trying to get lock in barrier.\n");
#line 614
		exit(-1);
#line 614
	}
#line 614

#line 614
	Cycle = (global->barrier_rank).cycle;
#line 614
	if (++(global->barrier_rank).counter != (number_of_processors)) {
#line 614
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 614
		while (Cycle == (global->barrier_rank).cycle) {
#line 614
			Error = pthread_cond_wait(&(global->barrier_rank).cv, &(global->barrier_rank).mutex);
#line 614
			if (Error != 0) {
#line 614
				break;
#line 614
			}
#line 614
		}
#line 614
		pthread_setcancelstate(Cancel, &Temp);
#line 614
	} else {
#line 614
		(global->barrier_rank).cycle = !(global->barrier_rank).cycle;
#line 614
		(global->barrier_rank).counter = 0;
#line 614
		Error = pthread_cond_broadcast(&(global->barrier_rank).cv);
#line 614
	}
#line 614
	pthread_mutex_unlock(&(global->barrier_rank).mutex);
#line 614
};

     if ((MyNum == 0) || (stats)) {
       {
#line 617
	struct timeval	FullTime;
#line 617

#line 617
	gettimeofday(&FullTime, NULL);
#line 617
	(time4) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 617
};
     }

     /* put it in order according to this digit */

     for (i = key_start; i < key_stop; i++) {  
       this_key = key_from[i] & bb;
       this_key = this_key >> shiftnum;  
       tmp = rank_ff_mynum[this_key];
       key_to[tmp] = key_from[i];
       rank_ff_mynum[this_key]++;
     }   /*  i */  

     if ((MyNum == 0) || (stats)) {
       {
#line 631
	struct timeval	FullTime;
#line 631

#line 631
	gettimeofday(&FullTime, NULL);
#line 631
	(time5) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 631
};
     }

     if (loopnum != max_num_digits-1) {
       from = from ^ 0x1;
       to = to ^ 0x1;
     }

     {
#line 639
	unsigned long	Error, Cycle;
#line 639
	long		Cancel, Temp;
#line 639

#line 639
	Error = pthread_mutex_lock(&(global->barrier_rank).mutex);
#line 639
	if (Error != 0) {
#line 639
		printf("Error while trying to get lock in barrier.\n");
#line 639
		exit(-1);
#line 639
	}
#line 639

#line 639
	Cycle = (global->barrier_rank).cycle;
#line 639
	if (++(global->barrier_rank).counter != (number_of_processors)) {
#line 639
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 639
		while (Cycle == (global->barrier_rank).cycle) {
#line 639
			Error = pthread_cond_wait(&(global->barrier_rank).cv, &(global->barrier_rank).mutex);
#line 639
			if (Error != 0) {
#line 639
				break;
#line 639
			}
#line 639
		}
#line 639
		pthread_setcancelstate(Cancel, &Temp);
#line 639
	} else {
#line 639
		(global->barrier_rank).cycle = !(global->barrier_rank).cycle;
#line 639
		(global->barrier_rank).counter = 0;
#line 639
		Error = pthread_cond_broadcast(&(global->barrier_rank).cv);
#line 639
	}
#line 639
	pthread_mutex_unlock(&(global->barrier_rank).mutex);
#line 639
}

     if ((MyNum == 0) || (stats)) {
       ranktime += (time3 - time2);
       sorttime += (time5 - time4);
     }
   } /* for */

   {
#line 647
	unsigned long	Error, Cycle;
#line 647
	long		Cancel, Temp;
#line 647

#line 647
	Error = pthread_mutex_lock(&(global->barrier_rank).mutex);
#line 647
	if (Error != 0) {
#line 647
		printf("Error while trying to get lock in barrier.\n");
#line 647
		exit(-1);
#line 647
	}
#line 647

#line 647
	Cycle = (global->barrier_rank).cycle;
#line 647
	if (++(global->barrier_rank).counter != (number_of_processors)) {
#line 647
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 647
		while (Cycle == (global->barrier_rank).cycle) {
#line 647
			Error = pthread_cond_wait(&(global->barrier_rank).cv, &(global->barrier_rank).mutex);
#line 647
			if (Error != 0) {
#line 647
				break;
#line 647
			}
#line 647
		}
#line 647
		pthread_setcancelstate(Cancel, &Temp);
#line 647
	} else {
#line 647
		(global->barrier_rank).cycle = !(global->barrier_rank).cycle;
#line 647
		(global->barrier_rank).counter = 0;
#line 647
		Error = pthread_cond_broadcast(&(global->barrier_rank).cv);
#line 647
	}
#line 647
	pthread_mutex_unlock(&(global->barrier_rank).mutex);
#line 647
}
   if ((MyNum == 0) || (stats)) {
     {
#line 649
	struct timeval	FullTime;
#line 649

#line 649
	gettimeofday(&FullTime, NULL);
#line 649
	(time6) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 649
}
     global->ranktime[MyNum] = ranktime;
     global->sorttime[MyNum] = sorttime;
     global->totaltime[MyNum] = time6-time1;
   }
   if (MyNum == 0) {
     global->rs = time1;
     global->rf = time6;
     global->final = to;
   }

}

/*
 * product_mod_46() returns the product (mod 2^46) of t1 and t2.
 */
double product_mod_46(double t1, double t2)
{
   double a1;
   double b1;
   double a2;
   double b2;
			
   a1 = (double)((long long)(t1 / RADIX_S));    /* Decompose the arguments.  */
   a2 = t1 - a1 * RADIX_S;
   b1 = (double)((long long)(t2 / RADIX_S));
   b2 = t2 - b1 * RADIX_S;
   t1 = a1 * b2 + a2 * b1;      /* Multiply the arguments.  */
   t2 = (double)((long long)(t1 / RADIX_S));
   t2 = t1 - t2 * RADIX_S;
   t1 = t2 * RADIX_S + a2 * b2;
   t2 = (double)((long long)(t1 / RADIX));

   return (t1 - t2 * RADIX);    /* Return the product.  */
}

/*
 * finds the (k)th random number, given the seed, b, and the ratio, t.
 */
double ran_num_init(unsigned long long k, double b, double t)
{
   unsigned long long j;

   while (k != 0) {             /* while() is executed m times
				   such that 2^m > k.  */
      j = k >> 1;
      if ((j << 1) != k) {
         b = product_mod_46(b, t);
      }
      t = product_mod_46(t, t);
      k = j;
   }

   return b;
}

long long get_max_digits(long long max_key)
{
  long long done = 0;
  long long temp = 1;
  long long key_val;

  key_val = max_key;
  while (!done) {
    key_val = key_val / radix;
    if (key_val == 0) {
      done = 1;
    } else {
      temp ++;
    }
  }
  return temp;
}

long long get_log2_radix(long long rad)
{
   long long cumulative=1;
   long long out;

   for (out = 0; out < 20; out++) {
     if (cumulative == rad) {
       return(out);
     } else {
       cumulative = cumulative * 2;
     }
   }
   fprintf(stderr,"ERROR: Radix %lld not a power of 2\n", rad);
   exit(-1);
}

long long get_log2_keys(long long num_keys)
{
   long long cumulative=1;
   long long out;

   for (out = 0; out < 30; out++) {
     if (cumulative == num_keys) {
       return(out);
     } else {
       cumulative = cumulative * 2;
     }
   }
   fprintf(stderr,"ERROR: Number of keys %lld not a power of 2\n", num_keys);
   exit(-1);
}

long long log_2(long long number)
{
  long long cumulative = 1;
  long long out = 0;
  long long done = 0;

  while ((cumulative < number) && (!done) && (out < 50)) {
    if (cumulative == number) {
      done = 1;
    } else {
      cumulative = cumulative * 2;
      out ++;
    }
  }

  if (cumulative == number) {
    return(out);
  } else {
    return(-1);
  }
}

void printerr(char *s)
{
  fprintf(stderr,"ERROR: %s\n",s);
}

void init(long long key_start, long long key_stop, long long from)
{
   double ran_num;
   double sum;
   long long tmp;
   long long i;
   long long *key_from;

   ran_num = ran_num_init((key_start << 2) + 1, SEED, RATIO);
   sum = ran_num / RADIX;
   key_from = (long long *) key[from];
   for (i = key_start; i < key_stop; i++) {
      ran_num = product_mod_46(ran_num, RATIO);
      sum = sum + ran_num / RADIX;
      ran_num = product_mod_46(ran_num, RATIO);
      sum = sum + ran_num / RADIX;
      ran_num = product_mod_46(ran_num, RATIO);
      sum = sum + ran_num / RADIX;
      key_from[i] = (long long) ((sum / 4.0) *  max_key);
      tmp = (long long) ((key_from[i])/100);
      ran_num = product_mod_46(ran_num, RATIO);
      sum = ran_num / RADIX;
   }
}

void test_sort(long long final)
{
   long long i;
   long long mistake = 0;
   long long *key_final;

   printf("\n");
   printf("                  TESTING RESULTS\n");
   key_final = key[final];
   for (i = 0; i < num_keys-1; i++) {
     if (key_final[i] > key_final[i + 1]) {
       fprintf(stderr,"error with key %lld, value %lld %lld \n",
        i,key_final[i],key_final[i + 1]);
       mistake++;
     }
   }

   if (mistake) {
      printf("FAILED: %lld keys out of place.\n", mistake);
   } else {
      printf("PASSED: All keys in place.\n");
   }
   printf("\n");
}

void printout()
{
   long long i;
   long long *key_final;

   key_final = (long long *) key[global->final];
   printf("\n");
   printf("                 SORTED KEY VALUES\n");
   printf("%8lld ",key_final[0]);
   for (i = 0; i < num_keys-1; i++) {
     printf("%8lld ",key_final[i+1]);
     if ((i+2)%5 == 0) {
       printf("\n");
     }
   }
   printf("\n");
}

