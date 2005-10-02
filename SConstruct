import re
import os
import time

def getLibName(lib):
	return os.path.join('#lib', subfolder, lib)
	
def getBinName(bin):
	if conf == 'mingw-cross':
		bin += '.exe'
	return os.path.join('#bin', conf, bin) # Put executable here so not to confuse

conf = ARGUMENTS.get('conf', 'posix')
build = ARGUMENTS.get('build', 'release')
subfolder = os.path.join(conf, build)

sconscript = ['./GUI/detail/SConscript',
			'./Utility/detail/SConscript',
            './Console/SConscript',
            './loadpng/SConscript',
            './Goop/SConscript',
            './liero2gus/SConscript',
            './lua/SConscript',
            './panzer/SConscript',
            #'./IoVM/SConscript',
			#'./ParserGen/SConscript',
			#'./SmallLang/SConscript',
			]
			
env = Environment(
		CPPPATH = ['.', '#loadpng', '#lua', '#Console', '#GUI', '#Utility'],
		LIBPATH = [os.path.join('#lib/', subfolder)],
	)
	
# NOTE: Always leave a trailing space after any flags you input so
# that more flags can be added later without problems

if conf == 'posix':
	env.Append(
		CPPPATH = ['/usr/local/include/zoidcom', '/usr/local/include/fmod', '/usr/local/include/boost-1_32'],
		LIBPATH = ['/usr/local/lib', '/usr/X11R6/lib'],
		CPPFLAGS = '-pipe -Wall -Wno-reorder -DGLIPTIC_SCREENSHOT_HAX ',
		#LINKFLAGS = '-pg',
	)
	env.Replace(
		CXX = 'g++-3.4',
	)
	
	if build == 'release':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG ')
	elif build == 'debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG ')
	elif build == 'dedserv':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG -DDEDSERV ')
	elif build == 'dedserv-debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG -DDEDSERV ')
		
elif conf == 'mingw-cross':
	mingwPath = ARGUMENTS.get('mingw-path', '/usr/local/mingw/')
	env.Append(
		CPPPATH = [os.path.join(mingwPath, 'include', 'boost-1_32'), os.path.join(mingwPath, 'include', 'fmod')],
		CPPFLAGS = '-pipe -Wall -DWINDOWS ',
	)
	env.Replace(
		CXX = os.path.join(mingwPath, 'bin', 'g++'),
		CC = os.path.join(mingwPath, 'bin', 'gcc'),
		LD = os.path.join(mingwPath, 'bin', 'ld'),
		RANLIB = os.path.join(mingwPath, 'bin', 'ranlib'),
		AR = os.path.join(mingwPath, 'bin', 'ar'),
	)
	
	if build == 'release':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG ')
	elif build == 'debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG ')
	elif build == 'dedserv':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG -DDEDSERV ')
	elif build == 'dedserv-debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG -DDEDSERV ')
elif conf == 'basara':
	env.Append(
		CPPPATH = ['/usr/local/include/zoidcom'],
		LIBPATH = ['/usr/local/lib'],
		CPPFLAGS = '-Wall ',
		)
	
	if build == 'release':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG ')
	elif build == 'debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG ')
	elif build == 'dedserv':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG -DDEDSERV ')
	elif build == 'dedserv-debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG -DDEDSERV ')
elif conf == 'darka':
	env.Append(
		CPPPATH = ['/usr/local/include/zoidcom'],
		LIBPATH = ['/usr/local/lib'],
		CPPFLAGS = '-Wall ',
		)
	
	if build == 'release':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG -fomit-frame-pointer ')
	elif build == 'debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG ')
	elif build == 'dedserv':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG -DDEDSERV -fomit-frame-pointer ')
	elif build == 'dedserv-debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG -DDEDSERV ')
elif conf == 'lucas':
	env.Append(
		CPPPATH = ['/usr/local/include', '/usr/local/include/zoidcom', '/usr/local/include/fmod', '/usr/local/include/boost-1_33'],
		LIBPATH = ['/usr/local/lib', '/usr/X11R6/lib'],
		CPPFLAGS = '-Wall ',
		)
	env.Replace(
		CXX = 'g++-3.4',
	)
	
	if build == 'release':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG ')
	elif build == 'debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG ')
	elif build == 'dedserv':
		env.Append(CPPFLAGS = '-O3 -g -DNDEBUG -DDEDSERV -fomit-frame-pointer ')
	elif build == 'dedserv-debug':
		env.Append(CPPFLAGS = '-O0 -g -DDEBUG -DDEDSERV ')
# add more platforms here
	
#Builds all cpp and c files in this directory and returns a list of nodes
def getObjects(env):
	sourcePattern = re.compile('\.(cpp|c)$')
	
	sources = []
	
	for i in os.listdir('.'):
		if sourcePattern.search(i):
			sources.append(i)
	
	buildDir = os.path.join('.build', subfolder)
	env.BuildDir(buildDir, '.', duplicate = 0)
	objects = []
	for s in sources:
		objects += env.Object(os.path.join(buildDir, s))
			
	return objects


SConscript(sconscript, exports=['env', 'getObjects', 'getLibName', 'getBinName', 'conf', 'subfolder', 'build'])
            
if conf == 'mingw-cross':
	env.Append(BUILDERS = {'Strip' : Builder(action = os.path.join(mingwPath, 'bin', 'strip') + ' $SOURCE')})
	gus = getBinName('gusanos')
	gus = env.Install('/usr/local/htdocs/stuff/gusanos-alpha', gus)
	env.Strip(gus)
	env.Alias('upload', '/usr/local/htdocs/stuff/gusanos-alpha')