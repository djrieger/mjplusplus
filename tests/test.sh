#!/bin/sh

succeeded=0
failed=0
segfaults=0

for i in $1/* ; do
	echo $i
	../mj++ --check $i #> /dev/null
	ret=$?
	echo $ret
	if [ $ret -eq 0 ] ; then
		succeeded=$((succeeded + 1))
	elif [ $ret -eq 1 ] ; then
		failed=$((failed + 1))
	else
		segfaults=$((segfaults + 1))
	fi
done
echo "ran" $((succeeded + failed+segfaults)) "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed with\033[1;31m" $segfaults "\033[0msegfaults."
