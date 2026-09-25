#!/usr/bin/env python3
"""Run the 50-sample SSIM benchmark and report timing percentiles in milliseconds."""

import argparse
import json
import math
from pathlib import Path
import subprocess
import sys

from html_report import render_report


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
    parser.add_argument("--input", type=Path,
                        help="read an existing results JSON file instead of running the benchmark")
    parser.add_argument("--html", type=Path, default=Path("report.html"),
                        help="HTML report destination (default: report.html)")
    args = parser.parse_args()
    directory = Path(__file__).resolve().parent
    results_file = args.input if args.input is not None else directory / "res.json"
    try:
        if args.html.resolve() == results_file.resolve() or (
            args.html.exists() and results_file.exists() and args.html.samefile(results_file)
        ):
            raise ValueError("HTML report and input results must use different files")
        if args.input is None:
            completed = subprocess.run(
                [sys.executable, str(directory / "run.py"), "-xtimes=50", "--json=res.json",
                 "../bin/mt-mean-ssim", "../rc/ur.jpg", "../rc/watermarked_ur.jpg"],
                cwd=directory,
            )
            if completed.returncode:
                return (completed.returncode if completed.returncode > 0
                        else 128 - completed.returncode)
        results = json.loads(results_file.read_text())
        percentiles = timing_percentiles(results)
        if args.input is None and len(results) != 50:
            raise ValueError(f"expected 50 timing samples, received {len(results)}")
        print(f"\nTiming percentiles ({len(results)} samples, linear interpolation):")
        for name, value in percentiles.items():
            print(f"{name}: {value:.3f} ms")
        command = None if args.input is not None else (
            "./run.py -xtimes=50 --json=res.json ../bin/mt-mean-ssim "
            "../rc/ur.jpg ../rc/watermarked_ur.jpg"
        )
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
