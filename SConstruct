import multiprocessing

SetOption('num_jobs', multiprocessing.cpu_count())

vars = Variables('.config.py')
vars.Add(BoolVariable('DEBUG', 'Set to build for debug', False))

env = DefaultEnvironment(variables=vars, CPPPATH=['#/include'])
env.MergeFlags('-Wall -std=c++17')

if env['DEBUG']:
    env.MergeFlags('-g')
else:
    env.Append(CXXFLAGS = '-O2')

Decider('MD5-timestamp')

SConscript('src/SConscript', exports='env', variant_dir='build', duplicate=0)
