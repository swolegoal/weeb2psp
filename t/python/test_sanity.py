#!/usr/bin/env python3

import unittest


class TestPythonSanity(unittest.TestCase):
    def test_sanity(self):
        self.assertEqual(1, 1)


if __name__ == "__main__":
    unittest.main()
