#!/bin/sh

if [ \! -x ../mj++ ] ; then
	echo "Where is your compiler?"
	exit
fi

succeeded=0
compfailed=0
segfaults=0
failed=0
asmerrors=0

for i in run/*.mj ; do
	echo $i
	#extract substring
	prefix=$(expr $i : '\(.*\)\.mj')
	../mj++ --check --out $prefix.S $i #> /dev/null
	ret=$?
	echo $ret
	if [ $ret -eq 0 ] ; then
		#compile
		gcc $prefix.S -o $prefix.out
		ret=$?
		echo $ret
		if [ $ret -eq 0 ] ; then
			#check diff
			$prefix.out | diff $prefix.check -
			ret=$?
			echo $ret
			if [ $ret -eq 0 ] ; then
				succeeded=$((succeeded + 1))
			else
				failed=$((failed + 1))
			fi
		else
			asmerrors=$((asmerrors + 1))
		fi
	elif [ $ret -eq 1 ] ; then
		compfailed=$((compfailed + 1))
	else
		segfaults=$((segfaults + 1))
	fi
done

total=$((succeeded + failed+segfaults+asmerrors+compfailed))
echo "ran" $total "tests:\033[1;32m" $succeeded "\033[0msucceeded,\033[1;31m" $failed "\033[0mfailed with\033[1;31m" $segfaults "\033[0msegfaults, \033[1;31m" $compfailed "\033[0mcompilation errors and\033[1;31m" $asmerrors "\033[0massembler errors."
