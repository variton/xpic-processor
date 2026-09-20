#!/usr/bin/env bash

# Run a command sequentially, stopping if any invocation fails.
usage() {
    printf 'Usage: %s -xtimes=<positive integer> [--log=<file>] [--json=<file>] [--] <path to binary> [args ...]\n' "${0##*/}"
    printf '  --log=<file>  Append binary stdout and stderr to a file and display them.\n'
    printf '  --json=<file> Write timing/SSIM results from this invocation as JSON (requires Python 3).\n'
}

if [[ ${1-} == --help || ${1-} == -h ]]; then
    usage
    exit 0
fi

if (( $# < 2 )) || [[ $1 != -xtimes=* ]]; then
    usage >&2
    exit 2
fi

times=${1#-xtimes=}
if [[ ! $times =~ ^[1-9][0-9]*$ ]]; then
    printf 'Error: -xtimes must be a positive integer without leading zeros.\n' >&2
    exit 2
fi

# Bound the count before using Bash integer arithmetic.
if (( ${#times} > 10 )) || { (( ${#times} == 10 )) && [[ $times > 2147483647 ]]; }; then
    printf 'Error: -xtimes must be at most 2147483647.\n' >&2
    exit 2
fi

shift
log_file=
json_file=
while [[ ${1-} == --log=* || ${1-} == --json=* ]]; do
    option=${1%%=*}
    value=${1#*=}
    if [[ -z $value ]]; then
        printf 'Error: %s requires a file path.\n' "$option" >&2
        exit 2
    fi
    case $option in
        --log) log_file=$value ;;
        --json) json_file=$value ;;
    esac
    shift
done
if [[ ${1-} == -- ]]; then
    shift
fi
if (( $# == 0 )); then
    usage >&2
    exit 2
fi

tee_files=()
if [[ -n $log_file || -n $json_file ]]; then
    command -v tee >/dev/null || exit 1
fi
if [[ -n $json_file ]]; then
    command -v python3 >/dev/null || exit 1
    script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd) || exit 1
    if [[ -n $log_file ]] && [[ $log_file == "$json_file" || $log_file -ef $json_file ]]; then
        printf 'Error: --log and --json must use different files.\n' >&2
        exit 2
    fi
    json_capture=$(mktemp) || exit 1
    finish_json() {
        status=$?
        python3 "$script_dir/results_to_json.py" "$json_capture" "$json_file"
        export_status=$?
        rm -f -- "$json_capture"
        (( status == 0 )) || exit "$status"
        exit "$export_status"
    }
    trap finish_json EXIT
    tee_files+=("$json_capture")
fi
if [[ -n $log_file ]]; then
    # Fail before running the binary if its output cannot be logged.
    : >> "$log_file" || exit 1
    tee_files+=("$log_file")
fi

for (( run = 0; run < times; run++ )); do
    if (( ${#tee_files[@]} )); then
        "$@" 2>&1 | tee -a -- "${tee_files[@]}"
        statuses=("${PIPESTATUS[@]}")
        (( statuses[0] == 0 )) || exit "${statuses[0]}"
        (( statuses[1] == 0 )) || exit "${statuses[1]}"
    else
        "$@" || exit "$?"
    fi
done
