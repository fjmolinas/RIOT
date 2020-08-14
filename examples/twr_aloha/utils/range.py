#!/usr/bin/env python3

import serial
import json
import time
import matplotlib
matplotlib.use('GTK3Agg')
from matplotlib import pyplot as plt
import argparse


def readlines(serial=None, fd=None):
    if serial:
        return serial.readline()
    elif fd:
        return fd.readline()
    else:
        raise ValueError


def run(serial, niter=1000, doblit=True):
    """
    Display the simulation using matplotlib, optionally using blit for speed
    """

    x_range = 60.0
    x_min = 0.0
    x_max = x_range

    y_min = -0.5
    y_max =  2.0

    fig, ax = plt.subplots(1, 1)
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)

    plt.draw()

    if doblit:
        # cache the background
        fig.canvas.draw()
        background = fig.canvas.copy_from_bbox(ax.bbox)
    x = [0]
    y = [0]
    points = ax.plot(x, y, 'k')[0]
    tic = time.time()

    i = 0
    x = []
    y = []
    try:
        while True:
            line = serial.readline()
            try:
                d = json.loads(line)
                utime_s = d['utime'] / 1e6
                raz = d['raz'][0]
            except:
                continue


            if raz > y_max:
                ax.set_ylim(y_min, raz + 0.5)

            if utime_s > x_max:
                ax.set_xlim(utime_s - x_range, utime_s)


            x.append(utime_s)
            y.append(raz)

            points.set_data(x,y)

            if doblit:
                # restore background
                fig.canvas.restore_region(background)

                # redraw just the points
                ax.draw_artist(points)

                # fill in the axes rectangle
                fig.canvas.blit(ax.bbox)

            else:
                # redraw everything
                fig.canvas.draw()
            plt.pause(0.0001)
    except KeyboardInterrupt:
        print("Keyboard interrupt gracefull exit")
    finally:
        plt.close(fig)
        print ("Blit = %s, average FPS: %.2f" % (
            str(doblit), niter / (time.time() - tic)))

PARSER = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
PARSER.add_argument('--port',
                    default='/dev/ttyACM0',
                    help='Serial port to test')
PARSER.add_argument('--baud',
                    type=int,
                    default=115200,
                    help='serial baudrate')
PARSER.add_argument('--file',
                    type=int,
                    default=115200,
                    help='serial baudrate')


if __name__ == '__main__':
    args = PARSER.parse_args()

    serial = serial.Serial(args.port, args.baud, timeout=0)

    serial.write(b"reboot\n")
    time.sleep(0.1)
    serial.write(b"range start\n")

    run(serial, doblit=False)
