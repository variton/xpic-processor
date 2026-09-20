#!/usr/bin/env python3
"""Convert benchmark timing/SSIM output to a JSON array."""

import argparse
import json
import re
from pathlib import Path


def parse_results(text):
    number = r"[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?"
    timing = re.compile(rf"time (?:elasped|elapsed):\s*({number})\s*ms", re.I)
    ssim = re.compile(rf"mean ssim:\s*({number})", re.I)
    results = []
    current = {}
    for line in text.splitlines():
        match = timing.fullmatch(line.strip())
        if match:
            if current:
                results.append(current)
            current = {"time_elapsed_ms": float(match[1])}
        else:
            match = ssim.fullmatch(line.strip())
            if match:
                current["mean_ssim"] = float(match[1])
                results.append(current)
                current = {}
    if current:
        results.append(current)
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("json_file", type=Path)
    args = parser.parse_args()
    if args.log.resolve() == args.json_file.resolve() or (
        args.json_file.exists() and args.log.samefile(args.json_file)
    ):
        parser.error("input log and JSON output must be different files")
    try:
        results = parse_results(args.log.read_text(errors="replace"))
        output = json.dumps(results, indent=2, allow_nan=False) + "\n"
        args.json_file.write_text(output)
    except (OSError, ValueError) as error:
        parser.exit(1, f"Error exporting results: {error}\n")


if __name__ == "__main__":
    main()
