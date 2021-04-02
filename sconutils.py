import os
import subprocess
import typing
from sys import stderr as STDERR
from sys import exit


def eprint(errmsg: str):
    """Prints `errmsg` to STDERR."""
    print(errmsg, file=STDERR)


def die(errmsg: str):
    """Prints message and exits Python with a status of one."""
    eprint(errmsg)
    exit(1)


def runpretty(args: str, verbose: bool = False):
    """
    Run a given program/shell command and print its output.

    Normal Flow
    ===========
    Takes a string (``args``) containing the program/args (if any) to pass on
    to the shell interpreter to run via the Python ``subprocess`` module.  It
    waits, then prints the ``STDOUT`` of the completed process.

    After that's all done, it returns ``STDOUT``, for the caller's convenience.
    Note that ``STDOUT`` is in its raw, binary form.  If you need ``STDOUT`` in
    Python string form by using Python's built-in ``decode()`` function.  For
    example::

      foo = runpretty('COMMAND [ARGS]').decode()

    Error Handling
    ==============
    If the spawned proccess returns a nonzero exit status, it will print the
    program's ``STDERR`` to the running Python iterpreter's ``STDERR``, cause
    Python to exit with a return status of 1.
    """
    if verbose:
        print("Running: " + args)
    proc = subprocess.Popen(
        args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )

    if proc.wait() == 1:
        print(proc.stdout.read().decode())
        die(proc.stderr.read().decode())

    std_output = proc.stdout.read()
    print(std_output.decode())
    return std_output


def bldlexrs(re_prefix: str = "src"):
    print('Generating re2c lexers from code in "src/re"...')

    for fname in os.listdir(re_prefix + "/re"):
        in_path = re_prefix + "/re/" + fname
        out_path = re_prefix + "/" + fname
        fext = os.path.splitext(fname)[-1]

        if fext == ".c" or fext == ".cpp":
            args = (
                "re2c -i -W --verbose"
                + " "
                + in_path
                + " "
                + "-o"
                + " "
                + out_path
            )
            runpretty(args=args, verbose=True)
