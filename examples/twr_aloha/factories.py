import time
from contextlib import ContextDecorator
from riotctrl.ctrl import RIOTCtrlBoardFactory
from iotlab import IoTLABExperiment
import sys
from typing import Tuple, List, Dict


class RIOTCtrlAppFactory(RIOTCtrlBoardFactory, ContextDecorator):
    def __init__(self, board_cls=None):
        super().__init__(board_cls)
        self.ctrl_list = list()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        for ctrl in self.ctrl_list:
            ctrl.stop_term()
        return False

    def get_ctrl(
        self,
        application_directory=".",
        modules=None,
        cflags=None,
        env=None,
        termargs=None,
        flash=True
    ):
        """Returns a RIOTCtrl with its terminal started, if no envs are
        available ot if no env matches the specified 'BOARD' nothing is
        returned

        :param board: riot 'BOARD' type to match in the class 'envs'
        :param app_dir: the application directory
        :param modules: extra modules to add to 'USEMODULE'
        :param cflags: optional 'CFLAGS'
        """
        the_env = dict()
        if env:
            the_env.update(env)
        # add extra MODULES and CFLAGS, extend as needed
        if cflags:
            the_env["CFLAGS"] = cflags
        if modules:
            the_env["USEMODULE"] = modules
        # retrieve a RIOTCtrl Object
        ctrl = super().get_ctrl(
            env=the_env, application_directory=application_directory
        )
        # append ctrl to list
        self.ctrl_list.append(ctrl)
        # flash and start terminal
        if flash:
            ctrl.flash()
        ctrl.reset()
        ctrl.start_term(**termargs)
        # return ctrl with started terminal
        return ctrl


class IoTLABExperimentsFactory(ContextDecorator):
    def __init__(self) -> None:
        super().__init__()
        self.exps = list()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        for exp in self.exps:
            exp.stop()
        return False

    def get_iotlab_experiment_nodes(
        self,
        boards: List[str],
        name: str = "dwm1001-pepper",
        site: str = "lille",
        duration: int = 120,
    ) -> Tuple[IoTLABExperiment, List[Dict]]:
        """Start an experiment on IoT-LAB with boards

        :param boards: list of boards to book for experiment
            can be a list of RIOT 'BOARD', eg:
                ['samr21-xpro','samr21-xpro','iotlab-m3']
            or a list of IoT-LAB nodes 'network_address', eg:
                ['m3-1.grenoble.iot-lab.info', 'dwm1001-1.saclay.iot-lab.info']
        :param name: iotlab-experiment name
        :param site: iotlab-site, defaults 'lille'
        """
        # populate environments based on provided boards
        envs = list()
        for board in boards:
            if IoTLABExperiment.valid_board(board):
                env = {"BOARD": "{}".format(board)}
            else:
                env = {"IOTLAB_NODE": "{}".format(board)}
            envs.append(env)
        exp = IoTLABExperiment(name=name, envs=envs, site=site)
        exp.start(duration=duration)
        self.exps.append(exp)
        return exp, envs
