#!/bin/sh
# david david
PROHIBITEDEMAILHASHES="f2151a35773b112c3cdec537203dac46 e9ff8469316bf427550ef651afbb8c3f"
GITUSEREMAIL=$(git config user.email)
GITUSEREMAILHASH=$(md5 -qs ${GITUSEREMAIL})
if [[ $PROHIBITEDEMAILHASHES =~ "${GITUSEREMAILHASH}" ]]; then
	echo "${GITUSEREMAIL} not allowed to commit. Aborting."
	exit 1
fi 

# Get all changed cpp and hpp files for this commit
CHANGEDFILES=`git diff --cached --name-only | egrep '\.cpp$|\.hpp$'`
if [ -z "${CHANGEDFILES}" ]; then
  echo "No files found for hook-astyling"
else
  echo "Hook-astyling files..."
  echo "$CHANGEDFILES" 
  # Only perform commit if astyle completes successful
  astyle --options=config/astyle.rc ${CHANGEDFILES}
  git add $CHANGEDFILES
fi
