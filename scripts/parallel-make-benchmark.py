#! /usr/bin/env python

import time
import os

for j in range(1, 10):
	start = time.time()
	os.system("make clean -C {0} > /dev/null; make -C {0} -j {1} > /dev/null 2> /dev/null".format("..", j))
	end = time.time()
	print "j={0} took {1} seconds".format(j, round(end - start))