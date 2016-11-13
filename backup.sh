

echo "pack version 0.36.0"
tar -czf mythtv-controller-0.36.0.tgz createdb.sql apt-get.sh apt-get_12.10.sh  debian checkwakeup.sh *.cpp *.h startmovie.pl startmovie.sh startrecorded.pl startrecorded.sh mythtv-controller.conf  startmovie.sh startrecorded.sh startrecorded.pl install.txt readme.txt filetypes.txt lirc/mythtv-controller info/ide.txt install.txt readme.txt charset Makefile fmodstudioapi10811linux.tar.gz images tema1 tema2 tema3 tema4 tema5 tema6 tema7 tema8 tema9 tema10 tema11 tema12 tema13 convert/dvdcover_mask.jpg

echo "Backup mythtv-controller debian/tar.gz file to sourcefource."
scp mythtv-controller-0.36.0.tgz mythtv-controller-0.36.0.deb hanshenrik32,mythtvcontrolle@frs.sourceforge.net:/home/frs/project/m/my/mythtvcontrolle/
