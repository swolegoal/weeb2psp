#!/usr/bin/env python3

from os.path import dirname, abspath
from sys import exit
import unittest

from tap import TAPTestRunner

if __name__ == "__main__":
    dir = dirname(abspath(__file__)) + "/" + "python"
    ldr = unittest.TestLoader()
    tests = ldr.discover(dir)

    runner = TAPTestRunner()
    runner.set_stream(True)
    runner.set_format("Tappy Wrappy: {method_name} - {short_description}")
    result = runner.run(tests)

    exit(0 if result.wasSuccessful() else 1)
