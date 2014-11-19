#!/bin/sh

# Get all changed cpp and hpp files for this commit
CHANGEDFILES=`git diff --cached --name-only | egrep '\.cpp$|\.hpp$'`
if [ -z "${CHANGEDFILES}" ]; then
  echo "No files found for hook-astyling"
else
  echo "Hook-astyling files..."
  echo "$CHANGEDFILES" 
  # Only perform commit if astyle completes successful
  set +e
  astyle --options=config/astyle.rc ${CHANGEDFILES}
  set -e
  git add $CHANGEDFILES
fi
