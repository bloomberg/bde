#! /bbs/opt/bin/perl -w

# usage: cphdrs <source> <target>
#
# Copies <source> file to <target> file.  If <source> has name
# 'bsl_filename.h' and if there exists a 'filename' and/or 'filename.SUNWCCh',
# then they are also copied to the same destination directory.

use strict;

die "usage: cphdrs <source> <target>" unless (@ARGV == 2);

my ($source, $target) = @ARGV;

sub docopy($$)
{
    my ($source, $target) = @_;

    open SOURCE, "< $source" or die "Cannot open $source for reading";
    my $attempt = 1;
    RETRY: while(1) {
        if(open TARGET, "> $target") {
            last RETRY;
        }
        elsif ($attempt++ < 5) {
            warn "WARNING: couldn't open $target (error is $!) - sleeping before attempt # $attempt";
            sleep 5;
        }
        else {
            die "Cannot open $target for writing - error $!";
        }
    }

    local $/; # slurp mode
    my $s = <SOURCE>;
    print TARGET $s;

    close SOURCE;
    close TARGET;
}

docopy($source, $target);

if (($source =~ /dummy/) && ($target =~ /dummy/)) {

    $source =~ s{dummy_([^/\\]*)\.h$}{$1};
    $target =~ s{dummy_([^/\\]*)\.h$}{$1};

}

docopy($source, $target) if (-f $source);
docopy("$source.SUNWCCh", "$target.SUNWCCh") if (-f "$source.SUNWCCh");

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
