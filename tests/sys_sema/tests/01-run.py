#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('THREAD 1 start')
    child.expect_exact('THREAD 1 sema is posted')
    child.expect_exact('THREAD 2 start')
    child.expect_exact('THREAD 3 start')
    child.expect_exact('THREAD 2 sema not posted, TIMEOUT')
    child.expect_exact('THREAD 1 wake up, post sema')
    child.expect_exact('THREAD 3 sema is posted')
    child.expect_exact('THREAD 0 sema destroyed')

    child.expect_exact('SUCCESS')

if __name__ == "__main__":
    sys.exit(run(testfunc))
