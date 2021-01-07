To build the example code in this directory:

On a General Linux Machine
--------------------------
```
for test in `ls *.cpp`
do
    pcomp $test
done
```

On a Linux VM
-------------
Install a build of BDE:
```
eval `bde_build_env.py`
export PKG_CONFIG_PATH=$BDE_CMAKE_INSTALL_DIR/opt/bb/lib64/pkgconfig/
for comp in bsl bdl bal inteldfp pcre2
do
    for tail in '' '-headers' '-release-symlink' '-pkgconfig'
    do
        cmake_build.py install --install_dir=$ID  --component="${comp}${tail}"
    done
done
```

Build the example code:
```
for test in `ls *.cpp`
do
    $CXX --std=c++17 $test -o $test.tsk $(pkg-config --define-prefix --libs --cflags bal,bdl,bsl)
done
```
