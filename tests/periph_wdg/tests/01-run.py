#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import json
import time
from testrunner import run

# We test only up to 10ms, after wards uC doesn't have time to print
# sysyem time before resetting
reset_times_us = [1e4, 5e4, 1e5, 5e5, 1e6]


def get_reset_time(child):
    reset_time = 0
    reset = False
    while not reset:
        line = child.readline()
        if "main(): This is RIOT!" in line:
            break
        jason = line.strip().split("json:")
        reset_time = json.loads(jason[1])
    return reset_time['time']


def get_start_time(child):
    child.readline()
    child.readline()
    line = child.readline().strip().split("json:")
    start_time = json.loads(line[1])
    return start_time['starttime']


def testfunc(child):
    for rst_time in reset_times_us:
        child.sendline("setup 0 {}".format(rst_time))
        child.expect_exact("[wdg]: - ")
        child.expect_exact(" -")

        if child.before != "invalid configuration time":
            child.sendline("startloop")
            start_time = get_start_time(child)
            reset_time = get_reset_time(child)
            wdg_reset_time = reset_time - start_time
            if wdg_reset_time < rst_time*0.9 or wdg_reset_time > rst_time*1.1:
                print("FAILED target time: {}[us], actual_time: {}[us]"
                      .format(rst_time, wdg_reset_time))
                sys.exit(-1)
            else:
                print("PASSED target time: {}[us], actual_time: {}[us]"
                      .format(rst_time, wdg_reset_time))

    print("TEST PASSED")
if __name__ == "__main__":
    sys.exit(run(testfunc, echo=False))
