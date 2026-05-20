#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(pwd)"
BUILD_TYPE=""
BUILD_DIR=""
RUN_ANALYSIS=false
BUILD_DOCS=false

usage() {
  cat <<EOF
Usage: $0 [debug|release|clean|clean-debug|clean-release] [-a] [-d]

Commands:
  debug           Configure and build Debug
  release         Configure and build Release
  clean           Remove all build artifacts
  clean-debug     Remove debug build artifacts
  clean-release   Remove release build artifacts

Flags:
  -a              Run static analysis
  -d              Build documentation
EOF
}

clean_target() {
  local target="$1"

  case "$target" in
    clean-debug)
      echo "Removing debug build"
      rm -rf build/debug
      rm -rf bin/*
      ;;
    clean-release)
      echo "Removing release build"
      rm -rf build/release
      rm -rf bin/*
      ;;
    clean)
      echo "Removing all builds"
      rm -rf build
      rm -rf bin/*
      ;;
    *)
      echo "Unknown clean target: $target" >&2
      exit 1
      ;;
  esac
}

parse_args() {
  local command=""

  for arg in "$@"; do
    case "$arg" in
      debug)
        command="debug"
        BUILD_TYPE="Debug"
        BUILD_DIR="build/debug"
        ;;
      release)
        command="release"
        BUILD_TYPE="Release"
        BUILD_DIR="build/release"
        ;;
      clean|clean-debug|clean-release)
        clean_target "$arg"
        exit 0
        ;;
      -a)
        RUN_ANALYSIS=true
        ;;
      -d)
        BUILD_DOCS=true
        ;;
      -h|--help)
        usage
        exit 0
        ;;
      *)
        echo "Unknown argument: $arg" >&2
        usage
        exit 1
        ;;
    esac
  done

  if [[ -z "$command" ]]; then
    echo "Missing build type" >&2
    usage
    exit 1
  fi
}

parse_args "$@"

if [[ "$BUILD_TYPE" == "Debug" && -d build/debug ]]; then
  find build/debug -name '*.gcda' -delete
  find build/debug -name '*.gcno' -delete
  echo "Debug build selected"
fi

mkdir -p "$BUILD_DIR"

echo "Setting up build config: $BUILD_TYPE"

cmake -S . \
  -B "$BUILD_DIR" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DCMAKE_CXX_COMPILER=g++-15 \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "Finished $BUILD_TYPE configuration"

JOBS="$(nproc)"

cmake --build "$BUILD_DIR" -j "$JOBS"

if [[ "$BUILD_TYPE" == "Release" ]]; then
  ctest --test-dir "$BUILD_DIR" --output-on-failure -j "$JOBS"
else
  ctest --test-dir "$BUILD_DIR" -V --output-on-failure -j "$JOBS"
fi

if $BUILD_DOCS; then
  echo "Building documentation"
  cmake -S . -B docs/build -G Ninja
  cmake --build docs/build --target doxygen > docs.log
fi

if $RUN_ANALYSIS; then
  echo "Running static analysis"
  run-clang-tidy -p "$BUILD_DIR" > clang.analysis
  scan-build --status-bugs cmake --build "$BUILD_DIR" -j "$JOBS"
fi
