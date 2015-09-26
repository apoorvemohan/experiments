/* NOTE:  if you just want to insert your own code at the time of checkpoint
 *  and restart, there are two simpler additional mechanisms:
 *  dmtcpaware, and the MTCP special hook functions:
 *    mtcpHookPreCheckpoint, mtcpHookPostCheckpoint, mtcpHookRestart
 */

#include <stdio.h>
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
#include <fcntl.h>

#include "dmtcp.h"
#include "jassert.h"

#define CLOCKID CLOCK_REALTIME
#define MAX_LINE_LEN 2000

int fd[] = {-1, -1, -1, -1, -1, -1, -1, -1};
struct perf_event_attr pe[8];
long long count = 0;
FILE *outfp = NULL;
char *filename = NULL;
char *type = NULL;
char *exit_after = NULL;
int flag = 0;
int isrestart = 0;

/* Reads from fd until count bytes are read, or
 * newline encountered.
 *
 * Side effects: Copies the characters, including
 * the newline, read from the fd into the buf.
 *
 * Returns num of chars read on success;
 *         -1 on read failure or invalid args; and
 *         -2 if the buffer is too small
 */
static int
readLine(int fd, char *buf, int count)
{
  int i = 0;
  char c;
  JASSERT(fd >= 0 && buf != NULL);
#define NEWLINE '\n' // Linux, OSX
  while (i < count) {
    ssize_t rc = read(fd, &c, 1);
    if (rc == 0) {
      break;
    } else if (rc < 0) {
      buf[i] = '\0';
      return -1;
    } else {
      buf[i++] = c;
      if (c == NEWLINE) break;
    }
  }
  buf[i] = '\0';
  if (i >= count)
    return -2;
  else
    return i;
}

static void
read_perf_ctr_val(int i, const char *name)
{
  JASSERT(fd[i] > 0);
  count = 0;
  read(fd[i], &count, sizeof(long long));
  fprintf(outfp, "%s: %lld\n", name, count);
  ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
  close(fd[i]);
}

static void
read_ctrs()
{
  int fd = open("/proc/self/status", O_RDONLY);
  JASSERT(fd > 0);
  char line[MAX_LINE_LEN] = {0};
  size_t a = MAX_LINE_LEN;
  while(readLine(fd, line, MAX_LINE_LEN) > 0) {
    if(strstr(line, "Name") || strstr(line, "VmRSS"))
      fprintf(outfp, "%s", line);
    memset(line, 0, MAX_LINE_LEN);
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

static long
perf_event_open1(struct perf_event_attr *hw_event,
                 pid_t pid,
                 int cpu,
                 int group_fd,
                 unsigned long flags)
{
  int ret;
  ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
  return ret;
}

static void
initialize_and_start_perf_attr(int i, __u32 type, __u64 config)
{
  memset(&pe[i], 0, sizeof(struct perf_event_attr));
  pe[i].type = type;
  pe[i].size = sizeof(struct perf_event_attr);
  pe[i].config = config;
  pe[i].disabled = 1;
  pe[i].exclude_kernel = 1;
  pe[i].exclude_hv = 1;
  fd[i] = perf_event_open1(&pe[i], 0, -1, -1, 0);
  if (fd[i] < 0) {
    fprintf(outfp, "Error opening leader %llx\n", pe[i].config);
    ioctl(fd[i], PERF_EVENT_IOC_DISABLE, 0);
    exit(EXIT_FAILURE);
  }
  ioctl(fd[i], PERF_EVENT_IOC_RESET, 0);
  ioctl(fd[i], PERF_EVENT_IOC_ENABLE, 0);

}

static void
invoke_ctr()
{
  initialize_and_start_perf_attr(0, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS);
  initialize_and_start_perf_attr(1, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CONTEXT_SWITCHES);
  initialize_and_start_perf_attr(2, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_MIGRATIONS);
  initialize_and_start_perf_attr(3, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
  initialize_and_start_perf_attr(4, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
  initialize_and_start_perf_attr(5, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);
  initialize_and_start_perf_attr(6, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);
  initialize_and_start_perf_attr(7, PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
}

static void
setup_perf_ctr()
{
  invoke_ctr();
}

void dmtcp_event_hook(DmtcpEvent_t event, DmtcpEventData_t *data)
{
  /* NOTE:  See warning in plugin/README about calls to printf here. */
  switch (event) {
    case DMTCP_EVENT_WRITE_CKPT:
      {
        JTRACE("CHKP");
        filename = getenv("STATFILE");
        if(isrestart){
          JTRACE("WRITE CHKP");
          JASSERT(filename);
          outfp = fopen(filename, "w+");
          if (!outfp) {
            perror("Error opening stats file in w+ mode");
            JASSERT(false);
          }
          read_ctrs();
          fclose(outfp);
          isrestart = 0;
        }
      }
      break;

    case DMTCP_EVENT_RESUME:
      {
        exit(0);
      }
      break;

    case DMTCP_EVENT_RESTART:
      {
        isrestart = 1;
        filename = getenv("STATFILE");
        JTRACE("Filename: ")(filename);
        setup_perf_ctr();
      }

      break;
    case DMTCP_EVENT_RESUME_USER_THREAD:
      {
        filename = getenv("STATFILE");
        JTRACE("Filename: ")(filename);
      }
      break;
    default:
      break;
  }
  DMTCP_NEXT_EVENT_HOOK(event, data);
}
