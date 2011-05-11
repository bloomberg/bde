#! /bbs/opt/bin/perl -w

use strict;
use English "-no_match_vars";

my $pkg = "bdem";
my $PKG = uc($pkg);
my $xsdfile = "$pkg.xsd";
my @optionComponents = qw(bdem_bdemencoderoptions bdem_bdemdecoderoptions
                          bdem_berdecoderoptions);

# For each specified 'component' in the argument list, generate three names:
# 'component.h', 'component.cpp', and 'component.t.cpp'.  Die unless all of the
# generated names refer to writable files.
sub componentFiles(@)
{
    my @result;
    my $errors = "";
    foreach my $component (@ARG) {
        foreach my $suffix (qw(.h .cpp .t.cpp)) {
            my $componentFile = $component.$suffix;
            if (-w $componentFile) {
                push @result, $componentFile;
            }
            else {
                $errors .= "Can't open ".$componentFile." for writing\n";
            }
        }
    }

    die $errors if $errors;

    return @result;
}

my @optionComponentFiles = componentFiles(@optionComponents);
my @configschemaFiles    = componentFiles(${pkg}."_configschema");

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
        
0 == system("bas_codegen.pl $xsdfile -m msg -E -P $dummyPrefix") or
    die "bas_codegen.pl failed\n";

foreach my $outputfile (@optionComponentFiles) {
    my $inputfile = $outputfile;
    $inputfile =~ s/${pkg}_/${pkg}_${dummyPrefix}/go;
    print "Processing $inputfile to $outputfile\n";
    mungeGeneratedFile($inputfile, $outputfile);
    unlink($inputfile);
}

# Generate the schemaconfig component from the .xsd file:
0 == system("bas_codegen.pl -p ${pkg} -m cfg -E -s $pkg $xsdfile") or
    die "bas_codegen.pl failed\n";

foreach my $configschemaFile (@configschemaFiles) {
    my $tmpfile = $configschemaFile.".tmp";
    print "Processing $configschemaFile to $tmpfile\n";
    mungeConfigSchemaFile($configschemaFile, $tmpfile);
    rename($tmpfile, $configschemaFile);
}

