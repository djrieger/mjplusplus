#!/bin/sh
# Unresolved issues:
# 1) When only changing the formatting of a file and comitting, this will
# result in an empty commit since astyle changes the file to the version
# of the previous commit.
# 2) Staging hunks instead of whole files is a potentail pitfall when
# doing hooked auto-formatting with astyle

# Get all changed cpp and hpp files for this commit
CHANGEDFILES=`git diff --cached --name-only | egrep '\.cpp$|\.hpp$'`
if [ -z "${CHANGEDFILES}" ]; then
  echo "No files found for hook-astyling"
else
  echo "Hook-astyling files..."
  echo "$CHANGEDFILES" 
  # Only perform commit if astyle completes successful
  astyle --options=astyle.rc ${CHANGEDFILES} && git add $CHANGEDFILES
fi