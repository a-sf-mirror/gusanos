import re
import os
import time

def getLibName(lib):
	return os.path.join('#lib', platform, lib)
	
def getBinName(bin):
	if platform == 'mingw-cross':
		bin += '.exe'
	return os.path.join('#bin', platform, bin)

platform = ARGUMENTS.get('platform', 'posix')

sconscript = ['./GUI/detail/SConscript',
            './Console/SConscript',
            './loadpng/SConscript',
            './Goop/SConscript',
            './liero2gus/SConscript']

if platform == 'posix':
	env = Environment(
		CPPPATH = ['.', '#loadpng', '/usr/local/include/zoidcom', '/usr/local/include/boost-1_32', '#Console', '#GUI'],
		LIBPATH = ['/usr/local/lib', '/usr/X11R6/lib', os.path.join('#lib/', platform)],
		CPPFLAGS = '-O3',
		#CPPFLAGS = '-O0 -g',
		CXX='g++-3.4',
		)
elif platform == 'mingw-cross':
	mingwPath = ARGUMENTS.get('mingw-path', '/usr/local/mingw/')
	env = Environment(
		CPPPATH = ['.', '#loadpng', os.path.join(mingwPath, 'include', 'boost-1_32'), '#Console', '#GUI'],
		LIBPATH = [os.path.join('#lib', platform)],
		CPPFLAGS = '-O3 -DWINDOWS',
		CXX = os.path.join(mingwPath, 'bin', 'g++'),
		CC = os.path.join(mingwPath, 'bin', 'gcc'),
		LD = os.path.join(mingwPath, 'bin', 'ld'),
		RANLIB = os.path.join(mingwPath, 'bin', 'ranlib'),
		AR = os.path.join(mingwPath, 'bin', 'ar'),
		)
elif platform == 'basara':
	env = Environment(
		CPPPATH = ['.', '/usr/local/include/zoidcom', '#Console', '#GUI'],
		LIBPATH = ['/usr/local/lib', os.path.join('#lib/', platform)],
		CPPFLAGS = '-O3',
		#CPPFLAGS = '-O0 -g',
		CXX='g++-3.4',
		)
# add more platforms here
	
#Builds all cpp and c files in this directory and returns a list of nodes
def getObjects(env):
	sourcePattern = re.compile('\.(cpp|c)$')
	
	sources = []
	
	for i in os.listdir('.'):
		if sourcePattern.search(i):
			sources.append(i)
	
	buildDir = os.path.join('.build', platform)
	env.BuildDir(buildDir, '.', duplicate = 0)
	objects = []
	for s in sources:
		objects += env.Object(os.path.join(buildDir, s))
			
	return objects


SConscript(sconscript, exports=['env', 'getObjects', 'getLibName', 'getBinName', 'platform'])
            
if platform == 'mingw-cross':
	env.Append(BUILDERS = {'Strip' : Builder(action = os.path.join(mingwPath, 'bin', 'strip') + ' $SOURCE')})
	gus = getBinName('gusanos')
	gus = env.Install('/usr/local/htdocs/stuff/gusanos-alpha', gus)
	env.Strip(gus)
	env.Alias('upload', '/usr/local/htdocs/stuff/gusanos-alpha')