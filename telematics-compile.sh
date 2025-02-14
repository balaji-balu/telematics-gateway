#!/bin/bash

# Define package name
PKG_NAME="telematics-gateway"

# Change to OpenWRT build directory
OPENWRT_DIR="$HOME/openwrt-project/openwrt-23-05-2/"
BUILD_DIR="$OPENWRT_DIR/build_dir"

cd "$OPENWRT_DIR"
echo $pwd
echo "🛠 Building package: $PKG_NAME"

# Clean previous builds
echo "🧹 Cleaning previous build..."
make package/$PKG_NAME/clean

# Run the prepare step
echo "📦 Running Build/Prepare..."
make package/$PKG_NAME/prepare V=s

# Check if the source files were copied
PKG_BUILD_DIR=$(find "$BUILD_DIR" -type d -name "$PKG_NAME" | head -n 1)

if [ -d "$PKG_BUILD_DIR" ]; then
    echo "✅ Package prepared successfully in: $PKG_BUILD_DIR"
    ls -l "$PKG_BUILD_DIR"
else
    echo "❌ Error: Package preparation failed. Check the Makefile."
    exit 1
fi

# Run the compile step
echo "⚙️  Running Build/Compile..."
make package/$PKG_NAME/compile V=s

# Check if the package binary was created
BIN_DIR="$OPENWRT_DIR/bin/packages/"
BIN_FILE=$(find "$BIN_DIR" -type f -name "$PKG_NAME*" | head -n 1)

if [ -n "$BIN_FILE" ]; then
    echo "✅ Package compiled successfully: $BIN_FILE"
else
    echo "❌ Error: Compilation failed. Check logs for errors."
    exit 1
fi