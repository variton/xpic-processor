#!/usr/bin/env bash

# Run a command sequentially, stopping if any invocation fails.
usage() {
    printf 'Usage: %s -xtimes=<positive integer> <path to binary> [args ...]\n' "${0##*/}"
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
for (( run = 0; run < times; run++ )); do
    "$@" || exit "$?"
done
