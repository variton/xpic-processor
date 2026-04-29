#!/usr/bin/env bash
set -Eeuo pipefail

BUILD_DIR="${BUILD_DIR:-build/debug}"
LAB_DIR="${LAB:-$(pwd)}"
REPORT_DIR="${REPORT_DIR:-valgrind-reports}"
REPORTS_DIR="reports"
DOCS_BUILD_DIR="docs/build"
DOCS_DIR="$LAB_DIR/docs"
DOCS_LOG="docs.log"

log() {
  printf '\n[%s] %s\n' "$(date '+%H:%M:%S')" "$*"
}

die() {
  printf '\nError: %s\n' "$*" >&2
  exit 1
}

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "Required command not found: $1"
}

cleanup() {
  rm -rf "$REPORT_DIR"
}
trap cleanup EXIT

require_cmd ctest
require_cmd cmake
require_cmd python3
require_cmd valgrind
require_cmd clang-format
require_cmd nproc

[[ -d "$LAB_DIR" ]] || die "LAB directory not found: $LAB_DIR"

JOBS="$(nproc)"

log "Removing debug build directory"
rm -rf "$BUILD_DIR"

log "Configuring Debug build"
cmake -S . \
  -B "$BUILD_DIR" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

log "Building Debug"
cmake --build "$BUILD_DIR" -j "$JOBS"

log "Running unit tests"
ctest --test-dir "$BUILD_DIR" --output-on-failure -j "$JOBS"

# --------------------------------------------------
# Non-regression test: deinterlacer under Valgrind
# --------------------------------------------------
log "Running non-regression test: deinterlacer"

mkdir -p "$REPORTS_DIR"
mkdir -p output

DEINTERLACER_BIN="$LAB_DIR/bin/deinterlacer"
INPUT_IMG="$LAB_DIR/rc/interlaced.jpg"
OUTPUT_IMG="$LAB_DIR/output/deinterlaced.jpg"
QUALITY_REPORT="$REPORTS_DIR/deinterlacer-quality-report.log"

[[ -x "$DEINTERLACER_BIN" ]] || die "Missing binary: $DEINTERLACER_BIN"
[[ -f "$INPUT_IMG" ]] || die "Missing input image: $INPUT_IMG"

valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --error-exitcode=1 \
  "$DEINTERLACER_BIN" \
  -i "$INPUT_IMG" \
  -o "$OUTPUT_IMG" \
  > "$QUALITY_REPORT" 2>&1

[[ -f "$OUTPUT_IMG" ]] || die "Deinterlacer did not produce output file"

log "Deinterlacer quality report saved to $QUALITY_REPORT"    
# --------------------------------------------------

log "Preparing Valgrind report directory"
mkdir -p "$REPORT_DIR"

log "Running Valgrind on test binaries"
shopt -s nullglob
counter=1
found_tests=0

for file in "$LAB_DIR"/bin/test_*; do
  if [[ -f "$file" && -x "$file" ]]; then
    found_tests=1
    name="$(basename "$file")"
    report_file="$REPORT_DIR/report_${counter}_${name}.xml"

    log "Running $name"
    valgrind \
      --leak-check=full \
      --show-leak-kinds=all \
      --track-origins=yes \
      --xml=yes \
      --xml-file="$report_file" \
      --log-file="$REPORT_DIR/${name}.log" \
      --error-exitcode=1 \
      "$file"

    printf 'Report saved to %s\n' "$report_file"
    ((counter++))
  fi
done


if [[ "$found_tests" -eq 0 ]]; then
  log "No test binaries found in $LAB_DIR/bin"
fi

log "Generating HTML Valgrind report"
python3 tools/valgrind-report-maker.py "$REPORT_DIR" -o docs/valgrind-report.html

cp "$DOCS_DIR"/valgrind-report.html "$REPORTS_DIR"/valgrind-report.html

log "Running clang-format"
shopt -s globstar
clang-format -i src/**/*.cpp include/**/*.h test/*.cpp

log "Building coverage target"
cmake --build "$BUILD_DIR" --target coverage
cp -r $BUILD_DIR/coverage_html $REPORTS_DIR

log "Building documentation"
cmake -S . -B "$DOCS_BUILD_DIR" -G Ninja
cmake --build "$DOCS_BUILD_DIR" --target doxygen > "$DOCS_LOG" 2>&1
cp -r "$DOCS_BUILD_DIR"/html "$REPORTS_DIR"
echo "$DOCS_LOG"

log "Done"
