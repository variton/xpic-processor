#!/usr/bin/env python3
"""Run a configured benchmark and report timing percentiles in milliseconds."""

import argparse
import json
import math
import os
import re
import shlex
from pathlib import Path
import subprocess
import sys

from html_report import render_report


def expand_environment(argument):
    """Expand $NAME and ${NAME} once, preserving each argument as one string."""
    def replace(match):
        name = match.group(1) or match.group(2)
        if name not in os.environ:
            raise ValueError(f"environment variable {name} is not set")
        return os.environ[name]

    return re.sub(r"\$\{([A-Za-z_][A-Za-z0-9_]*)\}|\$([A-Za-z_][A-Za-z0-9_]*)",
                  replace, argument)


def configured_command(config):
    """Read named benchmark settings relative to the configuration directory."""
    configuration = json.loads(config.read_text(encoding="utf-8"))
    if not isinstance(configuration, dict) or set(configuration) != {"command"}:
        raise ValueError('configuration must be a JSON object with a "command" field')
    settings = configuration["command"]
    if not isinstance(settings, dict):
        raise ValueError('"command" must be an object containing benchmark settings')
    allowed = {"runner", "xtimes", "binary_path", "original_image_path",
               "watermarked_image_path", "wartermarked_image_path", "json", "log"}
    unknown = set(settings) - allowed
    if unknown:
        raise ValueError(f"unknown command settings: {', '.join(sorted(unknown))}")
    if "watermarked_image_path" in settings and "wartermarked_image_path" in settings:
        raise ValueError("specify only one watermarked image path field")

    def path_setting(name, default=None):
        value = settings.get(name, default)
        if not isinstance(value, str) or not value or "\0" in value:
            raise ValueError(f"{name} must be a nonempty path string")
        value = expand_environment(value)
        if not value:
            raise ValueError(f"{name} expands to an empty path")
        return value

    count = settings.get("xtimes")
    if isinstance(count, bool) or not isinstance(count, int) or not 1 <= count <= 2147483647:
        raise ValueError("xtimes must be an integer between 1 and 2147483647")
    directory = config.resolve().parent
    runner = (directory / path_setting("runner", "./run.py")).resolve()
    if runner.name != "run.py":
        raise ValueError("runner must point to run.py")
    results_file = directory / path_setting("json", "res.json")
    watermarked_key = ("wartermarked_image_path" if "wartermarked_image_path" in settings
                       else "watermarked_image_path")
    command = [path_setting("binary_path"), path_setting("original_image_path"),
               path_setting(watermarked_key)]
    invocation = [sys.executable, str(runner), f"-xtimes={count}",
                  f"--json={results_file}"]
    if "log" in settings:
        log_file = directory / path_setting("log")
        if log_file.resolve() == config.resolve() or (
            log_file.exists() and log_file.samefile(config)
        ):
            raise ValueError("log file must not overwrite the configuration")
        invocation.append(f"--log={log_file}")
    invocation.extend(["--", *command])
    return invocation, directory, results_file, count


def timing_percentiles(results):
    """Use linear interpolation at index (sample_count - 1) * percentile / 100."""
    if not isinstance(results, list) or not results:
        raise ValueError("expected a nonempty JSON array of benchmark results")
    times = []
    for index, result in enumerate(results, start=1):
        value = result.get("time_elapsed_ms") if isinstance(result, dict) else None
        if (isinstance(value, bool) or not isinstance(value, (int, float))
                or not math.isfinite(value) or value < 0):
            raise ValueError(f"result {index} has no valid time_elapsed_ms")
        times.append(value)
    times.sort()
    percentiles = {}
    for percentile in (50, 80, 90, 95):
        position = (len(times) - 1) * percentile / 100
        lower = math.floor(position)
        upper = math.ceil(position)
        percentiles[f"p{percentile}"] = (
            times[lower] + (times[upper] - times[lower]) * (position - lower)
        )
    return percentiles


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--input", type=Path,
                        help="read an existing results JSON file instead of running the benchmark")
    source.add_argument("--conf", type=Path,
                        help="JSON benchmark settings supporting $NAME/${NAME}; paths are relative to the file")
    parser.add_argument("--html", type=Path, default=Path("report.html"),
                        help="HTML report destination (default: report.html)")
    args = parser.parse_args()
    directory = Path(__file__).resolve().parent
    results_file = args.input if args.input is not None else directory / "res.json"
    try:
        expected_count = 50
        invocation = [sys.executable, str(directory / "run.py"), "-xtimes=50", "--json=res.json",
                      "../bin/mt-mean-ssim", "../rc/ur.jpg", "../rc/watermarked_ur.jpg"]
        if args.conf is not None:
            invocation, directory, results_file, expected_count = configured_command(args.conf)
            for destination in (results_file, args.html):
                if destination.resolve() == args.conf.resolve() or (
                    destination.exists() and destination.samefile(args.conf)
                ):
                    raise ValueError("output files must not overwrite the configuration")
        if args.html.resolve() == results_file.resolve() or (
            args.html.exists() and results_file.exists() and args.html.samefile(results_file)
        ):
            raise ValueError("HTML report and input results must use different files")
        if args.input is None:
            completed = subprocess.run(invocation, cwd=directory)
            if completed.returncode:
                return (completed.returncode if completed.returncode > 0
                        else 128 - completed.returncode)
        results = json.loads(results_file.read_text())
        percentiles = timing_percentiles(results)
        if args.input is None and len(results) != expected_count:
            raise ValueError(f"expected {expected_count} timing samples, received {len(results)}")
        print(f"\nTiming percentiles ({len(results)} samples, linear interpolation):")
        for name, value in percentiles.items():
            print(f"{name}: {value:.3f} ms")
        command = None if args.input is not None else shlex.join(invocation)
        args.html.write_text(render_report(results, percentiles, results_file, command),
                             encoding="utf-8")
        print(f"HTML report: {args.html.resolve()}")
    except (OSError, ValueError) as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        return 130
    return 0


if __name__ == "__main__":
    sys.exit(main())
