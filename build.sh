#!/usr/bin/env bash
# Build the BadCard firmware with arduino-cli.
# Requires: arduino-cli, esp32 core (arduino-cli core install esp32:esp32),
# M5Cardputer >= 1.1.1 (brings M5Unified/M5GFX) and NimBLE-Arduino 2.x:
#   arduino-cli lib install "M5Cardputer@1.1.1" NimBLE-Arduino@2.5.1
# Artifacts are copied to firmware/.
set -euo pipefail

REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
# CDCOnBoot stays disabled: with the boot CDC interface TinyUSB starts before
# setup(), which locks the device descriptor to the composite MISC class that
# BIOS/GRUB USB keyboard drivers refuse to bind.
FQBN="${FQBN:-esp32:esp32:esp32s3:FlashSize=8M,PartitionScheme=huge_app,USBMode=default,CDCOnBoot=default}"
BUILD_PATH="${BUILD_PATH:-$(mktemp -d)}"
OUT_DIR="${OUT_DIR:-$REPO_DIR/firmware}"

# arduino-cli requires the sketch folder name to match the .ino file name,
# so stage the repo into a "BadCard" directory.
SKETCH_DIR="$BUILD_PATH/BadCard"
mkdir -p "$SKETCH_DIR"
rsync -a --exclude .git --exclude firmware "$REPO_DIR/" "$SKETCH_DIR/"

arduino-cli compile --fqbn "$FQBN" --build-path "$BUILD_PATH/build" "$SKETCH_DIR"

mkdir -p "$OUT_DIR"
for f in BadCard.ino.bin BadCard.ino.merged.bin BadCard.ino.bootloader.bin BadCard.ino.partitions.bin; do
  cp "$BUILD_PATH/build/$f" "$OUT_DIR/"
done
# Release-friendly name for the app-only image.
cp "$OUT_DIR/BadCard.ino.bin" "$OUT_DIR/BadCard.bin"

echo
echo "Firmware written to $OUT_DIR"
echo "Flash the full image with:"
echo "  esptool.py --chip esp32s3 --port <PORT> write_flash 0x0 $OUT_DIR/BadCard.ino.merged.bin"
