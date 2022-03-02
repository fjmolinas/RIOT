import argparse
from contextlib import ExitStack
from curses import flash
import logging
from mimetypes import init
import os
import shutil
import sys
from pexpect.exceptions import TIMEOUT
from data import ExperimentNode, MeasurementData, NodeData, TWRData, ExperimentData
from dacite import from_dict
import time

from riotctrl_shell.sys import Reboot
from twr_shell import TwrCmd, TwrIfconfigParser

from factories import RIOTCtrlAppFactory, IoTLABExperimentsFactory

DEFAULT_DIRECTORY = "logs"
DEFAULT_LOGFILE = "out"

LOG_HANDLER = logging.StreamHandler()
LOG_HANDLER.setFormatter(logging.Formatter(logging.BASIC_FORMAT))
LOG_LEVELS = ("debug", "info", "warning", "error", "fatal", "critical")
LOGGER = logging.getLogger("")

USAGE_EXAMPLE = """example:
"""

PARSER = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter, epilog=USAGE_EXAMPLE
)
PARSER.add_argument("app_dir", help="applications directory")
PARSER.add_argument(
    "--log-directory", "--ld", default=DEFAULT_DIRECTORY, help="Logs directory"
)
PARSER.add_argument("--logfile-name", "--ln", default=DEFAULT_LOGFILE, help="Logs file")
PARSER.add_argument(
    "--log-nodes", action="store_true", help="pipes node logs to stdout"
)
PARSER.add_argument(
    "--num-devices", "-n", type=int, default=2, help="dwm1001 devices to use"
)
PARSER.add_argument("--bias", "-b", action="store_true", help="use bias correction")
PARSER.add_argument("--site", "-s", default="lille", help="FIT IoTLAB site")
PARSER.add_argument(
    "--iotlab-nodes",
    nargs="+",
    default=None,
    help="List of iotlab-nodes network-addresses",
)
PARSER.add_argument(
    "--loglevel",
    "-l",
    choices=LOG_LEVELS,
    default="info",
    help="Python logger log level",
)
PARSER.add_argument(
    "--jobs",
    "-j",
    type=int,
    default=None,
    help="Parallel building (0 means not limit, like '--jobs')",
)
PARSER.add_argument(
    "--firmware",
    "-fw",
    default=None,
    help="Firmware File",
)

APPLICATION = "."


class TwrShell(Reboot, TwrCmd):
    """Convenience class inheriting from the Reboot and TwrCmd shell"""

    _netif = {
        "netif": None,
        "hwaddr": None,
        "hwaddr64": None,
        "panid": None,
        "channel": None,
    }

    def parse_netif(self):
        parser = TwrIfconfigParser()
        self._netif = parser.parse(self.ifconfig())

    def hwaddr(self):
        return self._netif["hwaddr"]

    def netif(self):
        return self._netif["netif"]

    def hwaddr64(self):
        return self._netif["hwaddr64"]

    def panid(self):
        return self._netif["panid"]

    def channel(self):
        return self._netif["channel"]


def twr_range(
    initiator: TwrShell,
    responder: TwrShell,
    alg="ss",
    itvl=100,
    count=100,
    retry=3,
    bias=False,
):
    """Starts ranging activity between: tag->anchor using algorithm
       'alg' for 'time'

    :param alg: twr algo 'ss','ss-ack','ss-ext','ds,'ds-ack'
    :param itvl: requests interval
    :param count: requests count
    """
    for i in range(0, retry):
        LOGGER.debug(f"TWR range try {i}")
        try:
            # perform requests
            out = initiator.twr_req(
                addr=responder.hwaddr(),
                itvl=itvl,
                proto=alg,
                count=count,
                timeout=itvl * count * 1.1 + 1,
                bias=bias,
            )
            data = []
            for line in out.splitlines():
                try:
                    twr_data = TWRData.from_json_str(line)
                    if (
                        twr_data.dst == responder.hwaddr()
                        and twr_data.src == initiator.hwaddr()
                    ):
                        data.append(twr_data.d_m)
                    else:
                        LOGGER.info(
                            f"ERROR, expected src=({initiator.hwaddr()}),dst=({responder.hwaddr()})"
                            f"            got src=({twr_data.src}),dst=({twr_data.dst})"
                        )
                except ValueError:
                    LOGGER.debug(f"JSONDecodeError on: '{line}'")
            return data
        except TIMEOUT:
            LOGGER.debug(f"TWR range try {i} ERROR")
            continue


def run(devices, app_dir, bias, site, fw=None, log_nodes=False, jobs=None):
    """ """
    exp_data = ExperimentData([])

    termargs = {}
    if log_nodes:
        termargs["logfile"] = sys.stdout

    # start iotlab experiment and recover list of nodes
    with ExitStack() as es:
        LOGGER.info("SetUp IotLab Experiment")
        # create factories for IoT-LAB experiments and RIOT nodes
        iotlab_factory = es.enter_context(IoTLABExperimentsFactory())
        factory = es.enter_context(RIOTCtrlAppFactory())
        # start experience, recover environments
        exp, exp_envs = iotlab_factory.get_iotlab_experiment_nodes(devices, site=site)
        # get nodes positions and network addresses
        nodes = []
        for node in exp.get_nodes_position():
            nodes.append(from_dict(data_class=ExperimentNode, data=node))

        if fw:
            exp.flash(fw)
        for i, node in enumerate(nodes):
            LOGGER.info(f"Flash device {i + 1}/{len(devices)} : {node.node_id} ...")
            for env in exp_envs:
                if node.network_address == env["IOTLAB_NODE"]:
                    ctrl = factory.get_ctrl(
                        application_directory=app_dir,
                        env=env,
                        termargs=termargs,
                        flash=(fw is None),
                    )
                    shell = TwrShell(ctrl)
                    node.shell = shell

        # give some time for all terminals to start
        time.sleep(3)

        for node in nodes:
            node.shell.parse_netif()
            node.hwaddr = node.shell.hwaddr()
            LOGGER.info(f"{node.node_id} [{node.hwaddr}] OK")

        for node in nodes:
            neighbors = nodes.copy()
            neighbors.remove(node)
            data = []
            for neighbor in neighbors:
                LOGGER.info(f"TWR range {node.hwaddr} -> {neighbor.hwaddr}")
                try:
                    distances = twr_range(
                        node.shell,
                        neighbor.shell,
                        alg="ss",
                        itvl=10,
                        count=100,
                        bias=bias,
                    )
                except:
                    distances = []
                distance = node.distance(neighbor)
                data.append(MeasurementData(distances, distance, neighbor.hwaddr))
            exp_data.data.append(NodeData(node, data))

    return exp_data


def create_directory(directory, clean=False, mode=0o755):
    """Directory creation helper with `clean` option.

    :param clean: tries deleting the directory before re-creating it
    """
    if clean:
        try:
            shutil.rmtree(directory)
        except OSError:
            pass
    os.makedirs(directory, mode=mode, exist_ok=True)


def create_and_dump(json_data, directory, name):
    """Create file with name <name>.json in 'directory' and dumps
    json_data do file
    """
    file_name = "{}.json".format(name)
    LOGGER.info("logging to file {}".format(file_name))
    file_path = os.path.join(directory, file_name)
    if os.path.exists(file_path):
        LOGGER.warning(f"File {file_name} already exists, overwriting")
    try:
        with open(file_path, "w") as f:
            f.write("{}\n".format(json_data))
    except OSError as err:
        sys.exit("Failed to create a log file: {}".format(err))
    return file_path


def main(args=None):
    args = PARSER.parse_args()

    # setup logger
    if args.loglevel:
        loglevel = logging.getLevelName(args.loglevel.upper())
        LOGGER.setLevel(loglevel)

    LOGGER.addHandler(LOG_HANDLER)
    LOGGER.propagate = False

    # parse args
    app_dir = args.app_dir
    num_devices = args.num_devices
    iotlab_nodes = args.iotlab_nodes
    log_directory = args.log_directory
    # create directory if non existant
    create_directory(log_directory)

    # setup experiments and run
    if iotlab_nodes:
        devices = iotlab_nodes
    else:
        devices = ["dwm1001"] * num_devices

    exp_data = run(
        devices=devices,
        app_dir=app_dir,
        bias=args.bias,
        site=args.site,
        log_nodes=args.log_nodes,
        jobs=args.jobs,
        fw=args.firmware,
    )
    for node in exp_data.data:
        node.node.shell = None  # not JSON serializable

    # dump json
    create_and_dump(exp_data.to_json_str(indent=2), log_directory, args.logfile_name)


if __name__ == "__main__":
    main()
