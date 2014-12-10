#!/bin/sh

if [ \! -x ../mj++ ] ; then
	echo "Where is your compiler?"
	exit
fi

FAST=false
FASTOPTION=""
if [ "$1" = "--fast" ] ; then
	echo "Ignoring files named big.mj and suppressing error printing..."
	FAST=true
	FASTOPTION="--fast"
fi

TEST_PASSED=true

runTest() {
	succeeded=0
	failed=0
	segfaults=0

	for i in $1/* ; do
		# skip big tests if option is given:
		if [ $FAST = true -a -z "${i##*big.mj}" ]; then
			continue
		fi

		# run test
		LD_LIBRARY_PATH=../libfirm/build/debug ../mj++ $2 $FASTOPTION $i > /dev/null 2> /dev/null
		ret=$?
		if [ $ret -eq 0 ] ; then
			succeeded=$((succeeded + 1))
			if [ -z "${1##*_incorrect}" ]; then
				echo "\033[1;31mTest $i failed\033[0m"
			fi
		elif [ $ret -eq 1 ] ; then
			failed=$((failed + 1))
			if [ -z "${1##*_correct}" ]; then
				echo "\033[1;31mTest $i failed\033[0m"
			fi
		else
			segfaults=$((segfaults + 1))
			echo "\033[1;31mTest $i segfaulted\033[0m"
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

runTestDiff() {
	succeeded=0
	failed=0
	segfaults=0

	for i in $1/*.mj ; do
		LD_LIBRARY_PATH=../libfirm/build/debug ../mj++ $2 $i | diff $i.output - > /dev/null
		ret=$?
		if [ $ret -eq 0 ] ; then
			succeeded=$((succeeded + 1))
		elif [ $ret -eq 1 ] ; then
			failed=$((failed + 1))
			echo "\033[1;31mTest $i failed\033[0m"
		else
			segfaults=$((segfaults + 1))
			echo "\033[1;31mTest $i segfaulted\033[0m"
		fi
	done

	if [ $segfaults -gt 0 ]; then
		TEST_PASSED=false
	elif [ $failed -gt 0 ]; then
		TEST_PASSED=false
	fi

	echo "$1:" $((succeeded + failed + segfaults)) "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed with\033[1;31m" $segfaults "\033[0msegfaults."
}

runCompileTestDiff() {
	succeeded=0
	compfailed=0
	segfaults=0
	failed=0
	asmerrors=0

	for i in run/*.mj ; do
		#extract substring
		prefix=$(expr $i : '\(.*\)\.mj')
		LD_LIBRARY_PATH=../libfirm/build/debug ../mj++ -cfo $prefix.S $i > /dev/null
		ret=$?
		if [ $ret -eq 0 ] ; then
			#compile
			gcc -m64 $prefix.S -o $prefix.out
			ret=$?
			if [ $ret -eq 0 ] ; then
				#check diff
				./$prefix.out | diff $prefix.check - > /dev/null
				ret=$?
				if [ $ret -eq 0 ] ; then
					succeeded=$((succeeded + 1))
				else
					failed=$((failed + 1))
					echo "\033[1;31mTest $i output differs\033[0m"
					TEST_PASSED=false
				fi
			else
				asmerrors=$((asmerrors + 1))
				echo "\033[1;31mTest $i assembler error\033[0m"
				TEST_PASSED=false
			fi
			rm $prefix.S $prefix.out
		elif [ $ret -eq 1 ] ; then
			compfailed=$((compfailed + 1))
			echo "\033[1;31mTest $i compilation failed\033[0m"
			TEST_PASSED=false
		else
			segfaults=$((segfaults + 1))
			echo "\033[1;31mTest $i segfaulted\033[0m"
			TEST_PASSED=false
		fi
	done
	rm -f *.vcg

	echo "$1:" $((succeeded + failed + segfaults + asmerrors + compfailed)) "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed with\033[1;31m" $segfaults "\033[0msegfaults,\033[1;31m" $compfailed "\033[0mcompilation errors and\033[1;31m" $asmerrors "\033[0massembler errors."
}

runTestDiff ast_printer "--print-ast" #&
runTest parser_correct " " #&
runTest parser_incorrect " " #&
runTest semantic_correct "--check" #&
runTest semantic_incorrect "--check" #&
runCompileTestDiff run ""
#wait

if [ $TEST_PASSED = true ]; then
	echo "\n\033[1;32mALL TESTS PASSED\033[0m"
	exit 0
else
	echo "\n\033[1;31mTESTS FAILED\033[0m"
	exit 1
fi
