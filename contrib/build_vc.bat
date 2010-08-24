::build_vc.bat
::Copyright 2010 Movid Authors
::
::This is a bat file that is used to compile visual studio projects
::through the command line.  It automatically finds build tools installed
::by express or full versions of visual c++ 2008 and 2010.




:::::::::::::::::::::::::::::::::::::::::::::::::::::
::firts arg is teh project filename
set project_path=%~dp1 
set project_ext=%~x1
set project_file=%~nx1  

::work in teh proejct fiel directory
cd %project_path%


:::::::::::::::::::::::::::::::::::::::::::::::::::::
::go over all the arguments, and constrcut agrgv to pass to vcbuild or msbuild
::we pass all args except teh first one, which is teh project file
::for teh project file, we might have to convert it to vcxproj for vs2010
set argc=0
set argv=

::get number of arguments
for %%a in (%*) do set /a argc+=1

::loop over
set i=2
set stop=%argc%
set /a stop+=1
:loop
    set /a i+=1
    shift
    set arg=%1
    if defined VS100COMNTOOLS (set argv=%arg:/=/t:%)
    else (set argv=%argv% %arg%)
    
    if not %i%==%stop% goto loop



:::::::::::::::::::::::::::::::::::::::::::::::::::::
::check which version of VCPP to use
if defined VS100COMNTOOLS goto VCPP2010
if defined VS09COMNTOOLS goto VCPP2009


:VCPP2009
:::::::::::::::::::::::::::::::::::::::::::::::::::::
::build using VC++ 2009

::setup env
%VS09COMNTOOLS%\..\..\vcvarsall.bat

::build the project
vcbuild %argv% %project_file%

goto END


:VCPP2010
:::::::::::::::::::::::::::::::::::::::::::::::::::::
::build using VC++ 2010

::setup env
%VS100COMNTOOLS%\..\..\vcvarsall.bat

::upgrade if necesarry
vcupgrade %project_file%
if %project_ext%==.vcproj set project_file=%project_file:.vcproj=.vcxproj%


:build 
msbuild %argv% %project_file%


:END

