// 09/23/2009: Arthur Chiu (achiu21)

// These dummy headers are trying to accomplish the following: bde_build.pl
// automatically copy all the .h files to the include directory for a non
// standard compliant package (bsl+stdhdrs in this case).

// However, this process is complicated by two problems.  First, there are a
// lot of headers that does not end with .h here, such as <limits> and
// <limits.SUNWCCh>.  This problem is solved by creating a script 'cphdrs.pl'
// that, given a header file name (provided by bde_build.pl), it will copy that
// file and also its corresponding without the .h and with the .SUNWCCh.

// The second problem is that when a header does not have a
// corresponding .h file, such as <algorithm>, there is no way to make
// bde_build.pl provide this name unless we create this dummy header file.

#error "This dummy file is used by build script and should not be included"
