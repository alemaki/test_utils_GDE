import os

# The only thing this build needs from whoever includes it is a godot-cpp-configured Environment. 
# This whole test_utils/ folder can be dropped into any godot-cpp project and pulled in with just:
#
#   test_utils_lib = SConscript(os.path.join('path', 'to', 'test_utils', 'SConscript'), exports='env')
Import('env')

own_dir = os.path.abspath('.')

def collect_cpp_files(root):
    cpp_files = []
    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            if filename.endswith('.cpp'):
                cpp_files.append(os.path.join(dirpath, filename))
    return cpp_files

# Mirror godot-cpp's own convention:
# Append directories to the cpp path so children can consume the content of the files
env.AppendUnique(CPPPATH=[own_dir, os.path.join(own_dir, 'doctest')])

test_utils_env = env.Clone()

sources = collect_cpp_files(own_dir)

library_name = 'libtest_utils' + env['suffix'] + env['LIBSUFFIX']
test_utils_lib = test_utils_env.StaticLibrary(target=env.File(os.path.join('bin', library_name)), source=sources)

# Mirror godot-cpp's own convention:
env.AppendUnique(LIBS=[test_utils_lib])

Return('test_utils_lib')
