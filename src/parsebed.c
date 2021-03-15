#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "weebparse.h"
#include "weebutil.h"

extern int errno;

int main(int argc, char *argv[]) {
  FILE *parseme;

  if (argc < 2) {
    die("Usage:  %s CONF_FILE\n", 1, argv[0]);
  } else if (strncmp("-", argv[1], _POSIX_ARG_MAX) == 0) {
    parseme = stdin;
  } else {
    parseme = fopen(argv[1], "r");
  }

  if (parseme == NULL) {
    die("%s: Error %d, can't open \"%s\": %s!\n",
        errno, argv[0], errno, argv[1], strerror(errno));
  }

  return lex_file(parseme, argv[1]);
}
