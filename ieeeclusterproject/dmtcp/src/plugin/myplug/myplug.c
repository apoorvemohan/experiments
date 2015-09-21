/* NOTE:  if you just want to insert your own code at the time of checkpoint
 *  and restart, there are two simpler additional mechanisms:
 *  dmtcpaware, and the MTCP special hook functions:
 *    mtcpHookPreCheckpoint, mtcpHookPostCheckpoint, mtcpHookRestart
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <signal.h>
#include <time.h>

#include "dmtcp.h"

#define CLOCKID CLOCK_REALTIME

int fd[] = {-1, -1, -1, -1, -1, -1, -1, -1};
struct perf_event_attr *pe[8];
int long long count = 0;
FILE *outfp = NULL;
char *filename = NULL;
char *type = NULL;
char *exit_after = NULL;
int flag = 0;
int isrestart = 0;

void read_perf_ctr_val(int i, char *name){

        assert(fd[i] > 0);
        count = 0;
        read(fd[i], &count, sizeof(long long));
        fprintf(outfp, "%s: %lld\n", name, count);
        ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
        close(fd[i]);
}

void read_ctrs(){

        FILE *fp = fopen("/proc/self/status", "r");
        char *line = NULL;
        size_t a = 0;

        while(getline(&line, &a, fp) != -1){
                if(strstr(line, "Name") || strstr(line, "VmRSS"))
                        fprintf(outfp, "%s", line);
        }

        read_perf_ctr_val(0, "PAGE_FAULTS");
        read_perf_ctr_val(1, "CONTEXT_SWITCHES");
        read_perf_ctr_val(2, "CPU_MIGRATIONS");
        read_perf_ctr_val(3, "CPU_CYCLES");
        read_perf_ctr_val(4, "INSTRUCTIONS");
        read_perf_ctr_val(5, "CACHE_REFERENCES");
        read_perf_ctr_val(6, "CACHE_MISSES");
        read_perf_ctr_val(7, "BRANCH_INSTRUCTIONS");

        char buff[20];
        time_t now = time(NULL);
        strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(outfp, "[TIMESTAMP] %s\n\n", buff);
}

static long perf_event_open1(struct perf_event_attr *hw_event, pid_t pid,
                       int cpu, int group_fd, unsigned long flags){

           int ret;
           ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                          group_fd, flags);
           return ret;
}

void initialize_and_start_perf_attr(int i, __u32 type, __u64 config){

        pe[i] = (struct perf_event_attr*)malloc(sizeof(struct perf_event_attr));
        memset(pe[i], 0, sizeof(struct perf_event_attr));
        pe[i]->type = type;
        pe[i]->size = sizeof(struct perf_event_attr);
        pe[i]->config = config;
        pe[i]->disabled = 1;
        pe[i]->exclude_kernel = 1;
        pe[i]->exclude_hv = 1;
        fd[i] = perf_event_open1(pe[i], 0, -1, -1, 0);
        ioctl(fd[i], PERF_EVENT_IOC_RESET, 0);
        ioctl(fd[i], PERF_EVENT_IOC_ENABLE, 0);

        if (fd[i] == -1) {
            fprintf(outfp, "Error opening leader %llx\n", pe[i]->config);
            ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
            exit(EXIT_FAILURE);
        }
}

void invoke_ctr(){

        initialize_and_start_perf_attr(0, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS);
        initialize_and_start_perf_attr(1, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CONTEXT_SWITCHES);
        initialize_and_start_perf_attr(2, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_MIGRATIONS);
        initialize_and_start_perf_attr(3, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
        initialize_and_start_perf_attr(4, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
        initialize_and_start_perf_attr(5, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);
        initialize_and_start_perf_attr(6, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);
        initialize_and_start_perf_attr(7, PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
}

void setup_perf_ctr()
{
        int time = 60;
        invoke_ctr();
//      if((type != NULL) && (strlen(type) !=0) && (strcmp(type, "ONCE") == 0)){
//              char *endptr;
//              time = (int) strtol(exit_after, &endptr, 10);
//      }
        if((strcmp(type, "ONCE") != 0) && (strcmp(type, "BASE") != 0)){
		printf("ALARM SET\n");
                alarm(time);
        }
}

void sigalrm_handler(int a){

	if(strcmp(type, "LAUNCH") != 0){
        	outfp = fopen(filename, "w");
	        read_ctrs();
        	fclose(outfp);
	}

        if( (strcmp(type, "ONCE") == 0) || (strcmp(type, "LAUNCH") == 0)){

                exit(0);
        }
        else if((type != NULL) && (strlen(type) !=0) && (strcmp(type, "INFINITE") == 0)){

		setup_perf_ctr();

        }
}

void dmtcp_event_hook(DmtcpEvent_t event, DmtcpEventData_t *data)
{
  /* NOTE:  See warning in plugin/README about calls to printf here. */
  switch (event) {
  
  case DMTCP_EVENT_INIT:{

	printf("INIT\n");
	type = getenv("STATGEN");
  }
  break;

  case DMTCP_EVENT_RESUME:{

	printf("RESUME\n");
        if((type != NULL) && ((strcmp(type, "ONCE") == 0) || (strcmp(type, "LAUNCH") == 0))){
		printf("In resume\n");
                system("date");
                sigalrm_handler(0);
        }
  }

  break;

  case DMTCP_EVENT_RESUME_USER_THREAD:
  {
      if (data->resumeInfo.isRestart) {

	isrestart = 1;
	printf("THREAD RESUME\n");
//      int dummy = 1;
//      while(dummy);

        timer_t timerid;
        struct sigevent sev;
        struct itimerspec its;
        long long freq_nanosecs;

        filename = getenv("STATFILE");
        type = getenv("STATGEN");

        if((type != NULL) && (strlen(type) !=0))
                signal(SIGALRM, sigalrm_handler);
                
        if((filename != NULL) && strlen(filename) != 0)
                setup_perf_ctr();

      } else {
	;
      }

      break;
  }

  case DMTCP_EVENT_EXIT:{
        if(isrestart && (strcmp(type, "BASE") == 0)){
	    outfp = fopen(filename, "w");
	    read_ctrs();
	    fclose(outfp);
	}
  }  

default:
    break;
  }

  DMTCP_NEXT_EVENT_HOOK(event, data);

}
                             
