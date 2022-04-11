# Copyright (C) 2022 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
vfs-related shell interactions

Defines vfs-related shell command interactions
"""

from riotctrl.shell import ShellInteraction


class VFS(ShellInteraction):
    """Virtual File System ShellInteraction"""

    @ShellInteraction.check_term
    def vfs_cmd(self, args=None, timeout=-1, async_=False):
        cmd = "vfs"
        if args is not None:
            cmd += " {args}".format(args=" ".join(str(a) for a in args))
        return self.cmd(cmd, timeout=timeout, async_=False)

    def vfs_rmount(self, path, timeout=-1, async_=False):
        return self.vfs_cmd(
            args=(
                "rmount",
                path,
            ),
            timeout=timeout,
            async_=async_,
        )
