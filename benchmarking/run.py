#!/usr/bin/env python3
"""Run a benchmark sequentially and optionally export timing/SSIM results."""

import argparse
from contextlib import ExitStack
import json
import re
import subprocess
import sys
import tempfile
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


def positive_count(value):
    if not re.fullmatch(r"[1-9][0-9]*", value):
        raise argparse.ArgumentTypeError("must be a positive integer without leading zeros")
    if len(value) > 10 or int(value) > 2147483647:
        raise argparse.ArgumentTypeError("must be at most 2147483647")
    return int(value)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-xtimes", required=True, type=positive_count,
                        help="number of sequential invocations")
    parser.add_argument("--log", type=Path,
                        help="append stdout and stderr to this file and display them")
    parser.add_argument("--json", type=Path,
                        help="write timing/SSIM results from this invocation as JSON")
    parser.add_argument("command", nargs=argparse.REMAINDER,
                        help="binary followed by its arguments")
    args = parser.parse_args()
    command = args.command
    if command[:1] == ["--"]:
        command = command[1:]
    if not command:
        parser.error("a command is required")
    if args.log is not None and args.json is not None:
        if args.log.resolve() == args.json.resolve() or (
            args.log.exists() and args.json.exists() and args.log.samefile(args.json)
        ):
            parser.error("--log and --json must use different files")

    status = 0
    with ExitStack() as stack:
        capture = (stack.enter_context(tempfile.TemporaryFile())
                   if args.json is not None else None)
        try:
            log = (stack.enter_context(args.log.open("ab", buffering=0))
                   if args.log is not None else None)
            for _ in range(args.xtimes):
                if log is None and capture is None:
                    status = subprocess.call(command)
                else:
                    with subprocess.Popen(command, stdout=subprocess.PIPE,
                                          stderr=subprocess.STDOUT) as process:
                        try:
                            while chunk := process.stdout.read1(65536):
                                if capture is not None:
                                    capture.write(chunk)
                                if log is not None:
                                    log.write(chunk)
                                sys.stdout.buffer.write(chunk)
                                sys.stdout.buffer.flush()
                        except BaseException:
                            process.terminate()
                            raise
                    status = process.returncode
                if status:
                    if status < 0:
                        status = 128 - status
                    break
        except FileNotFoundError as error:
            print(f"Error: {error}", file=sys.stderr)
            status = 127 if error.filename == command[0] else 1
        except OSError as error:
            print(f"Error: {error}", file=sys.stderr)
            status = 126 if error.filename == command[0] else 1
        except KeyboardInterrupt:
            status = 130
        finally:
            if capture is not None:
                try:
                    capture.seek(0)
                    results = parse_results(capture.read().decode(errors="replace"))
                    output = json.dumps(results, indent=2, allow_nan=False) + "\n"
                    args.json.write_text(output)
                except (OSError, ValueError) as error:
                    print(f"Error exporting results: {error}", file=sys.stderr)
                    status = status or 1
    return status


if __name__ == "__main__":
    sys.exit(main())
