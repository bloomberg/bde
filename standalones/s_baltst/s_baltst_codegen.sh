#!/usr/bin/env bash

toLower() {
    sed -f $tmp_lower_sed "$@"
}

toUpper() {
    sed -f $tmp_upper_sed "$@"
}

if [ $# -ne 0 ] ; then
    echo Error: $0 "$@"                               1>&2
    echo  No command-line arguments expected: aborted 1>&2
    exit 1
fi

pwd -P | grep 'bde/sideshows/s_baltst$' >/dev/null
if [ 0 -eq $? ] ; then
    cat <<End 1>&2
Do not run this script in the source directory, it will overwrite all
source files.  Run it in a separate directory and pick and choose the
components you want.
End
    exit 1
fi

cat <<End
Will generate one component per class.  The components will be named
's_baltst_<class name>', except for components containing
classes 's_baltst::Choice[1-3]' and 's_baltst::Sequence[1-6]', which will
be in components 's_baltst_ratsnest<class name>'.  These components have
many dependencies between them, including circular dependencies, and
must be combined into 's_baltst_ratsnest.h' and 's_baltst_ratsnest.cpp'.
See package documentation as to how to do this.
End

prefix=tmp.s_baltst_codegen
tmp_lower_sed=$prefix.lower.$$.sed
tmp_upper_sed=$prefix.upper.$$.sed
tmp_allFiles_sed=$prefix.include.$$.sed
tmp_this_sed=$prefix.component.$$.sed

rm -f s_baltst_tmp_singles*
bas_codegen.pl s_baltst.xsd -m msg -p s_baltst -C tmp_singles --msgSplit 1    \
                    --noExternalization --noHashSupport --noAggregateConversion
if [ $? -ne 0 ] ; then
    exit 1
fi

cat >$tmp_lower_sed <<End
s/A/a/g
s/B/b/g
s/C/c/g
s/D/d/g
s/E/e/g
s/F/f/g
s/G/g/g
s/H/h/g
s/I/i/g
s/J/j/g
s/L/l/g
s/M/m/g
s/N/n/g
s/O/o/g
s/P/p/g
s/Q/q/g
s/R/r/g
s/S/s/g
s/T/t/g
s/U/u/g
s/V/v/g
s/W/w/g
s/X/x/g
s/Y/y/g
s/Z/z/g
End

cat >$tmp_upper_sed <<End
s/a/A/g
s/b/B/g
s/c/C/g
s/d/D/g
s/e/E/g
s/f/F/g
s/g/G/g
s/h/H/g
s/i/I/g
s/j/J/g
s/l/L/g
s/m/M/g
s/n/N/g
s/o/O/g
s/p/P/g
s/q/Q/g
s/r/R/g
s/s/S/g
s/t/T/g
s/u/U/g
s/v/V/g
s/w/W/g
s/x/X/g
s/y/Y/g
s/z/Z/g
End

cat <<End >$tmp_allFiles_sed
s/include <bdes_ident/include <bsls_ident/
s/^BDES_IDENT_RCSID/BSLS_IDENT_RCSID/
s/^BDES_IDENT_PRAGMA_ONCE$/BSLS_IDENT_PRAGMA_ONCE/
End

component_list=

rc=0
for f in s_baltst_tmp_singles*.h ; do
    b=$(basename $f .h);
    ucb=$(echo $b | toUpper)

    class=$(grep -e '^class' -e '^struct' $f)
    if [ $(echo "$class" | wc | sed -e 's/^ *//' -e 's/ .*$//') -ne 1 ] ; then
        echo Error: file $f does not contain exactly one class 1>&2
        rc=1
        break
    fi
    class=$(echo "$class" | sed -e 's/^class *//' -e 's/^struct *//'          \
                                                                 -e 's/ .*$//')
    class=$(echo -n "$class")
    class=$(echo $class | toLower)
    ucclass=$(echo $class | toUpper)
    echo $class | egrep -e '^choice[1-3]' -e '^sequence[1-6]' >/dev/null
    if [ 0 -eq $? ] ; then
        echo "$class found in $b, rat's nest component"
        c=s_baltst_ratsnest$class
        i=s_baltst_ratsnest
    else
        c=s_baltst_$class
        i=$c
    fi

    echo "$b -> $c"

    echo "s/include <$b.h>$/include <$i.h>/"               >>$tmp_allFiles_sed
    component_list=$(echo $component_list $c)

    echo "s#^// $b[.]h #// $i.h #"                              >$tmp_this_sed
    echo "s#^// $b[.]cpp #// $i.cpp #"                         >>$tmp_this_sed
    echo "s/ INCLUDED_$ucb/ INCLUDED_S_BALTST_$ucclass/"       >>$tmp_this_sed
    echo "s/BDES_IDENT_RCSID[(]${b}_/BSLS_IDENT_RCSID(${c}_/"  >>$tmp_this_sed

    mv $b.h   tmp.txt
    sed -f $tmp_this_sed <tmp.txt >$c.h
    mv $b.cpp tmp.txt
    sed -f $tmp_this_sed <tmp.txt >$c.cpp
done

if [ 0 -ne $rc ] ; then
    exit $rc
fi

for c in $component_list ; do
    mv $c.h tmp.txt
    sed -f $tmp_allFiles_sed <tmp.txt >$c.h
    mv $c.cpp tmp.txt
    sed -f $tmp_allFiles_sed <tmp.txt >$c.cpp
done

rm tmp.txt $tmp_allFiles_sed $tmp_lower_sed $tmp_upper_sed $tmp_this_sed
