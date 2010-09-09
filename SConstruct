#################################################################
# Command line options to controll build
#################################################################
AddOption( '--clean-contrib',
  help='force rebuilding of contrib libraries',
  dest='clean-contrib', action="store_true"
)


#################################################################
# Source files list for each target we are building
#################################################################

#source files for the daemon ####################################
daemon_src = [
  'src/movid.cpp',
  'contrib/cJSON/cJSON.c'
]

#source files for libmovid, core+modules ########################
libmovid_src = [
  'src/moDaemon.cpp',
  'src/moDataGenericContainer.cpp',
  'src/moDataStream.cpp',
  'src/moFactory.cpp',
  'src/moLog.cpp',
  'src/moModule.cpp',
  'src/moOSC.cpp',
  'src/moPipeline.cpp',
  'src/moProperty.cpp',
  'src/moThread.cpp',
  'src/moUtils.cpp',
  'src/modules/moAmplifyModule.cpp',
  'src/modules/moBackgroundSubtractModule.cpp',
  'src/modules/moBlobFinderModule.cpp',
  'src/modules/moCalibrationModule.cpp',
  'src/modules/moCameraModule.cpp',
  'src/modules/moCannyModule.cpp',
  'src/modules/moCombineModule.cpp',
  'src/modules/moDilateModule.cpp',
  'src/modules/moDistanceTransformModule.cpp',
  'src/modules/moDumpModule.cpp',
  'src/modules/moErodeModule.cpp',
  'src/modules/moFiducialTrackerModule.cpp',
  'src/modules/moFingerTipFinderModule.cpp',
  'src/modules/moGreedyBlobTrackerModule.cpp',
  'src/modules/moGrayScaleModule.cpp',
  'src/modules/moHighpassModule.cpp',
  'src/modules/moHsvModule.cpp',
  'src/modules/moImageDisplayModule.cpp',
  'src/modules/moImageFilterModule.cpp',
  'src/modules/moImageModule.cpp',
  'src/modules/moInvertModule.cpp',
  'src/modules/moJustifyModule.cpp',
  'src/modules/moMaskModule.cpp',
  'src/modules/moMirrorImageModule.cpp',
  'src/modules/moPeakFinderModule.cpp',
  'src/modules/moRoiModule.cpp',
  'src/modules/moSmoothModule.cpp',
  'src/modules/moThresholdModule.cpp',
  'src/modules/moTuioModule.cpp',
  'src/modules/moTuio2Module.cpp',
  'src/modules/moVideoModule.cpp',
  'src/modules/moYCrCbThresholdModule.cpp'
]


#################################################################
# Build contirb and configure env for linking against deps
#################################################################
env = SConscript('contrib/SConscript')



#################################################################
# Platform sepcific settings for build env and OpenCV flags
#################################################################
import sys, os

# WIN32 #########################################################
if sys.platform == 'win32':
  #on widnows daemon also needs XgetOpt source file
  daemon_src.append('contrib/XgetOpt/XgetOpt.cxx')

  #OpenCV, must config manually on windowze :/
  opencv_dir = ARGUMENTS.get('OPENCV_DIR', 'C:\OpenCV2.1')
  env.Append(
    LIBS = ['cv210', 'cxcore210', 'highgui210', 'cvaux210'],
    LIBPATH = [opencv_dir+'\lib'],
    CPPPATH = [opencv_dir+'\include\opencv'])

  #gotta set up msvc compiler and linker for list of options see:
  #see http://msdn.microsoft.com/en-us/library/fwkeyyhe(v=VS.71).aspx
  #and http://msdn.microsoft.com/en-us/library/y0zzbyt4(VS.80).aspx
  env.Append(
    CPPDEFINES = ['WIN32'],
    CCFLAGS = ['/O2', '/Oi', '/GL', '/EHsc', '/MD'], #mainly optimization
    LIBS = ['ws2_32.lib', 'user32.lib'], #ws_32.lib is needed buy libevent
    LINKFLAGS = ['/LTCG', '/OPT:REF', '/OPT:ICF']) #mainly optimization



# UNIX #######################################################
else:
  #add OpenCV flags and libs
  env.ParseConfig('pkg-config --cflags --libs opencv') #gotta love unix :P
  
  #set the compiler if set in ENV, used e.g. to force 32bit by setting to g++ -m32
  if os.environ.get('CC'): env.Replace(CC=os.environ['CC'])
  if os.environ.get('CXX'): env.Replace(CXX=os.environ['CXX'])




#################################################################
# Build Rules for libmovid and movid daemon
#################################################################
libmovid = env.Library('libmovid', libmovid_src )
env.Program('movid', daemon_src + [libmovid])
