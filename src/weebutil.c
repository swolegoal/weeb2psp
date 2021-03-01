#include "weebutil.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void die(const char *msg, int status, ...) {
  va_list args;
  va_start(args, status);
  vfprintf(stderr, msg, args);
  va_end(args);

  exit(status);
}
