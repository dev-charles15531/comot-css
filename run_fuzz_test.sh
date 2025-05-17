#!/bin/bash
set -e

# Config
BUILD_DIR=build
CORPUS_DIR=tests/fuzz/corpus
CRASH_DIR=tests/fuzz/crashes
FUZZ_TARGET=css_tokenizer_fuzz
RUNS=500

# Ensure corpus directory exists
mkdir -p "$CORPUS_DIR"
mkdir -p "$CRASH_DIR"

# echo "🔧 Configuring with tests enabled..."
# CC=clang cmake -B "$BUILD_DIR" -DBUILD_TESTS=ON

echo "🔨 Building fuzz target..."
cmake --build "$BUILD_DIR"

echo "🚀 Running fuzz test: $FUZZ_TARGET"
"$BUILD_DIR/tests/fuzz/$FUZZ_TARGET" "$CORPUS_DIR" \
  -runs="$RUNS" \
  -artifact_prefix="$CRASH_DIR/" \
  -max_len=1024 \
  -rss_limit_mb=50 \
  -max_total_time=30

echo "✅ Fuzzing complete."
