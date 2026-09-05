#!/bin/sh
version_file=mruby/build/host/mrbgems/mruby-bin-mrbmacs-gtk/version.txt

if [ ! -s "$version_file" ]; then
  echo "Version file was not generated: $version_file" >&2
  exit 1
fi

version=$(sed -n '1p' "$version_file")

mkdir -p Mrbmacs.app/Contents/MacOS
mkdir -p Mrbmacs.app/Contents/Resources
cp misc/Info.plist Mrbmacs.app/Contents/
/usr/libexec/PlistBuddy \
  -c "Add :CFBundleShortVersionString string $version" \
  Mrbmacs.app/Contents/Info.plist
cp mruby/bin/mrbmacs-gtk Mrbmacs.app/Contents/MacOS/
