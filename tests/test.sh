#!/bin/sh

TEST_PASSED=true

runTest() {
	succeeded=0
	failed=0
	segfaults=0

	for i in $1/* ; do
		../mj++ $2 $i > /dev/null 2> /dev/null
		ret=$?
		if [ $ret -eq 0 ] ; then
			succeeded=$((succeeded + 1))
		elif [ $ret -eq 1 ] ; then
			failed=$((failed + 1))
		else
			segfaults=$((segfaults + 1))
		fi
	done

	if [ $segfaults -gt 0 ]; then
		TEST_PASSED=false
	elif [ -z "${1##*_incorrect}" -a $succeeded -gt 0 ]; then
		TEST_PASSED=false
	elif [ -z "${1##*_correct}" -a $failed -gt 0 ]; then
		TEST_PASSED=false
	fi

	echo "$1:" $((succeeded + failed + segfaults)) "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed with\033[1;31m" $segfaults "\033[0msegfaults."
}

testPrettyPrinter() {
	../mj++ --print-ast ast_printer/HelloWord.mj > temp
	diff temp ast_printer/HelloWord-output.mj > /dev/null
	if [ $? -eq 0 ] ; then
		echo "prettyprinter test succeeded"
	else
		echo "prettyprinter test failed"
		TEST_PASSED=false
	fi
	rm temp
}

testPrettyPrinter
runTest parser_correct " "
runTest parser_incorrect " "
runTest semantic_correct "--check"
runTest semantic_incorrect "--check"

if [ $TEST_PASSED = true ]; then
	echo "\n\033[1;32mALL TESTS PASSED\033[0m"
	exit 0
else
	echo "\n\033[1;31mTESTS FAILED\033[0m"
	exit 1
fi
