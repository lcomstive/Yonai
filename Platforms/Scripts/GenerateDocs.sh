#!/bin/bash

# Set directory to /Docs/
cd ../../Docs

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

echo "Generating documents for v$VERSION"

### Generate documentation
# Pipe output with project version to doxygen command
( cat Doxyfile ; echo ; echo "PROJECT_NUMBER=$VERSION" ) | doxygen -
