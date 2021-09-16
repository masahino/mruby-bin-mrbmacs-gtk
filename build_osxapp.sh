#!/bin/sh
mkdir -p Mrbmacs.app/Contents/MacOS
mkdir -p Mrbmacs.app/Contents/Resources
cp misc/Info.plist Mrbmacs.app/Contents/
cp mruby/bin/mrbmacs-gtk Mrbmacs.app/Contents/MacOS/

