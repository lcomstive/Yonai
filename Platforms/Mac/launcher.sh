#!/bin/sh

AQUA_BUNDLE="`echo "$0" | sed -e 's/\/Contents\/MacOS\/Aqua\ Engine//'`"
AQUA_RESOURCES="$AQUA_BUNDLE/Contents/Resources"

echo "Running $0"
echo "Bundle Location: $AQUA_BUNDLE"

# Update dynamic library path to find dylibs in Resources
export "DYLD_LIBRARY_PATH=$AQUA_RESOURCES/lib"

# Update path so child processes can be run more easily
export "PATH=$AQUA_RESOURCES/bin:$PATH"

# Launch engine
exec "$AQUA_RESOURCES/bin/AquaEditor"