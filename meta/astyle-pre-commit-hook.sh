#!/bin/sh

# Get all changed cpp and hpp files for this commit
CHANGEDFILES=`git diff --cached --name-only | egrep '\.cpp$|\.hpp$'`
if [ -z "${CHANGEDFILES}" ]; then
  echo "No files found for hook-astyling"
else
  echo "Hook-astyling files..."
  echo "$CHANGEDFILES" 
  # Only perform commit if astyle completes successful
  astyle --options=meta/astyle.rc ${CHANGEDFILES}
  git add $CHANGEDFILES
fi
