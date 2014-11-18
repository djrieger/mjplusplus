#!/bin/sh

succeeded=0
failed=0

for i in $1/* ; do
	echo $i
	../mj++ --print-ast $i > /dev/null
	ret=$?
	echo $ret
	if [ $ret -eq 0 ] ; then
		succeeded=$((succeeded + 1))
	else
		failed=$((failed + 1))
	fi
done
echo "ran" $((succeeded + failed)) "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed"
