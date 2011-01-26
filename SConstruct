# command line parameters
# V=1 - verbose build
# LIBC=0 - don't link with libc

import os
import config

SRCS = 'main.c'

SUBDIRS = 'arch/'+config.ARCH+' kernel lib cbashell test drivers'
if os.path.exists('../cbaos.extra'):
	SUBDIRS += ' ../cbaos.extra '

#env = DefaultEnvironment(ENV = {'PATH' : os.environ['PATH']})
env = DefaultEnvironment(ENV = os.environ)

env.Append(CPPPATH = ['#/include/'])
# copy path, and... what else is there?
# default means, that you don't need to prepend ie: env.Program()


#dict = env.Dictionary()
#for k in dict.keys():
#	print k, dict[k]

if 'CROSSCOMPILE' in dir(config):
	env['CC'] = config.CROSSCOMPILE+'-gcc'
	env['AS'] = config.CROSSCOMPILE+'-gcc'
	env['LINK'] = config.CROSSCOMPILE+'-gcc'
	env['OBJCOPY'] = config.CROSSCOMPILE+'-objcopy'

env['CFLAGS'] = ['-Wall', '-Os', '-g']

# import variables from config as defines, ie. ARCH_ARM_CORTEX_M3
for i in dir(config):
	value = eval('config.'+i)
	if i.startswith('CONFIG_'):
		if str(value).isdigit():
			env['CFLAGS'].append('-D'+i+'=' + str(value))
		else:
			env['CFLAGS'].append('-D'+i+'="' + value + '"')
		continue

	if i.isupper():
		env['CFLAGS'].append('-D'+i+'_' + value.upper().replace('-', '_').replace('.', '_'))


if env.has_key('OBJCOPY'):
	objcopy_bldr = Builder(action = env['OBJCOPY']+' $OCFLAGS $SOURCE $TARGET')
	env.Append(BUILDERS = {'ObjCopy':objcopy_bldr}) 
env.Append(BUILDERS = {'Size':Builder(action = 'size $SOURCE') })


# allow environment variables to work around these
for i in ['CC', 'CFLAGS']:
	if i in os.environ:
		env[i] = os.environ[i]

if ARGUMENTS.get('V') != '1':
	env['CCCOMSTR'] = "Compiling $SOURCE"
	env['ASCOMSTR'] = "Assembling $SOURCE"
	env['LINKCOMSTR'] = "Linking $TARGET"

Export('env config')

src = Split(SRCS)
src.append(SConscript([ i+'/SConscript'  for i in Split(SUBDIRS) ]))

Program(config.NAME, src)
Decider('timestamp-match')

if env.has_key('OBJCOPY'):
	env.ObjCopy(OCFLAGS='-O ihex', target=config.NAME.replace('.elf', '.hex'), source=config.NAME)
	env.ObjCopy(OCFLAGS='-O binary', target=config.NAME.replace('.elf', '.bin'), source=config.NAME)
env.Size(source=config.NAME)


# you'd make objcopy like this:
#       env = Environment()
#       bld = Builder(action = 'foobuild < $SOURCE > $TARGET')
#       env.Append(BUILDERS = {'Foo' : bld})
#       env.Foo('file.foo', 'file.input')
#       env.Program('hello.c')
