#!/usr/bin/env python
import os
import sys
import platform

curwd = os.path.dirname(__file__)
root = os.path.normpath(os.path.join(curwd, os.pardir))
build = os.path.join(os.path.abspath(root), 'build')
gyplib = os.path.join(root, 'tools', 'gyp', 'pylib')

sys.path.insert(0, gyplib)
import gyp

def run_gyp(args):
    rc = gyp.main(args)
    if rc != 0:
        print('bde_gyp.py: error: gyp exited with rc %d' % rc)
        sys.exit(rc)

if __name__ == '__main__':
    main = os.path.join(os.path.abspath(root), 'bde.gyp')
    common = os.path.join(os.path.abspath(root), 'common.gypi')
    args = sys.argv[1:]
    args.append(main)
    args.extend(['-I', common]);
    args.extend(['--generator-output', build])
    args.extend(['-Goutput_dir=' + build])
    args.append('-Gdefault_target=all_libs');
    args.append('--depth=' + root);
    args.extend(['-D', 'product_rollup_path=' + build]);

    if (platform.uname()[0] == "Windows"):
        args.extend(['-D', 'python_path="' + sys.executable + '"']);
    else:
        args.extend(['-D', 'python_path=' + sys.executable]);
        
    # override 64-bit default builds to be 32-bit builds on IA-32 x86 platforms
    mach = (platform.uname())[4];
    if (mach == 'i386' or mach == 'i686' or mach == 'x86'):
        args.extend(['-D', 'ABI_bits=32']);
    run_gyp(list(args))
