#include <stdlib.h>
       #include <unistd.h>
       #include <stdio.h>
       #include <signal.h>
       #include <time.h>

       #define CLOCKID CLOCK_REALTIME
       #define SIG SIGALRM

	FILE *fp = NULL;

       static void
       handler(int sig)
       {
	printf("Hello World!!!\n");
	fflush(fp);
	fprintf(fp, "Hello World\n");
	system("date");
       }

       int
       main(int argc, char *argv[])
       {

	   fp = fopen("test.log", "a");
           timer_t timerid;
           struct sigevent sev;
           struct itimerspec its;
           long long freq_nanosecs;

	   signal(SIGALRM, handler);

           /* Create the timer */

           sev.sigev_notify = SIGEV_SIGNAL;
           sev.sigev_signo = SIGALRM;
           sev.sigev_value.sival_ptr = &timerid;
           timer_create(CLOCKID, &sev, &timerid);

           /* Start the timer */

           freq_nanosecs = atoll("3000000000");
           its.it_value.tv_sec = (size_t)3;
           its.it_value.tv_nsec = (long)0;
           its.it_interval.tv_sec = its.it_value.tv_sec;
           its.it_interval.tv_nsec = its.it_value.tv_nsec;

           timer_settime(timerid, 0, &its, NULL); 


	while(1);

       }

