# Re2c Experiments
This directory contains my INI-style parser testbed named `conftest`.  It will
eventually get fully integrated into the main `weeb2psp` program in the root of
this project, at which point this directory will probably get deleted.

## Dependencies
- [Scons](https://scons.org/)
- [re2c](http://re2c.org/)
- A sane POSIX-compliant toolchain and system.

## Building
```
$ scons
```

### Debug Printing
To enable the printing of parsed `batch` parameters, uncomment the `DEBUG_PRINT`
macro at the top of `re/conftest.c` like so:
```
 1  #include <errno.h>
 2  #include <limits.h>
 3  #include <stdbool.h>
 4  #include <stdarg.h>
 5  #include <stdio.h>
 6  #include <stdlib.h>
 7  #include <string.h>
 8
 9  // Uncomment to add debug printing of parsed batch vars (slow)
10  #define DEBUG_PRINT
```

- *Don't forget to rerun `scons`!*

## Testing/Running
### SYNOPSIS
```
build/conftest BATCH_FILE
```

The `test.conf` should parse with no issues, and the `error.conf` file should
fail parsing on line 6.
