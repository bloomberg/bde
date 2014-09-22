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

# ----------------------------------------------------------------------------
# Copyright 2013 Bloomberg Finance L.P.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ----------------------------- END-OF-FILE ----------------------------------
