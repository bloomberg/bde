#!/usr/bin/perl

if (-d "/bb/data/bdetstdb/") {
    if (0 <= $#ARGV && "-f" == $ARGV[0]) {
        runcmd("rm -r /bb/data/bdetstdb");
    }
    else {
  print <<EOF;
/bb/data/bdetstdb directory already exists.  Please clean this up prior
to running this script.
EOF
        exit(1);
    }
}

if (!-e "bdetstdb.lrl") {
  print <<EOF;
This script must be run from a directory containing the configuration files and
scripts found in /bbcm/infrastructure/adapters/a_comdb2/test.
EOF

    exit 1
}

runcmd("mkdir /bb/data/bdetstdb/");
runcmd("cp * /bb/data/bdetstdb/");
changedir("/bb/data/bdetstdb");
runcmd("initcomdb2 bdetstdb.lrl");
runcmd("cp bdetstdb.lrl /bb/bin/");
runcmd("nstart bdetstdb comdb2.tsk bdetstdb 300 -lrl /bb/bin/bdetstdb.lrl");
sleep(10);
runcmd("./create_test_data.sh");

sub printUsage() {
  print <<EOF;
usage: create_test_database.pl
Create the 'bdetstdb' BDE test comdb2 database in /bb/data/bdetstdb/. This
script must be run from a directory containing the configuration files and
scripts found in /bbcm/infrastructure/adapters/a_comdb2/test.
EOF
}

sub changedir {
  my ($dir) = @_;
  print ">cd $dir\n";
  chdir("$dir") or
    die "Unable to find destination $dir";
}


sub runcmd {
  my($cmd) = @_;
  print ">$cmd\n";
  $status = system("$cmd");
  if ($? != 0) {
    die "$cmd failed to execute ";
  }
}
