#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import sys
import tempfile

from testrunner import run

# Default test over loopback interface
COAP_HOST = "[fd00:dead:beef::1]"

UPDATING_TIMEOUT = 10
MANIFEST_TIMEOUT = 15

USE_ETHOS = int(os.getenv("USE_ETHOS", "1"))
TAP = os.getenv("TAP", "riot0")
TMPDIR = tempfile.TemporaryDirectory()


def start_aiocoap_fileserver():
    aiocoap_process = subprocess.Popen(
        "exec aiocoap-fileserver %s" % TMPDIR.name, shell=True
    )

    return aiocoap_process


def cleanup(aiocoap_process):
    aiocoap_process.kill()
    TMPDIR.cleanup()


def notify(coap_server, client_url, version=None):
    cmd = [
        "make",
        "suit/notify",
        "SUIT_COAP_SERVER={}".format(coap_server),
        "SUIT_CLIENT={}".format(client_url),
    ]
    if version is not None:
        cmd.append("SUIT_NOTIFY_VERSION={}".format(version))
    assert not subprocess.call(cmd)


def publish(server_dir, server_url, app_ver, keys='default', latest_name=None):
    cmd = [
        "make",
        "suit/publish",
        "SUIT_COAP_FSROOT={}".format(server_dir),
        "SUIT_COAP_SERVER={}".format(server_url),
        "APP_VER={}".format(app_ver),
        "RIOTBOOT_SKIP_COMPILE=1",
        "SUIT_KEY={}".format(keys),
    ]
    if latest_name is not None:
        cmd.append("SUIT_MANIFEST_SIGNED_LATEST={}".format(latest_name))

    assert not subprocess.call(cmd)


def wait_for_update(child):
        return child.expect([r"riotboot_flashwrite: processing bytes (\d+)-(\d+)",
                             "riotboot_flashwrite: riotboot flashing "
                             "completed successfully"],
                             timeout=UPDATING_TIMEOUT)


def app_version(child):
    # get version of currently running image
    # "Image Version: 0x00000000"
    child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F:]+)")
    app_ver = int(child.match.group("app_ver"), 16)
    return app_ver


def get_ipv6_addr(child):
    if USE_ETHOS == 0:
        # Get device global address
        child.expect(
            r"inet6 addr: (?P<gladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)"
            "  scope: global  VAL"
        )
        addr = "{}".format(child.match.group("gladdr").lower())
    else:
        # Get device local address
        addr = "fe80::2%{}".format(TAP)
    return addr


def ping6(client):
    # HACK first ping/tx after reset allways fails
    cmd = ["ping", "-6", client, "-c", "1", "-W", "1"]
    subprocess.call(cmd)


def test_invalid_version(child, client, app_ver):
    publish(TMPDIR.name, COAP_HOST, app_ver - 1)
    notify(COAP_HOST, client, app_ver - 1)
    child.expect_exact("suit_coap: trigger received")
    child.expect_exact("suit: verifying manifest signature...")
    child.expect_exact("seq_nr <= running image")


def test_invalid_signature(child, client, app_ver):
    publish(TMPDIR.name, COAP_HOST, app_ver + 1, 'invalid_keys')
    notify(COAP_HOST, client, app_ver + 1)
    child.expect_exact("suit_coap: trigger received")
    child.expect_exact("suit: verifying manifest signature...")
    child.expect_exact("Unable to validate signature")


def test_successful_update(child, client, app_ver):
    for version in [app_ver + 1, app_ver + 2]:
        # Trigger update process, verify it validates manifest correctly
        publish(TMPDIR.name, COAP_HOST, version)
        notify(COAP_HOST, client, version)
        child.expect_exact("suit_coap: trigger received")
        child.expect_exact("suit: verifying manifest signature...")
        child.expect(
            r"riotboot_flashwrite: initializing update to target slot (\d+)",
            timeout=MANIFEST_TIMEOUT,
        )
        target_slot = int(child.match.group(1))
        # Wait for update to complete
        while wait_for_update(child) == 0:
            pass

        # Verify running slot
        child.expect(r"running from slot (\d+)")
        assert target_slot == int(child.match.group(1)), "BOOTED FROM SAME SLOT"
        # HACK: ping after reset so ethos answers all subsequent messages
        ping6(client)


def testfunc(child):
    # Initial Setup, get client address and current app_ver
    child.expect_exact("main(): This is RIOT!")
    current_app_ver = app_version(child)
    client = get_ipv6_addr(child)
    # Wait for suit_coap thread to start
    child.expect_exact("suit_coap: started.")
    # HACK: ping after reset so ethos answers all subsequent messages
    ping6(client)

    def run(func):
        if child.logfile == sys.stdout:
            func(child, "[{}]".format(client), current_app_ver)
        else:
            try:
                func(child, "[{}]".format(client), current_app_ver)
                print(".", end="", flush=True)
            except Exception as e:
                print("FAILED")
                raise e

    run(test_invalid_signature)
    run(test_invalid_version)
    run(test_successful_update)

    print("TEST PASSED")


if __name__ == "__main__":
    try:
        res = 1
        aiocoap_process = start_aiocoap_fileserver()
        # TODO: wait for coap port to be available

        res = run(testfunc, echo=True)

    except Exception as e:
        print(e)
    finally:
        cleanup(aiocoap_process)

    sys.exit(res)
