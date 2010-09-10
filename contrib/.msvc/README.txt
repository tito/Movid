This dirctory contains a micrsoft visual c++ 2010 solution to complie movid dependencies on windows.

The solutions has 5 projects inside of it:
event
fidtrack
ptypes
wosclib
contribs

contribs is a project that doesnt really have any source code.  it is used to controll the build of all of the other ones.  contribs project is marked as depends on all other projetcs (so the other ones will be build first).  the contribs porject also deinfes a pre and post-build step which deletes contrib/lib and copies the compiled libs intoto contrib/lib (../lib)

build.bat accepts the same arguments as msbuild.  it is meant to be used to compile from command line or when starting command from a script.  it sets up the env so all vc++ tools are available and then calls msbuild to build the solution (it also deletes ..\lib as teh first thing, so that when passign /t:clean that directory will be removed as well, since no build rule will be executed)