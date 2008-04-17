#!/usr/local/bin/perl
#     R. Mankel, DESY Hamburg     17-Oct-2007
#     A. Parenti, DESY Hamburg    16-Apr-2008
#     $Revision: 1.1 $
#     $Date: 2008/04/10 16:10:11 $
#
#  Try periodically to fire merge job.
#  Terminate when done
#
#  Usage: mps_auto.pl seconds
#

BEGIN {
use File::Basename;
unshift(@INC, dirname($0)."/mpslib");
}
use Mpslib;

$seconds = 60;

# parse the arguments
while (@ARGV) {
  $arg = shift(ARGV);
  if ($arg =~ /\A-/) {  # check for option 
    if ($arg =~ "h") {
      $helpwanted = 1;
    }
    elsif ($arg =~ "d") {
      $localdir = 1;
    }
    $optionstring = "$optionstring$arg";
  }
  else {                # parameters not related to options
    $i = $i + 1;
    if ($i eq 1) {
      $seconds = $arg;
    }
  }
}


if ($seconds <20) {
  print "Set seconds to 20\n";
  $seconds = 20;
}

$done = 0;

while ($done == 0) {
    read_db();
    # go to merge job 
    $i = (@JOBID) - 1;
    if (@JOBSTATUS[$i] ne "SETUP") {
	$done = 1;
	break;
    }
    else {
	print "mps_fire.pl -m\n";
	#system "mps_fire.pl -m";
	print "sleep $seconds\n";
	system "sleep $seconds";
    }
}
