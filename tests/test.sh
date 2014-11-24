#!/bin/sh

TEST_PASSED=true

function runTest {
	succeeded=0
	failed=0
	segfaults=0

	for i in $1/* ; do
		#echo $i
		../mj++ $2 $i &> /dev/null
		ret=$?
		#echo $ret
		if [ $ret -eq 0 ] ; then
			succeeded=$((succeeded + 1))
		elif [ $ret -eq 1 ] ; then
			failed=$((failed + 1))
		else
			segfaults=$((segfaults + 1))
		fi
	done

	if [ $TEST_PASSED = true ]; then
		if [[ $1 =~ "incorrect" ]] && [[ succeeded -eq 0 ]]; then
			TEST_PASSED=true
		elif [[ failed -eq 0 ]]; then
			TEST_PASSED=true
		fi
	fi

	echo "$1:" $((succeeded + failed+segfaults)) "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed with\033[1;31m" $segfaults "\033[0msegfaults."
}

function testPrettyPrinter {
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
