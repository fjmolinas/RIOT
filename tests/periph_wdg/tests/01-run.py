#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import json
from testrunner import run

def testfunc(child):

    child.sendline('range')
    child.expect_exact("[wdg] range -")
    child.expect_exact("- [wdg] range")

    rst_range = json.loads(child.before)
    rst_max = int(rst_range["max"])
    rst_min = int(rst_range["min"])

    child.sendline("init {}".format(2*rst_max))
    child.expect_exact("[wdg]: invalid configuration time")

    child.sendline("init {}".format(rst_min/2))
    child.expect_exact("[wdg]: invalid configuration time")

    child.sendline("init {}".format(rst_max/2))
    child.sendline("start")
    child.expect_exact("RIOT wdg test application", timeout=rst_max)

if __name__ == "__main__":
    sys.exit(run(testfunc))
