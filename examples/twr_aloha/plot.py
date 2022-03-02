#!/usr/bin/env python3
from __future__ import annotations
from dataclasses import dataclass, field
import re
import argparse
import logging
from typing import List, Tuple

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

from data import ExperimentData

LOG_HANDLER = logging.StreamHandler()
LOG_HANDLER.setFormatter(logging.Formatter(logging.BASIC_FORMAT))
LOG_LEVELS = ("debug", "info", "warning", "error", "fatal", "critical")
LOGGER = logging.getLogger("")

PARSER = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
PARSER.add_argument(
    "files", nargs="+", help="Range measurements files in the form <tag>:<path>"
)
PARSER.add_argument(
    "--loglevel", choices=LOG_LEVELS, default="info", help="Python logger log level"
)
PARSER.add_argument(
    "--errors",
    "-e",
    choices=("absolute", "relative"),
    default=None,
    help="Show ranging errors instead of distances",
)
PARSER.add_argument(
    "--plot",
    "-p",
    dest="plot_type",
    choices=("bar", "box", "violin"),
    default="box",
    help="Plot type",
)
PARSER.add_argument(
    "--filter",
    "-f",
    nargs=2,
    type=float,
    help="Filter the range measurements by dropping all samples outside the boundary [d_min, d_max] in meters",
)


def parse_file_spec(file_spec: str) -> Tuple[str, str]:
    file_pattern = re.compile(r"^(?P<tag>.*):(?P<path>.*)$")
    match = file_pattern.search(file_spec)
    assert match, f"malformed filename {file_spec} must be in the form <tag>:<path>"
    return match.group("tag"), match.group("path")


@dataclass
class FramedExperimentData:
    label: str
    exp_data: ExperimentData
    df: pd.DataFrame = field(init=False)

    def __post_init__(self):
        self.df = FramedExperimentData.exp_data_to_df(self.exp_data)

    def filter_df(self, low: float, high: float) -> pd.DataFrame:
        return self.df[(self.df.d_est >= low) & (self.df.d_est <= high)]

    @classmethod
    def from_json_file(cls, file_spec: str) -> FramedExperimentData:
        label, file_path = parse_file_spec(file_spec)

        with open(file_path) as file:
            json_str = "".join(file.readlines())
        exp_data = ExperimentData.from_json_str(json_str).sort()
        return cls(label, exp_data)

    @staticmethod
    def exp_data_to_df(exp_data: ExperimentData):
        df_columns = ["src", "nei", "d", "d_est", "d_err_absolute", "d_err_relative"]
        df_data = []
        for datum in exp_data.data:
            src = datum.node.hwaddr
            for meas in datum.data:
                nei = meas.neighbor
                d = meas.distance
                for d_est in meas.distances:
                    d_err_abs = abs(d - d_est)
                    d_err_rel = d_err_abs / d
                    df_data.append([src, nei, d, d_est, d_err_abs, d_err_rel])
        df = pd.DataFrame(df_data, columns=df_columns)
        return df

    @staticmethod
    def nei_to_distance(df: pd.DataFrame, nei: str, precision: int) -> float:
        # map neighbor id to distance
        d = df[df.nei == nei].drop_duplicates(subset="d", ignore_index=True).d[0]
        return round(d, precision)


@dataclass
class FramedExperimentDataList:
    labels: List[str]
    exp_data_list: List[ExperimentData]
    df: pd.DataFrame = field(init=False)

    def __post_init__(self):
        assert len(self.labels) == len(
            self.exp_data_list
        ), "Not enough labels to build the list"
        dfs = []
        for i, exp_data in enumerate(self.exp_data_list):
            df = FramedExperimentData.exp_data_to_df(exp_data)
            df["method"] = self.labels[i]
            dfs.append(df)
        self.df = pd.concat(dfs, ignore_index=True)

    def filter_df(self, low: float, high: float) -> pd.DataFrame:
        return self.df[(self.df.d_est >= low) & (self.df.d_est <= high)]

    @classmethod
    def from_json_files(cls, file_specs: List[str]) -> FramedExperimentData:
        labels = []
        exp_data_list = []
        for file_spec in file_specs:
            label, file_path = parse_file_spec(file_spec)
            labels.append(label)
            with open(file_path) as file:
                json_str = "".join(file.readlines())
                exp_data = ExperimentData.from_json_str(json_str).sort()
                exp_data_list.append(exp_data)
        return cls(labels, exp_data_list)


PLOT_HANDLERS = {"bar": sns.barplot, "box": sns.boxplot, "violin": sns.violinplot}


def plot_rng_data(
    exp_data: FramedExperimentData,
    errors: str = None,
    precision: int = 2,
    plot_type: str = "box",
    rng_filter_m: Tuple[float, float] = None,
):

    df = (
        exp_data.filter_df(low=rng_filter_m[0], high=rng_filter_m[1])
        if rng_filter_m
        else exp_data.df
    )

    for node, distances in df.groupby("src"):
        fig = plt.figure()
        with sns.axes_style("whitegrid"):
            ax = PLOT_HANDLERS[plot_type](
                x="nei",
                y=f"d_err_{errors}" if errors else "d_est",
                dodge=False,
                data=distances,
            )
            ax.axis("tight")
            ax.set_xlabel("neighbor id")
            # Secondary axis
            ax2 = ax.twiny()
            ax2.set_xlim(ax.get_xlim())
            ax2.set_xticks(ax.get_xticks())
            ax2.set_xticklabels(
                [
                    FramedExperimentData.nei_to_distance(
                        distances, nei_tick_label.get_text(), precision
                    )
                    for nei_tick_label in ax.get_xticklabels()
                ]
            )
            ax2.set_xlabel("neighbor distance")
            fig.suptitle(
                f"UWB ranging {'errors' + f'({errors})' if errors else 'data'} for node {node}",
                fontsize=15,
            )
            fig.tight_layout()


def plot_cmp_rng_data(
    exp_data_list: FramedExperimentDataList,
    errors: str = None,
    precision: int = 2,
    plot_type: str = "violin",
    rng_filter_m: Tuple[float, float] = None,
):
    df = (
        exp_data_list.filter_df(low=rng_filter_m[0], high=rng_filter_m[1])
        if rng_filter_m
        else exp_data_list.df
    )

    for node, distances in df.groupby("src"):
        fig = plt.figure()
        import numpy as np

        with sns.axes_style("whitegrid"):
            ax = PLOT_HANDLERS[plot_type](
                x="nei",
                y=f"d_err_{errors}" if errors else "d_est",
                hue="method",
                data=distances,
            )
            ax.axis("tight")
            ax.set_xlabel("neighbor id")

            # Secondary axis
            ax2 = ax.twiny()
            ax2.set_xlim(ax.get_xlim())
            ax2.set_xticks(ax.get_xticks())
            ax2.set_xticklabels(
                [
                    FramedExperimentData.nei_to_distance(
                        distances, nei.get_text(), precision
                    )
                    for nei in ax.get_xticklabels()
                ]
            )
            ax2.set_xlabel("neighbor distance")

            fig.suptitle(
                f"UWB ranging {'errors' + f'({errors})' if errors else 'data'} for node {node}",
                fontsize=15,
            )

            fig.tight_layout()


def main():
    args = PARSER.parse_args()

    # setup logger
    if args.loglevel:
        loglevel = logging.getLevelName(args.loglevel.upper())
        LOGGER.setLevel(loglevel)

    LOGGER.addHandler(LOG_HANDLER)
    LOGGER.propagate = False

    if len(args.files) == 1:
        exp_data = FramedExperimentData.from_json_file(file_spec=args.files[0])
        print(exp_data.df.head())
        plot_rng_data(
            exp_data,
            errors=args.errors,
            rng_filter_m=args.filter,
            plot_type=args.plot_type,
        )
        plt.show(block=False)
        input("Press enter to exit")
        return

    exp_datas = FramedExperimentDataList.from_json_files(file_specs=args.files)
    plot_cmp_rng_data(
        exp_datas,
        errors=args.errors,
        rng_filter_m=args.filter,
        plot_type=args.plot_type,
    )
    plt.show(block=False)
    input("Press enter to exit")


if __name__ == "__main__":
    main()
