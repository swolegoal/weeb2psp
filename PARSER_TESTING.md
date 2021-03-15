# Re2c Experiments
This project contains a build target `build/parsebed` for testing Weeb2PSP's
INI-style parser.

## Dependencies
- [Scons](https://scons.org/)
- [re2c](http://re2c.org/)
- A sane POSIX-compliant toolchain and system.

## Building
```
$ scons build/parsebed
```

### Debug Printing
To enable the printing of parsed batch parameters, rerun `scons` with the
`iniprint=1` flag:
```
$ scons build/parsebed iniprint=1
```

## Testing/Running
### SYNOPSIS
```
build/parsebed BATCH_FILE
```

The `batch-examples/test.batch` should parse with no issues, and the
`batch-examples/error.batch` file should fail parsing (but not crash) on line
12.

### FUZZING
You will need the `afl-fuzz`
([American Fuzzy Lop](https://github.com/google/AFL)) fuzzing program installed.

Build for the fuzzer with `afl-gcc`:
```
$ scons build/parsebed afl=1
```

Run the `afl-fuzz` as root:
```
# afl-fuzz -i batch-examples -o afl-$(date +%y-%m-%d.%s).run -- build/parsebed @@
```

> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**⚠** If this is your first time using `afl-fuzz`, you may need to
> follow its onscreen instructions and run it again.
