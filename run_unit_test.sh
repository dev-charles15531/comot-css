#!/bin/bash
set -e

# Config
BUILD_DIR=build
TEST_TARGET=css_tokenizer_unit_test

echo "🔧 Configuring with tests enabled..."
CC=clang cmake -B "$BUILD_DIR" -DBUILD_TESTS=ON

echo "🔨 Building test target..."
cmake --build "$BUILD_DIR"

echo "🚀 Running unit tests: $TEST_TARGET"
"$BUILD_DIR/tests/unit/$TEST_TARGET"

echo "✅ Unit test complete."
