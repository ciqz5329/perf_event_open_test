/* See LICENSE file for license and copyright information */

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <sched.h>

#include "timing.h"


#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

inline bool
perf_init(libflush_session_t* session, libflush_session_args_t* args)
{
  (void) session;
  (void) args;

  static struct perf_event_attr attr;
  attr.type = PERF_TYPE_HARDWARE;
  attr.config = PERF_COUNT_HW_CPU_CYCLES;
  attr.size = sizeof(attr);
  attr.exclude_kernel = 1;
  attr.exclude_hv = 1;
  attr.exclude_callchain_kernel = 1;

  session->perf.fd = syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
  assert(session->perf.fd >= 0 && "if this assertion fails you have no perf event interface available for the userspace. install a different kernel/rom."); // if this assertion fails you have no perf event interface available for the userspace. install a different kernel/rom.

  return true;
}

inline bool
perf_terminate(libflush_session_t* session)
{
  close(session->perf.fd);

  return true;
}

inline uint64_t
perf_get_timing(libflush_session_t* session)
{
  long long result = 0;

  if (read(session->perf.fd, &result, sizeof(result)) < (ssize_t) sizeof(result)) {
    return 0;
  }

  return result;
}

inline void
perf_reset_timing(libflush_session_t* session)
{
  ioctl(session->perf.fd, PERF_EVENT_IOC_RESET, 0);
}


