#! /bbs/opt/bin/perl -w

use strict;
use English "-no_match_vars";

my $pkg = "baenet";
my $PKG = uc($pkg);
my $xsdfile = "$pkg.xsd";
my @generatedComponents = qw(httpbasicheaderfields
httpcontenttype            httprequestline
httprequestmethod          httpresponseheaderfields
httphost                   httpstartline
httpstatuscode             httpstatuscodecategory
httpmessagetype            httptransferencoding
httpviarecord              httpstatusline
httprequestheaderfields    httprequestheader);

my @optionComponentFiles;
my @configschemaFiles;
my $areWritable = 1;

foreach my $generatedClass (@generatedComponents) {
    foreach my $suffix (qw(.h .cpp .t.cpp)) {
        my $optionComponentFile = $pkg."_".$generatedClass.$suffix;
        if (-w $optionComponentFile) {
            push @optionComponentFiles, $optionComponentFile;
        }
        else {
            printf STDERR "Can't open %s for writing\n", $optionComponentFile;
            $areWritable = 0;
        }
    }
}

die "Cannot write to output files.  Quitting.\n" unless $areWritable;

# Segregate 'baexml_configschema' from other generated components.
unshift @configschemaFiles, pop @optionComponentFiles;
unshift @configschemaFiles, pop @optionComponentFiles;
unshift @configschemaFiles, pop @optionComponentFiles;

# Every generated class name will start with the recognizable word
# "dummyprefix":
my $dummyPrefix = "dummyprefix";
my $DUMMYPREFIX = uc($dummyPrefix);

# Read the file named by the specified first argument, convert from namespace-
# style packaging to prefix-style packaging, and write result to the specified
# second argument.
sub mungeGeneratedFile($$)
{
    my ($infilename, $outfilename) = @_;

    open INFILE, "< $infilename" or die "Cannot open $infilename";
    open OUTFILE, "> $outfilename" or die "Cannot open $outfilename";

    my $line;
    while (defined($line = <INFILE>)) {
        # skip lines beginning, ending, or using the namespace scope
        next if $line =~ m(namespace *$pkg *{)o;
        next if $line =~ m(}\s*// close namespace $pkg)o;
        next if $line =~ m(using namespace\s([a-zA-Z_0-9]+::)*${pkg}\s?;)o;

        # Transform prefixes:
        $line =~ s/${pkg}_${dummyPrefix}/${pkg}_/;
        $line =~ s/${PKG}_${DUMMYPREFIX}/${PKG}_/;
        $line =~ s/${pkg}:://igo;
        $line =~ s/${dummyPrefix}/${pkg}_/igo;

        print OUTFILE $line;
    }
}

# Read the configschema file named by the specified first argument, convert
# from namespace- style packaging to prefix-style packaging, and write result
# back the file named by the specified second argument.
sub mungeConfigSchemaFile($$)
{
    my ($infilename, $outfilename) = @_;

    open INFILE, "< $infilename" or die "Cannot open $infilename";
    open OUTFILE, "> $outfilename" or die "Cannot open $outfilename";

    my $line;
    while (defined($line = <INFILE>)) {
        # skip lines beginning, ending, or using the namespace scope
        next if $line =~ m(namespace *$pkg *{)o;
        next if $line =~ m(}\s*// close namespace $pkg)o;
        next if $line =~ m(using namespace\s([a-zA-Z_0-9]+::)*${pkg}\s?;)o;

        # Transform prefixes:
        $line =~ s/${pkg}:://igo;
        $line =~ s/\b ConfigSchema \b/${pkg}_ConfigSchema/xgo;
        $line =~ s/ ? service configuration / '${pkg}' /go;

        print OUTFILE $line;
    }
}

# Generate the option files from the .xsd file:
my $writable = 1;

0 == system("bas_codegen.pl $xsdfile -m msg -p baenet -E -P $dummyPrefix "
          . "--testD rivers=yes --dualSTL=yes --noAggregateConversion "
          . "--ignoreCase=yes") or
    die "bas_codegen.pl failed\n";

foreach my $outputfile (@optionComponentFiles) {
    my $inputfile = $outputfile;
    $inputfile =~ s/${pkg}_/${pkg}_${dummyPrefix}/go;
    print "Processing $inputfile to $outputfile\n";
    mungeGeneratedFile($inputfile, $outputfile);
    unlink($inputfile);
}
