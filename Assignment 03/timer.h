#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>


#define GET_TIME(now)               \
  do {                              \
    struct timeval _t;              \
    gettimeofday(&_t, NULL);        \
    (now) = _t.tv_sec                \
          + _t.tv_usec / 1000000.0; \
  } while (0)

#endif /* TIMER_H */
