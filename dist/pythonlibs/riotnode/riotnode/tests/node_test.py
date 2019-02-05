"""riotnode.node test module."""

import os
import sys
import tempfile

import pytest
import pexpect

import riotnode.node

CURDIR = os.path.dirname(__file__)
APPLICATIONS_DIR = os.path.join(CURDIR, 'utils', 'application')


def test_riotnode_application_dir():
    """Test the creation of a riotnode with an `application_dir`."""
    riotbase = os.path.abspath(os.environ['RIOTBASE'])
    application = os.path.join(riotbase, 'examples/hello-world')
    board = 'native'

    env = {'BOARD': board}
    node = riotnode.node.RIOTNode(application, env)

    assert node.application_directory == application
    assert node.board() == board

    clean_cmd = ['make', '--no-print-directory', '-C', application, 'clean']
    assert node.make_command(['clean']) == clean_cmd


def test_riotnode_curdir():
    """Test the creation of a riotnode with current directory."""
    riotbase = os.path.abspath(os.environ['RIOTBASE'])
    application = os.path.join(riotbase, 'examples/hello-world')
    board = 'native'

    _curdir = os.getcwd()
    _environ = os.environ.copy()
    try:
        os.environ['BOARD'] = board
        os.chdir(application)

        node = riotnode.node.RIOTNode()

        assert node.application_directory == application
        assert node.board() == board
        assert node.make_command(['clean']) == ['make', 'clean']
    finally:
        os.chdir(_curdir)
        os.environ.clear()
        os.environ.update(_environ)


@pytest.fixture(name='app_pidfile_env')
def fixture_app_pidfile_env():
    """Environment to use application pidfile"""
    with tempfile.NamedTemporaryFile() as tmpfile:
        yield {'PIDFILE': tmpfile.name}


def test_running_echo_application(app_pidfile_env):
    """Test basic functionnalities with the 'echo' application."""
    env = {'BOARD': 'board', 'APPLICATION': './echo.py'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')

        # Test multiple echo
        for i in range(16):
            child.sendline('Hello Test {}'.format(i))
            child.expect(r'Hello Test (\d+)', timeout=1)
            num = int(child.match.group(1))
            assert i == num


def test_running_error_cases(app_pidfile_env):
    """Test basic functionnalities with the 'echo' application.

    This tests:
    * stopping already stopped child
    """
    # Use only 'echo' as process to exit directly
    env = {'BOARD': 'board',
           'NODE_WRAPPER': 'echo', 'APPLICATION': 'Starting RIOT node'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')

        # Term is already finished and expect should trigger EOF
        with pytest.raises(pexpect.EOF):
            child.expect('this should eof')

    # Exiting the context manager should not crash when node is killed


def test_expect_not_matching_stdin(app_pidfile_env):
    """Test that expect does not match stdin."""
    env = {'BOARD': 'board', 'APPLICATION': './hello.py'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')
        child.expect_exact('Hello World')

        msg = "This should not be matched as it is on stdin"
        child.sendline(msg)
        matched = child.expect_exact([pexpect.TIMEOUT, msg], timeout=1)
        assert matched == 0
        # This would have matched with `node.run_term(echo=True)`
