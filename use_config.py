#!/usr/bin/env python

import sys, os

if len(sys.argv) <= 1:
    print("need an argument, config file name:")
    for f in os.listdir('configs/'):
        print("configs/%s" % f)
    sys.exit()

# import config file
#execfile(sys.argv[1])
# for python3
exec(compile(open(sys.argv[1]).read(), sys.argv[1], 'exec'))

# check arch var exists
if 'ARCH' not in locals():
	sys.exit("config file doesn't have ARCH set")

# create new symlinks
try:
	os.remove("config.py")
	os.remove("config.pyc")
	os.remove("include/arch")
	os.remove("include/mach")
except:
	pass

os.symlink(sys.argv[1], "config.py")
os.symlink("../arch/" + ARCH + "/include/", "include/arch")
if 'MACH' in locals():
	os.symlink("../arch/" + ARCH + "/mach-" + MACH + "/include/", "include/mach")
