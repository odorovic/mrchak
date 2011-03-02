import os
import platform
from os import path

NASMPATH = 'C:/nasm'
VERIPATH = 'C:/iverilog/bin'
PY26PATH = 'C:/python26'

platform = Platform()

def generate_as86(source, target, env, for_signature):
    return 'as86 -b %s -s %s %s' % (target[0], target[1], source[0])
as86builder = Builder(generator = generate_as86,
                      source_suffix='.pasm')

def generate_nasm(source, target, env, for_signature):
    return 'nasm -Ires/ -fbin -o %s %s > %s' % (target[0], source[0], target[1])
nasmBuilder = Builder(generator=generate_nasm,
                      src_suffix='asm')


def generate_roms(source, target, env, for_signature):
    return 'python bin/mkroms.py -s %s -l %s -u %s %s' % (source[1],
                                               target[0],
                                               target[1],
                                               source[0])
rombuilder = Builder(generator=generate_roms,
                     suffix = '.img')
veribuilder = Builder(action='iverilog -Isystem -o $TARGET $SOURCE',
                      suffix = '.vvp',
                      src_suffix = '.v')
cppbuilder = Builder(action='cpp $SOURCE $TARGET',
                     suffix='.pasm',
                     src_suffix='asm')

def build_shellscript(source, target, env):
    romlo = str(source[0])
    romhi = str(source[1])
    vvpfile = str(source[2])
    cmpath = path.dirname(str(source[3]))
    cmodel = path.splitext(path.basename(str(source[3])))[0]
    target_name = str(target[0])
    print cmpath, cmodel
    cmd = "vvp -M %s -m%s %s +ROMLO=%s +ROMHI=%s" % (
            cmpath,
            cmodel,
            vvpfile,
            romlo,
            romhi)
    f = open(target_name, "w");
    if env['PLATFORM'].name == 'posix':
        print >> f, "#!/bin/bash"
    print >> f 
    print >> f, cmd
    f.close()
    os.chmod(target_name, 0755)
    return 0

if platform.name == 'win32':
    SCRIPTSUFFIX = '.bat'
else:
    SCRIPTSUFFIX = '.sh'

runscriptbuilder = Builder(action=build_shellscript,
                           suffix=SCRIPTSUFFIX)

env = Environment(PLATFORM = platform,
                  SHLIBPREFIX='',
                  SHLIBSUFFIX='.vpi')

if platform.name == 'win32':
    cflagstr = '-TC -D__CHAR_BIT__=8 -MD'
    cflags = env.ParseFlags(cflagstr)
    env.Append(CPPPATH=['c:\iverilog\include\iverilog', 'win_include'])
else:
    cflagstr = os.popen('iverilog-vpi --cflags').read()
    cflags = env.ParseFlags(cflagstr)
env.MergeFlags(cflags)

env.Append(BUILDERS={'Nasm':nasmBuilder,
                     'Rom':rombuilder,
                     'RunScript': runscriptbuilder,
                     'Verilog': veribuilder,
                     'Preprocess': cppbuilder})



if platform.name == 'win32':
    env.Append(LIBS=['veriuser', 'vpi'])
    env.Append(LINKFLAGS=['/DEBUG'])
    env.Append(LIBPATH=['C:/iverilog/lib'])
    env.AppendENVPath('PATH', NASMPATH)
    env.AppendENVPath('PATH', VERIPATH)
    env.AppendENVPath('PATH', PY26PATH)
else:
    env.AppendENVPath('PATH', os.getcwd()+'/bin')

    ldflagstr = os.popen('iverilog-vpi --ldflags').read()
    ldflags = env.ParseFlags(ldflagstr)
    env.MergeFlags(ldflags)
    ldlibsstr = os.popen('iverilog-vpi --ldlibs').read()
    ldlibs = env.ParseFlags(ldlibsstr)
    env.MergeFlags(ldlibs)

env.Append(CPPPATH=['include'])
env.Decider('make')

Export('env')
cmodel = env.SConscript('cmodel/SConscript',
               variant_dir='build/$PLATFORM/cmodel',
               duplicate=0)
progs, roms = env.SConscript('res/SConscript',
               variant_dir='build/$PLATFORM/testprgs',
               duplicate=0)
verimodel = env.SConscript('system/SConscript',
               variant_dir='build/$PLATFORM/verilog',
               duplicate=0)

prognames = [path.splitext(path.basename(str(prog[0])))[0] for prog in progs]

scripts = [env.RunScript(target=t, source=srcs+verimodel+cmodel) for t, srcs in zip(prognames, roms)]

Default(scripts)
