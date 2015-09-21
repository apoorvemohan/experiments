#line 228 "/home/apoorve/project/splash2_bench/codes/null_macros/c.m4.null.POSIX"

#line 1 "main.C"
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/*  SPLASH Ocean Code                                                    */
/*                                                                       */
/*  This application studies the role of eddy and boundary currents in   */
/*  influencing large-scale ocean movements.  This implementation uses   */
/*  dynamically allocated four-dimensional arrays for grid data storage. */
/*                                                                       */
/*  Command line options:                                                */
/*                                                                       */
/*     -nN : Simulate NxN ocean.  N must be (power of 2)+2.              */
/*     -pP : P = number of processors.  P must be power of 2.            */
/*     -eE : E = error tolerance for iterative relaxation.               */
/*     -rR : R = distance between grid points in meters.                 */
/*     -tT : T = timestep in seconds.                                    */
/*     -s  : Print timing statistics.                                    */
/*     -o  : Print out relaxation residual values.                       */
/*     -h  : Print out command line options.                             */
/*                                                                       */
/*  Default: OCEAN -n130 -p1 -e1e-7 -r20000.0 -t28800.0                  */
/*                                                                       */
/*  NOTE: This code works under both the FORK and SPROC models.          */
/*                                                                       */
/*************************************************************************/


#line 42
#include <pthread.h>
#line 42
#include <sys/time.h>
#line 42
#include <unistd.h>
#line 42
#include <stdlib.h>
#line 42
#define MAX_THREADS 32
#line 42
pthread_t PThreadTable[MAX_THREADS];
#line 42


#define DEFAULT_N      258
#define DEFAULT_P        1
#define DEFAULT_E        1e-7
#define DEFAULT_T    28800.0
#define DEFAULT_R    20000.0
#define UP               0
#define DOWN             1
#define LEFT             2
#define RIGHT            3
#define UPLEFT           4
#define UPRIGHT          5
#define DOWNLEFT         6
#define DOWNRIGHT        7
#define PAGE_SIZE     4096

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "decs.h"

struct multi_struct *multi;
struct global_struct *global;
struct locks_struct *locks;
struct bars_struct *bars;

double ****psi;
double ****psim;
double ***psium;
double ***psilm;
double ***psib;
double ***ga;
double ***gb;
double ****work1;
double ***work2;
double ***work3;
double ****work4;
double ****work5;
double ***work6;
double ****work7;
double ****temparray;
double ***tauz;
double ***oldga;
double ***oldgb;
double *f;
double ****q_multi;
double ****rhs_multi;

long nprocs = DEFAULT_P;
double h1 = 1000.0;
double h3 = 4000.0;
double h = 5000.0;
double lf = -5.12e11;
double res = DEFAULT_R;
double dtau = DEFAULT_T;
double f0 = 8.3e-5;
double beta = 2.0e-11;
double gpr = 0.02;
long im = DEFAULT_N;
long jm;
double tolerance = DEFAULT_E;
double eig2;
double ysca;
long jmm1;
double pi;
double t0 = 0.5e-4 ;
double outday0 = 1.0;
double outday1 = 2.0;
double outday2 = 2.0;
double outday3 = 2.0;
double factjacob;
double factlap;
long numlev;
long *imx;
long *jmx;
double *lev_res;
double *lev_tol;
double maxwork = 10000.0;

struct Global_Private *gp;

double *i_int_coeff;
double *j_int_coeff;
long xprocs;
long yprocs;
long *xpts_per_proc;
long *ypts_per_proc;
long minlevel;
long do_stats = 0;
long do_output = 0;

int main(int argc, char *argv[])
{
   long i;
   long j;
   long k;
   long x_part;
   long y_part;
   long d_size;
   long itemp;
   long jtemp;
   double procsqrt;
   long temp = 0;
   double min_total;
   double max_total;
   double avg_total;
   double min_multi;
   double max_multi;
   double avg_multi;
   double min_frac;
   double max_frac;
   double avg_frac;
   long ch;
   extern char *optarg;
   unsigned long computeend;
   unsigned long start;

   {
#line 161
	struct timeval	FullTime;
#line 161

#line 161
	gettimeofday(&FullTime, NULL);
#line 161
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 161
}

   while ((ch = getopt(argc, argv, "n:p:e:r:t:soh")) != -1) {
     switch(ch) {
     case 'n': im = atoi(optarg);
               if (log_2(im-2) == -1) {
                 printerr("Grid must be ((power of 2)+2) in each dimension\n");
                 exit(-1);
               }
               break;
     case 'p': nprocs = atoi(optarg);
               if (nprocs < 1) {
                 printerr("P must be >= 1\n");
                 exit(-1);
               }
               if (log_2(nprocs) == -1) {
                 printerr("P must be a power of 2\n");
                 exit(-1);
               }
               break;
     case 'e': tolerance = atof(optarg); break;
     case 'r': res = atof(optarg); break;
     case 't': dtau = atof(optarg); break;
     case 's': do_stats = !do_stats; break;
     case 'o': do_output = !do_output; break;
     case 'h': printf("Usage: OCEAN <options>\n\n");
               printf("options:\n");
               printf("  -nN : Simulate NxN ocean.  N must be (power of 2)+2.\n");
               printf("  -pP : P = number of processors.  P must be power of 2.\n");
               printf("  -eE : E = error tolerance for iterative relaxation.\n");
               printf("  -rR : R = distance between grid points in meters.\n");
               printf("  -tT : T = timestep in seconds.\n");
               printf("  -s  : Print timing statistics.\n");
               printf("  -o  : Print out relaxation residual values.\n");
               printf("  -h  : Print out command line options.\n\n");
               printf("Default: OCEAN -n%1d -p%1d -e%1g -r%1g -t%1g\n",
                       DEFAULT_N,DEFAULT_P,DEFAULT_E,DEFAULT_R,DEFAULT_T);
               exit(0);
               break;
     }
   }

   {;}

   jm = im;
   printf("\n");
   printf("Ocean simulation with W-cycle multigrid solver\n");
   printf("    Processors                         : %1ld\n",nprocs);
   printf("    Grid size                          : %1ld x %1ld\n",im,jm);
   printf("    Grid resolution (meters)           : %0.2f\n",res);
   printf("    Time between relaxations (seconds) : %0.0f\n",dtau);
   printf("    Error tolerance                    : %0.7g\n",tolerance);
   printf("\n");

   xprocs = 0;
   yprocs = 0;
   procsqrt = sqrt((double) nprocs);
   j = (long) procsqrt;
   while ((xprocs == 0) && (j > 0)) {
     k = nprocs / j;
     if (k * j == nprocs) {
       if (k > j) {
         xprocs = j;
         yprocs = k;
       } else {
         xprocs = k;
         yprocs = j;
       }
     }
     j--;
   }
   if (xprocs == 0) {
     printerr("Could not find factors for subblocking\n");
     exit(-1);
   }

   minlevel = 0;
   itemp = 1;
   jtemp = 1;
   numlev = 0;
   minlevel = 0;
   while (itemp < (im-2)) {
     itemp = itemp*2;
     jtemp = jtemp*2;
     if ((itemp/yprocs > 1) && (jtemp/xprocs > 1)) {
       numlev++;
     }
   }

   if (numlev == 0) {
     printerr("Must have at least 2 grid points per processor in each dimension\n");
     exit(-1);
   }

/*   imx = (long *) valloc(numlev*sizeof(long));;
   jmx = (long *) valloc(numlev*sizeof(long));;
   lev_res = (double *) valloc(numlev*sizeof(double));;
   lev_tol = (double *) valloc(numlev*sizeof(double));;
   i_int_coeff = (double *) valloc(numlev*sizeof(double));;
   j_int_coeff = (double *) valloc(numlev*sizeof(double));;
   xpts_per_proc = (long *) valloc(numlev*sizeof(long));;
   ypts_per_proc = (long *) valloc(numlev*sizeof(long));;
*/

posix_memalign(&imx ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&jmx ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&lev_res ,sizeof(double *) ,(numlev*sizeof(double)));
posix_memalign(&lev_tol ,sizeof(double *) ,(numlev*sizeof(double)));
posix_memalign(&i_int_coeff ,sizeof(double *) ,(numlev*sizeof(double)));
posix_memalign(&j_int_coeff ,sizeof(double *) ,(numlev*sizeof(double)));
posix_memalign(&xpts_per_proc ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&ypts_per_proc ,sizeof(long *) ,(numlev*sizeof(long)));


   imx[numlev-1] = im;
   jmx[numlev-1] = jm;
   lev_res[numlev-1] = res;
   lev_tol[numlev-1] = tolerance;

   for (i=numlev-2;i>=0;i--) {
     imx[i] = ((imx[i+1] - 2) / 2) + 2;
     jmx[i] = ((jmx[i+1] - 2) / 2) + 2;
     lev_res[i] = lev_res[i+1] * 2;
   }

   for (i=0;i<numlev;i++) {
     xpts_per_proc[i] = (jmx[i]-2) / xprocs;
     ypts_per_proc[i] = (imx[i]-2) / yprocs;
   }
   for (i=numlev-1;i>=0;i--) {
     if ((xpts_per_proc[i] < 2) || (ypts_per_proc[i] < 2)) {
       minlevel = i+1;
       break;
     }
   }

   for (i=0;i<numlev;i++) {
     temp += imx[i];
   }
   temp = 0;
   j = 0;
   for (k=0;k<numlev;k++) {
     for (i=0;i<imx[k];i++) {
       j++;
       temp += jmx[k];
     }
   }

   d_size = nprocs*sizeof(double ***);
/*   psi = (double ****) valloc(d_size);;
   psim = (double ****) valloc(d_size);;
   work1 = (double ****) valloc(d_size);;
   work4 = (double ****) valloc(d_size);;
   work5 = (double ****) valloc(d_size);;
   work7 = (double ****) valloc(d_size);;
   temparray = (double ****) valloc(d_size);;
*/

posix_memalign(&psi ,sizeof(double ****) ,(d_size));
posix_memalign(&psim ,sizeof(double ****) ,(d_size));
posix_memalign(&work1 ,sizeof(double ****) ,(d_size));
posix_memalign(&work4 ,sizeof(double ****) ,(d_size));
posix_memalign(&work5 ,sizeof(double ****) ,(d_size));
posix_memalign(&work7 ,sizeof(double ****) ,(d_size));
posix_memalign(&temparray ,sizeof(double ****) ,(d_size));

   d_size = 2*sizeof(double **);
   for (i=0;i<nprocs;i++) {
/*     psi[i] = (double ***) valloc(d_size);;
     psim[i] = (double ***) valloc(d_size);;
     work1[i] = (double ***) valloc(d_size);;
     work4[i] = (double ***) valloc(d_size);;
     work5[i] = (double ***) valloc(d_size);;
     work7[i] = (double ***) valloc(d_size);;
     temparray[i] = (double ***) valloc(d_size);;
*/

posix_memalign(&psi[i] ,sizeof(double ***) ,(d_size));
posix_memalign(&psim[i] ,sizeof(double ***) ,(d_size));
posix_memalign(&work1[i] ,sizeof(double ***) ,(d_size));
posix_memalign(&work4[i] ,sizeof(double ***) ,(d_size));
posix_memalign(&work5[i] ,sizeof(double ***) ,(d_size));
posix_memalign(&work7[i] ,sizeof(double ***) ,(d_size));
posix_memalign(&temparray[i] ,sizeof(double ***) ,(d_size));

   }

   d_size = nprocs*sizeof(double **);
/*   psium = (double ***) valloc(d_size);;
   psilm = (double ***) valloc(d_size);;
   psib = (double ***) valloc(d_size);;
   ga = (double ***) valloc(d_size);;
   gb = (double ***) valloc(d_size);;
   work2 = (double ***) valloc(d_size);;
   work3 = (double ***) valloc(d_size);;
   work6 = (double ***) valloc(d_size);;
   tauz = (double ***) valloc(d_size);;
   oldga = (double ***) valloc(d_size);;
   oldgb = (double ***) valloc(d_size);;

   gp = (struct Global_Private *) valloc((nprocs+1)*sizeof(struct Global_Private));;
*/


posix_memalign(&psium ,sizeof(double ***) ,(d_size));
posix_memalign(&psilm ,sizeof(double ***) ,(d_size));
posix_memalign(&psib ,sizeof(double ***) ,(d_size));
posix_memalign(&ga ,sizeof(double ***) ,(d_size));
posix_memalign(&gb ,sizeof(double ***) ,(d_size));
posix_memalign(&work2 ,sizeof(double ***) ,(d_size));
posix_memalign(&work3 ,sizeof(double ***) ,(d_size));
posix_memalign(&work6 ,sizeof(double ***) ,(d_size));
posix_memalign(&tauz ,sizeof(double ***) ,(d_size));
posix_memalign(&oldga ,sizeof(double ***) ,(d_size));
posix_memalign(&oldgb ,sizeof(double ***) ,(d_size));
posix_memalign(&gp ,sizeof(struct Global_Private *) ,((nprocs+1)*sizeof(struct Global_Private)));

   for (i=0;i<nprocs;i++) {

posix_memalign(&gp[i].rel_num_x ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].rel_num_y ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].eist ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].ejst ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].oist ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].ojst ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].rlist ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].rljst ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].rlien ,sizeof(long *) ,(numlev*sizeof(long)));
posix_memalign(&gp[i].rljen ,sizeof(long *) ,(numlev*sizeof(long)));

/*     gp[i].rel_num_x = (long *) valloc(numlev*sizeof(long));;
     gp[i].rel_num_y = (long *) valloc(numlev*sizeof(long));;
     gp[i].eist = (long *) valloc(numlev*sizeof(long));;
     gp[i].ejst = (long *) valloc(numlev*sizeof(long));;
     gp[i].oist = (long *) valloc(numlev*sizeof(long));;
     gp[i].ojst = (long *) valloc(numlev*sizeof(long));;
     gp[i].rlist = (long *) valloc(numlev*sizeof(long));;
     gp[i].rljst = (long *) valloc(numlev*sizeof(long));;
     gp[i].rlien = (long *) valloc(numlev*sizeof(long));;
     gp[i].rljen = (long *) valloc(numlev*sizeof(long));;
*/     gp[i].multi_time = 0;
     gp[i].total_time = 0;
   }

   subblock();

   x_part = (jm - 2)/xprocs + 2;
   y_part = (im - 2)/yprocs + 2;

   d_size = x_part*y_part*sizeof(double) + y_part*sizeof(double *);

//   global = (struct global_struct *) valloc(sizeof(struct global_struct));;
posix_memalign(&global ,sizeof(struct global_struct *) ,(sizeof(struct global_struct)));

   for (i=0;i<nprocs;i++) {

posix_memalign(&psi[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&psi[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&psim[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&psim[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&psium[i] ,sizeof(double **) ,(d_size));
posix_memalign(&psilm[i] ,sizeof(double **) ,(d_size));
posix_memalign(&psib[i] ,sizeof(double **) ,(d_size));
posix_memalign(&ga[i] ,sizeof(double **) ,(d_size));
posix_memalign(&gb[i] ,sizeof(double **) ,(d_size));
posix_memalign(&work1[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&work1[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&work2[i] ,sizeof(double **) ,(d_size));
posix_memalign(&work3[i] ,sizeof(double **) ,(d_size));
posix_memalign(&work4[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&work4[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&work5[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&work5[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&work6[i] ,sizeof(double **) ,(d_size));
posix_memalign(&work7[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&work7[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&temparray[i][0] ,sizeof(double **) ,(d_size));
posix_memalign(&temparray[i][1] ,sizeof(double **) ,(d_size));
posix_memalign(&tauz[i] ,sizeof(double **) ,(d_size));
posix_memalign(&oldga[i] ,sizeof(double **) ,(d_size));
posix_memalign(&oldgb[i] ,sizeof(double **) ,(d_size));

/*     psi[i][0] = (double **) valloc(d_size);;
     psi[i][1] = (double **) valloc(d_size);;
     psim[i][0] = (double **) valloc(d_size);;
     psim[i][1] = (double **) valloc(d_size);;
     psium[i] = (double **) valloc(d_size);;
     psilm[i] = (double **) valloc(d_size);;
     psib[i] = (double **) valloc(d_size);;
     ga[i] = (double **) valloc(d_size);;
     gb[i] = (double **) valloc(d_size);;
     work1[i][0] = (double **) valloc(d_size);;
     work1[i][1] = (double **) valloc(d_size);;
     work2[i] = (double **) valloc(d_size);;
     work3[i] = (double **) valloc(d_size);;
     work4[i][0] = (double **) valloc(d_size);;
     work4[i][1] = (double **) valloc(d_size);;
     work5[i][0] = (double **) valloc(d_size);;
     work5[i][1] = (double **) valloc(d_size);;
     work6[i] = (double **) valloc(d_size);;
     work7[i][0] = (double **) valloc(d_size);;
     work7[i][1] = (double **) valloc(d_size);;
     temparray[i][0] = (double **) valloc(d_size);;
     temparray[i][1] = (double **) valloc(d_size);;
     tauz[i] = (double **) valloc(d_size);;
     oldga[i] = (double **) valloc(d_size);;
     oldgb[i] = (double **) valloc(d_size);;
*/   }
//   f = (double *) valloc(im*sizeof(double));;
posix_memalign(&f ,sizeof(double *) ,(im*sizeof(double)));


//   multi = (struct multi_struct *) valloc(sizeof(struct multi_struct));;
posix_memalign(&multi ,sizeof(struct multi_struct *) ,(sizeof(struct multi_struct)));


   d_size = numlev*sizeof(double **);
   if (numlev%2 == 1) {         /* To make sure that the actual data
                                   starts double word aligned, add an extra
                                   pointer */
     d_size += sizeof(double **);
   }
   for (i=0;i<numlev;i++) {
     d_size += ((imx[i]-2)/yprocs+2)*((jmx[i]-2)/xprocs+2)*sizeof(double)+
              ((imx[i]-2)/yprocs+2)*sizeof(double *);
   }

   d_size *= nprocs;

   if (nprocs%2 == 1) {         /* To make sure that the actual data
                                   starts double word aligned, add an extra
                                   pointer */
     d_size += sizeof(double ***);
   }

   d_size += nprocs*sizeof(double ***);
//   q_multi = (double ****) valloc(d_size);;
//   rhs_multi = (double ****) valloc(d_size);;

posix_memalign(&q_multi ,sizeof(double ****) ,(d_size));
posix_memalign(&rhs_multi ,sizeof(double ****) ,(d_size));

//   locks = (struct locks_struct *) valloc(sizeof(struct locks_struct));;
//   bars = (struct bars_struct *) valloc(sizeof(struct bars_struct));;

posix_memalign(&locks ,sizeof(struct locks_struct *) ,(sizeof(struct locks_struct)));
posix_memalign(&bars ,sizeof(struct bars_struct *) ,(sizeof(struct bars_struct)));

   {pthread_mutex_init(&(locks->idlock), NULL);}
   {pthread_mutex_init(&(locks->psiailock), NULL);}
   {pthread_mutex_init(&(locks->psibilock), NULL);}
   {pthread_mutex_init(&(locks->donelock), NULL);}
   {pthread_mutex_init(&(locks->error_lock), NULL);}
   {pthread_mutex_init(&(locks->bar_lock), NULL);}

#if defined(MULTIPLE_BARRIERS)
   {
#line 517
	unsigned long	Error;
#line 517

#line 517
	Error = pthread_mutex_init(&(bars->iteration).mutex, NULL);
#line 517
	if (Error != 0) {
#line 517
		printf("Error while initializing barrier.\n");
#line 517
		exit(-1);
#line 517
	}
#line 517

#line 517
	Error = pthread_cond_init(&(bars->iteration).cv, NULL);
#line 517
	if (Error != 0) {
#line 517
		printf("Error while initializing barrier.\n");
#line 517
		pthread_mutex_destroy(&(bars->iteration).mutex);
#line 517
		exit(-1);
#line 517
	}
#line 517

#line 517
	(bars->iteration).counter = 0;
#line 517
	(bars->iteration).cycle = 0;
#line 517
}
   {
#line 518
	unsigned long	Error;
#line 518

#line 518
	Error = pthread_mutex_init(&(bars->gsudn).mutex, NULL);
#line 518
	if (Error != 0) {
#line 518
		printf("Error while initializing barrier.\n");
#line 518
		exit(-1);
#line 518
	}
#line 518

#line 518
	Error = pthread_cond_init(&(bars->gsudn).cv, NULL);
#line 518
	if (Error != 0) {
#line 518
		printf("Error while initializing barrier.\n");
#line 518
		pthread_mutex_destroy(&(bars->gsudn).mutex);
#line 518
		exit(-1);
#line 518
	}
#line 518

#line 518
	(bars->gsudn).counter = 0;
#line 518
	(bars->gsudn).cycle = 0;
#line 518
}
   {
#line 519
	unsigned long	Error;
#line 519

#line 519
	Error = pthread_mutex_init(&(bars->p_setup).mutex, NULL);
#line 519
	if (Error != 0) {
#line 519
		printf("Error while initializing barrier.\n");
#line 519
		exit(-1);
#line 519
	}
#line 519

#line 519
	Error = pthread_cond_init(&(bars->p_setup).cv, NULL);
#line 519
	if (Error != 0) {
#line 519
		printf("Error while initializing barrier.\n");
#line 519
		pthread_mutex_destroy(&(bars->p_setup).mutex);
#line 519
		exit(-1);
#line 519
	}
#line 519

#line 519
	(bars->p_setup).counter = 0;
#line 519
	(bars->p_setup).cycle = 0;
#line 519
}
   {
#line 520
	unsigned long	Error;
#line 520

#line 520
	Error = pthread_mutex_init(&(bars->p_redph).mutex, NULL);
#line 520
	if (Error != 0) {
#line 520
		printf("Error while initializing barrier.\n");
#line 520
		exit(-1);
#line 520
	}
#line 520

#line 520
	Error = pthread_cond_init(&(bars->p_redph).cv, NULL);
#line 520
	if (Error != 0) {
#line 520
		printf("Error while initializing barrier.\n");
#line 520
		pthread_mutex_destroy(&(bars->p_redph).mutex);
#line 520
		exit(-1);
#line 520
	}
#line 520

#line 520
	(bars->p_redph).counter = 0;
#line 520
	(bars->p_redph).cycle = 0;
#line 520
}
   {
#line 521
	unsigned long	Error;
#line 521

#line 521
	Error = pthread_mutex_init(&(bars->p_soln).mutex, NULL);
#line 521
	if (Error != 0) {
#line 521
		printf("Error while initializing barrier.\n");
#line 521
		exit(-1);
#line 521
	}
#line 521

#line 521
	Error = pthread_cond_init(&(bars->p_soln).cv, NULL);
#line 521
	if (Error != 0) {
#line 521
		printf("Error while initializing barrier.\n");
#line 521
		pthread_mutex_destroy(&(bars->p_soln).mutex);
#line 521
		exit(-1);
#line 521
	}
#line 521

#line 521
	(bars->p_soln).counter = 0;
#line 521
	(bars->p_soln).cycle = 0;
#line 521
}
   {
#line 522
	unsigned long	Error;
#line 522

#line 522
	Error = pthread_mutex_init(&(bars->p_subph).mutex, NULL);
#line 522
	if (Error != 0) {
#line 522
		printf("Error while initializing barrier.\n");
#line 522
		exit(-1);
#line 522
	}
#line 522

#line 522
	Error = pthread_cond_init(&(bars->p_subph).cv, NULL);
#line 522
	if (Error != 0) {
#line 522
		printf("Error while initializing barrier.\n");
#line 522
		pthread_mutex_destroy(&(bars->p_subph).mutex);
#line 522
		exit(-1);
#line 522
	}
#line 522

#line 522
	(bars->p_subph).counter = 0;
#line 522
	(bars->p_subph).cycle = 0;
#line 522
}
   {
#line 523
	unsigned long	Error;
#line 523

#line 523
	Error = pthread_mutex_init(&(bars->sl_prini).mutex, NULL);
#line 523
	if (Error != 0) {
#line 523
		printf("Error while initializing barrier.\n");
#line 523
		exit(-1);
#line 523
	}
#line 523

#line 523
	Error = pthread_cond_init(&(bars->sl_prini).cv, NULL);
#line 523
	if (Error != 0) {
#line 523
		printf("Error while initializing barrier.\n");
#line 523
		pthread_mutex_destroy(&(bars->sl_prini).mutex);
#line 523
		exit(-1);
#line 523
	}
#line 523

#line 523
	(bars->sl_prini).counter = 0;
#line 523
	(bars->sl_prini).cycle = 0;
#line 523
}
   {
#line 524
	unsigned long	Error;
#line 524

#line 524
	Error = pthread_mutex_init(&(bars->sl_psini).mutex, NULL);
#line 524
	if (Error != 0) {
#line 524
		printf("Error while initializing barrier.\n");
#line 524
		exit(-1);
#line 524
	}
#line 524

#line 524
	Error = pthread_cond_init(&(bars->sl_psini).cv, NULL);
#line 524
	if (Error != 0) {
#line 524
		printf("Error while initializing barrier.\n");
#line 524
		pthread_mutex_destroy(&(bars->sl_psini).mutex);
#line 524
		exit(-1);
#line 524
	}
#line 524

#line 524
	(bars->sl_psini).counter = 0;
#line 524
	(bars->sl_psini).cycle = 0;
#line 524
}
   {
#line 525
	unsigned long	Error;
#line 525

#line 525
	Error = pthread_mutex_init(&(bars->sl_onetime).mutex, NULL);
#line 525
	if (Error != 0) {
#line 525
		printf("Error while initializing barrier.\n");
#line 525
		exit(-1);
#line 525
	}
#line 525

#line 525
	Error = pthread_cond_init(&(bars->sl_onetime).cv, NULL);
#line 525
	if (Error != 0) {
#line 525
		printf("Error while initializing barrier.\n");
#line 525
		pthread_mutex_destroy(&(bars->sl_onetime).mutex);
#line 525
		exit(-1);
#line 525
	}
#line 525

#line 525
	(bars->sl_onetime).counter = 0;
#line 525
	(bars->sl_onetime).cycle = 0;
#line 525
}
   {
#line 526
	unsigned long	Error;
#line 526

#line 526
	Error = pthread_mutex_init(&(bars->sl_phase_1).mutex, NULL);
#line 526
	if (Error != 0) {
#line 526
		printf("Error while initializing barrier.\n");
#line 526
		exit(-1);
#line 526
	}
#line 526

#line 526
	Error = pthread_cond_init(&(bars->sl_phase_1).cv, NULL);
#line 526
	if (Error != 0) {
#line 526
		printf("Error while initializing barrier.\n");
#line 526
		pthread_mutex_destroy(&(bars->sl_phase_1).mutex);
#line 526
		exit(-1);
#line 526
	}
#line 526

#line 526
	(bars->sl_phase_1).counter = 0;
#line 526
	(bars->sl_phase_1).cycle = 0;
#line 526
}
   {
#line 527
	unsigned long	Error;
#line 527

#line 527
	Error = pthread_mutex_init(&(bars->sl_phase_2).mutex, NULL);
#line 527
	if (Error != 0) {
#line 527
		printf("Error while initializing barrier.\n");
#line 527
		exit(-1);
#line 527
	}
#line 527

#line 527
	Error = pthread_cond_init(&(bars->sl_phase_2).cv, NULL);
#line 527
	if (Error != 0) {
#line 527
		printf("Error while initializing barrier.\n");
#line 527
		pthread_mutex_destroy(&(bars->sl_phase_2).mutex);
#line 527
		exit(-1);
#line 527
	}
#line 527

#line 527
	(bars->sl_phase_2).counter = 0;
#line 527
	(bars->sl_phase_2).cycle = 0;
#line 527
}
   {
#line 528
	unsigned long	Error;
#line 528

#line 528
	Error = pthread_mutex_init(&(bars->sl_phase_3).mutex, NULL);
#line 528
	if (Error != 0) {
#line 528
		printf("Error while initializing barrier.\n");
#line 528
		exit(-1);
#line 528
	}
#line 528

#line 528
	Error = pthread_cond_init(&(bars->sl_phase_3).cv, NULL);
#line 528
	if (Error != 0) {
#line 528
		printf("Error while initializing barrier.\n");
#line 528
		pthread_mutex_destroy(&(bars->sl_phase_3).mutex);
#line 528
		exit(-1);
#line 528
	}
#line 528

#line 528
	(bars->sl_phase_3).counter = 0;
#line 528
	(bars->sl_phase_3).cycle = 0;
#line 528
}
   {
#line 529
	unsigned long	Error;
#line 529

#line 529
	Error = pthread_mutex_init(&(bars->sl_phase_4).mutex, NULL);
#line 529
	if (Error != 0) {
#line 529
		printf("Error while initializing barrier.\n");
#line 529
		exit(-1);
#line 529
	}
#line 529

#line 529
	Error = pthread_cond_init(&(bars->sl_phase_4).cv, NULL);
#line 529
	if (Error != 0) {
#line 529
		printf("Error while initializing barrier.\n");
#line 529
		pthread_mutex_destroy(&(bars->sl_phase_4).mutex);
#line 529
		exit(-1);
#line 529
	}
#line 529

#line 529
	(bars->sl_phase_4).counter = 0;
#line 529
	(bars->sl_phase_4).cycle = 0;
#line 529
}
   {
#line 530
	unsigned long	Error;
#line 530

#line 530
	Error = pthread_mutex_init(&(bars->sl_phase_5).mutex, NULL);
#line 530
	if (Error != 0) {
#line 530
		printf("Error while initializing barrier.\n");
#line 530
		exit(-1);
#line 530
	}
#line 530

#line 530
	Error = pthread_cond_init(&(bars->sl_phase_5).cv, NULL);
#line 530
	if (Error != 0) {
#line 530
		printf("Error while initializing barrier.\n");
#line 530
		pthread_mutex_destroy(&(bars->sl_phase_5).mutex);
#line 530
		exit(-1);
#line 530
	}
#line 530

#line 530
	(bars->sl_phase_5).counter = 0;
#line 530
	(bars->sl_phase_5).cycle = 0;
#line 530
}
   {
#line 531
	unsigned long	Error;
#line 531

#line 531
	Error = pthread_mutex_init(&(bars->sl_phase_6).mutex, NULL);
#line 531
	if (Error != 0) {
#line 531
		printf("Error while initializing barrier.\n");
#line 531
		exit(-1);
#line 531
	}
#line 531

#line 531
	Error = pthread_cond_init(&(bars->sl_phase_6).cv, NULL);
#line 531
	if (Error != 0) {
#line 531
		printf("Error while initializing barrier.\n");
#line 531
		pthread_mutex_destroy(&(bars->sl_phase_6).mutex);
#line 531
		exit(-1);
#line 531
	}
#line 531

#line 531
	(bars->sl_phase_6).counter = 0;
#line 531
	(bars->sl_phase_6).cycle = 0;
#line 531
}
   {
#line 532
	unsigned long	Error;
#line 532

#line 532
	Error = pthread_mutex_init(&(bars->sl_phase_7).mutex, NULL);
#line 532
	if (Error != 0) {
#line 532
		printf("Error while initializing barrier.\n");
#line 532
		exit(-1);
#line 532
	}
#line 532

#line 532
	Error = pthread_cond_init(&(bars->sl_phase_7).cv, NULL);
#line 532
	if (Error != 0) {
#line 532
		printf("Error while initializing barrier.\n");
#line 532
		pthread_mutex_destroy(&(bars->sl_phase_7).mutex);
#line 532
		exit(-1);
#line 532
	}
#line 532

#line 532
	(bars->sl_phase_7).counter = 0;
#line 532
	(bars->sl_phase_7).cycle = 0;
#line 532
}
   {
#line 533
	unsigned long	Error;
#line 533

#line 533
	Error = pthread_mutex_init(&(bars->sl_phase_8).mutex, NULL);
#line 533
	if (Error != 0) {
#line 533
		printf("Error while initializing barrier.\n");
#line 533
		exit(-1);
#line 533
	}
#line 533

#line 533
	Error = pthread_cond_init(&(bars->sl_phase_8).cv, NULL);
#line 533
	if (Error != 0) {
#line 533
		printf("Error while initializing barrier.\n");
#line 533
		pthread_mutex_destroy(&(bars->sl_phase_8).mutex);
#line 533
		exit(-1);
#line 533
	}
#line 533

#line 533
	(bars->sl_phase_8).counter = 0;
#line 533
	(bars->sl_phase_8).cycle = 0;
#line 533
}
   {
#line 534
	unsigned long	Error;
#line 534

#line 534
	Error = pthread_mutex_init(&(bars->sl_phase_9).mutex, NULL);
#line 534
	if (Error != 0) {
#line 534
		printf("Error while initializing barrier.\n");
#line 534
		exit(-1);
#line 534
	}
#line 534

#line 534
	Error = pthread_cond_init(&(bars->sl_phase_9).cv, NULL);
#line 534
	if (Error != 0) {
#line 534
		printf("Error while initializing barrier.\n");
#line 534
		pthread_mutex_destroy(&(bars->sl_phase_9).mutex);
#line 534
		exit(-1);
#line 534
	}
#line 534

#line 534
	(bars->sl_phase_9).counter = 0;
#line 534
	(bars->sl_phase_9).cycle = 0;
#line 534
}
   {
#line 535
	unsigned long	Error;
#line 535

#line 535
	Error = pthread_mutex_init(&(bars->sl_phase_10).mutex, NULL);
#line 535
	if (Error != 0) {
#line 535
		printf("Error while initializing barrier.\n");
#line 535
		exit(-1);
#line 535
	}
#line 535

#line 535
	Error = pthread_cond_init(&(bars->sl_phase_10).cv, NULL);
#line 535
	if (Error != 0) {
#line 535
		printf("Error while initializing barrier.\n");
#line 535
		pthread_mutex_destroy(&(bars->sl_phase_10).mutex);
#line 535
		exit(-1);
#line 535
	}
#line 535

#line 535
	(bars->sl_phase_10).counter = 0;
#line 535
	(bars->sl_phase_10).cycle = 0;
#line 535
}
   {
#line 536
	unsigned long	Error;
#line 536

#line 536
	Error = pthread_mutex_init(&(bars->error_barrier).mutex, NULL);
#line 536
	if (Error != 0) {
#line 536
		printf("Error while initializing barrier.\n");
#line 536
		exit(-1);
#line 536
	}
#line 536

#line 536
	Error = pthread_cond_init(&(bars->error_barrier).cv, NULL);
#line 536
	if (Error != 0) {
#line 536
		printf("Error while initializing barrier.\n");
#line 536
		pthread_mutex_destroy(&(bars->error_barrier).mutex);
#line 536
		exit(-1);
#line 536
	}
#line 536

#line 536
	(bars->error_barrier).counter = 0;
#line 536
	(bars->error_barrier).cycle = 0;
#line 536
}
#else
   {
#line 538
	unsigned long	Error;
#line 538

#line 538
	Error = pthread_mutex_init(&(bars->barrier).mutex, NULL);
#line 538
	if (Error != 0) {
#line 538
		printf("Error while initializing barrier.\n");
#line 538
		exit(-1);
#line 538
	}
#line 538

#line 538
	Error = pthread_cond_init(&(bars->barrier).cv, NULL);
#line 538
	if (Error != 0) {
#line 538
		printf("Error while initializing barrier.\n");
#line 538
		pthread_mutex_destroy(&(bars->barrier).mutex);
#line 538
		exit(-1);
#line 538
	}
#line 538

#line 538
	(bars->barrier).counter = 0;
#line 538
	(bars->barrier).cycle = 0;
#line 538
}
#endif

   link_all();

   multi->err_multi = 0.0;
   i_int_coeff[0] = 0.0;
   j_int_coeff[0] = 0.0;
   for (i=0;i<numlev;i++) {
     i_int_coeff[i] = 1.0/(imx[i]-1);
     j_int_coeff[i] = 1.0/(jmx[i]-1);
   }

/* initialize constants and variables

   id is a global shared variable that has fetch-and-add operations
   performed on it by processes to obtain their pids.   */

   global->id = 0;
   global->psibi = 0.0;
   pi = atan(1.0);
   pi = 4.*pi;

   factjacob = -1./(12.*res*res);
   factlap = 1./(res*res);
   eig2 = -h*f0*f0/(h1*h3*gpr);

   jmm1 = jm-1 ;
   ysca = ((double) jmm1)*res ;

   im = (imx[numlev-1]-2)/yprocs + 2;
   jm = (jmx[numlev-1]-2)/xprocs + 2;

   if (do_output) {
     printf("                       MULTIGRID OUTPUTS\n");
   }

   {
#line 575
	long	i, Error;
#line 575

#line 575
	for (i = 0; i < (nprocs) - 1; i++) {
#line 575
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(slave), NULL);
#line 575
		if (Error != 0) {
#line 575
			printf("Error in pthread_create().\n");
#line 575
			exit(-1);
#line 575
		}
#line 575
	}
#line 575

#line 575
	slave();
#line 575
};
   {
#line 576
	unsigned long	i, Error;
#line 576
	for (i = 0; i < (nprocs) - 1; i++) {
#line 576
		Error = pthread_join(PThreadTable[i], NULL);
#line 576
		if (Error != 0) {
#line 576
			printf("Error in pthread_join().\n");
#line 576
			exit(-1);
#line 576
		}
#line 576
	}
#line 576
};
   {
#line 577
	struct timeval	FullTime;
#line 577

#line 577
	gettimeofday(&FullTime, NULL);
#line 577
	(computeend) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 577
}

   printf("\n");
   printf("                       PROCESS STATISTICS\n");
   printf("                  Total          Multigrid         Multigrid\n");
   printf(" Proc             Time             Time            Fraction\n");
   printf("    0   %15.0f    %15.0f        %10.3f\n", gp[0].total_time,gp[0].multi_time, gp[0].multi_time/gp[0].total_time);

   if (do_stats) {
     min_total = max_total = avg_total = gp[0].total_time;
     min_multi = max_multi = avg_multi = gp[0].multi_time;
     min_frac = max_frac = avg_frac = gp[0].multi_time/gp[0].total_time;
     for (i=1;i<nprocs;i++) {
       if (gp[i].total_time > max_total) {
         max_total = gp[i].total_time;
       }
       if (gp[i].total_time < min_total) {
         min_total = gp[i].total_time;
       }
       if (gp[i].multi_time > max_multi) {
         max_multi = gp[i].multi_time;
       }
       if (gp[i].multi_time < min_multi) {
         min_multi = gp[i].multi_time;
       }
       if (gp[i].multi_time/gp[i].total_time > max_frac) {
         max_frac = gp[i].multi_time/gp[i].total_time;
       }
       if (gp[i].multi_time/gp[i].total_time < min_frac) {
         min_frac = gp[i].multi_time/gp[i].total_time;
       }
       avg_total += gp[i].total_time;
       avg_multi += gp[i].multi_time;
       avg_frac += gp[i].multi_time/gp[i].total_time;
     }
     avg_total = avg_total / nprocs;
     avg_multi = avg_multi / nprocs;
     avg_frac = avg_frac / nprocs;
     for (i=1;i<nprocs;i++) {
       printf("  %3ld   %15.0f    %15.0f        %10.3f\n", i,gp[i].total_time,gp[i].multi_time, gp[i].multi_time/gp[i].total_time);
     }
     printf("  Avg   %15.0f    %15.0f        %10.3f\n", avg_total,avg_multi,avg_frac);
     printf("  Min   %15.0f    %15.0f        %10.3f\n", min_total,min_multi,min_frac);
     printf("  Max   %15.0f    %15.0f        %10.3f\n", max_total,max_multi,max_frac);
   }
   printf("\n");

   global->starttime = start;
   printf("                       TIMING INFORMATION\n");
   printf("Start time                        : %16lu\n", global->starttime);
   printf("Initialization finish time        : %16lu\n", global->trackstart);
   printf("Overall finish time               : %16lu\n", computeend);
   printf("Total time with initialization    : %16lu\n", computeend-global->starttime);
   printf("Total time without initialization : %16lu\n", computeend-global->trackstart);
   printf("    (excludes first timestep)\n");
   printf("\n");

   {exit(0);}
}

long log_2(long number)
{
  long cumulative = 1;
  long out = 0;
  long done = 0;

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

