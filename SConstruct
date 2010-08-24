import sys

#build rules for libs in contrib, returns env with 
#include and lib paths set as well as libs to link
env = SConscript('contrib/Sconscript')

if sys.platform == 'darwin':
	env.PrependUnique(CCFLAGS='-m32')
	env.PrependUnique(LINKFLAGS='-m32')

#build movidcore static library
env.Library('libmovid', [
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
	'src/modules/moYCrCbThresholdModule.cpp' ],
)

#add libmovid to libs being linked, for movid build
env.Append(LIBPATH = '.')
env.Append(LIBS = 'libmovid')

#build movid
env.Program('movid', ['src/movid.cpp', 'contrib/cJSON/cJSON.c'], CCFLAGS='-m32')

env.Dump()
