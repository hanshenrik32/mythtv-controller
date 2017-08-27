#!/usr/bin/perl

$moviepath=@ARGV[0];

#$moviepath="dette er et eks.iso";

$moviepath=~ s/ /\ /g;

print $moviepath . "\n";

my @args = ('/usr/bin/vlc','-f', $moviepath);
my $scpresult = system(@args);
