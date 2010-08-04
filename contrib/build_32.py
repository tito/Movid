# Just a little helper script to compile the dependencies in 32 bit.
# Don't use this script if you don't know what you're doing.
# I need it to get cams like the PS3 on OSX working.
# XXX Remove this script and do it properly in the Makefile
from subprocess import call
from os import chdir, curdir
from os.path import abspath, join


CONTRIB_DIR = abspath(curdir)

# Libfidtrack
folder = chdir('libfidtrack')
# save their CXXFLAGS in order not to override them completely
CXXFLAGS = "-Wall -O2 -finline-functions -ffast-math -funroll-all-loops -fomit-frame-pointer"
call(["make", "CXXFLAGS=%s -m32" % (CXXFLAGS, )])

# Ptypes
folder = chdir(join(CONTRIB_DIR, 'ptypes-2.1.1/src'))
call(["make", "-f", "Makefile.Darwin", "CXX=g++ -m32"])
#call(["make", "-f", "Makefile.Darwin", "OS_CXXOPTS_ST=-m32"])

# libevent
folder = chdir(join(CONTRIB_DIR, 'libevent-1.4.13-stable'))
call(["sh", "configure", "CFLAGS=-m32"])
call(["make"])

# WOsclib
folder = chdir(join(CONTRIB_DIR, 'WOscLib-00.09'))
call(["sh", "configure", "CFLAGS=-m32"])
call(["make", "CXX=g++ -m32"])




