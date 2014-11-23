#!/bin/bash

PROHIBITEDEMAILHASHES="f2151a35773b112c3cdec537203dac46 e9ff8469316bf427550ef651afbb8c3f 2747686ce14f2f4629913ac26ef3de82 ba697d57c56ae5ef5738f6387397dd4c b5fe2acf0b672a6329716b824cae2f88 7f3f96c777e686bdc0bb960ae7969802"
GITUSEREMAIL=$(git config user.email)

which md5sum &> /dev/null 
if [ $? -eq 0 ]; then
	GITUSEREMAILHASH=$(echo -n ${GITUSEREMAIL} | md5sum | awk '{ print $1 }')
else
	echo "Please install md5sum and make sure it is in your PATH in order for author e-mail address verification to work. Aborting commit."
  exit 1
fi

if echo "${PROHIBITEDEMAILHASHES}" | grep -q "${GITUSEREMAILHASH}"; then
	echo "${GITUSEREMAIL} not allowed to commit. Aborting."
	exit 1
fi


# Get all changed cpp and hpp files for this commit
CHANGEDFILES=`git diff --cached --name-only | egrep '\.cpp$|\.hpp$'`
if [ -z "${CHANGEDFILES}" ]; then
  echo "No files found for hook-astyling"
else
  echo "Hook-astyling files..."
  # Only perform commit if astyle completes successful
  for i in ${CHANGEDFILES} ; do
    [ -e $i ] && astyle --options=config/astyle.rc $i || { echo $i deleted && continue ; }
    git add $i
  done
fi
