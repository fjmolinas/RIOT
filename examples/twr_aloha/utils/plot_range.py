#!/usr/bin/env python3

import json
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.ticker import PercentFormatter
import argparse


PARSER = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
PARSER.add_argument('files',
                    nargs='+',
                    help='Range measurements files')


def _readjson(fd):
    utime = []
    raz = []
    los = []

    for line in fd:
        try:
            d = json.loads(line)
            d_utime = d['utime'] / 1e6
            d_raz = d['raz'][0]
            d_los = d['los'][0]
        except:
            continue

        utime.append(d_utime)
        raz.append(d_raz)
        los.append(d_los)

    return utime, raz, los


def my_plotter(ax, utime, raz, param_dict):
    ax.plot(utime, raz)

if __name__ == '__main__':
    args = PARSER.parse_args()

    fig, axs = plt.subplots(1, 2, tight_layout=True)

    axs[0].set_title("Range estimation from TOF")
    axs[0].set_ylabel("[m]")
    axs[0].set_xlabel("time [s]")
    axs[0].set_xlabel("time [s]")
    axs[0].set_xlim(50, 200)

    for i, file_name in enumerate(args.files):
        with open(file_name) as fd:
            utime, raz, los = _readjson(fd)
            min_u = min(utime)
            utime = [u - min_u for u in utime]
            axs[0].plot(utime, raz, label=file_name, alpha=1 / (i + 1))
            axs[1].hist(raz, bins=40, density=True, label=file_name,)

    axs[0].legend()
    axs[1].legend()
    plt.show()
