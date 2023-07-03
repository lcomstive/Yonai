#!/bin/bash

# Retrieve version from file
VERSION=$(cat ../Version.txt)

# Get current branch
GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)

# Check if Git branch is not main
if [ "$GIT_BRANCH" != "main" ]; then
	# Retrieve Git repository info
	GIT_HASH=$(git log --pretty=format:'%h' -n 1)
	
	# Update project version to include Git info
	VERSION="$VERSION-$GIT_HASH [$GIT_BRANCH]"
fi

if [ -f "./Dist/" ]; then
	rm -rf ./Dist/
fi

echo "Generating documents for v$VERSION"

# Create output directory
mkdir -p ./Dist/

### Generate documentation
# Pipe output with project version to doxygen command

# Public facing documentation
( cat Doxyfile ; echo ; echo "PROJECT_NUMBER=$VERSION" ) | doxygen -

# Internal (C++ Engine) documentation
( cat DoxyfileInternal ; echo ; echo "PROJECT_NUMBER=$VERSION" ) | doxygen -

# Move all items in Dist/API/html to Dist/API
mv ./Dist/API/html/ ./Dist/API/
rm -rf ./Dist/API/html

# Move all items in Dist/InternalAPI/html to Dist/InternalAPI
mv ./Dist/InternalAPI/html/ ./Dist/InternalAPI/
rm -rf ./Dist/InternalAPI/html

# Copy Home/ folder to Dist/
cp ./Home/ ./Dist/