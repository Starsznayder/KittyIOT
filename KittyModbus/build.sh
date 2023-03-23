sudo systemctl disable com6
sudo systemctl stop com6
sudo rm -rf /kitty
sudo mkdir /kitty
sudo chown $(whoami) /kitty
sudo chgrp $(whoami) /kitty
mkdir /kitty/IOT
mkdir /kitty/dev
cp -R sbin /kitty/IOT/sbin
cp -R ini /kitty/IOT/ini
sudo cp /kitty/IOT/sbin/com6.service /etc/systemd/system/com6.service
sudo systemctl enable com6
sudo systemctl start com6

mkdir /kitty/IOT/build
currPath=$(pwd)
projPath="${currPath}/KittyModbus.pro"
echo $projPath
cd /kitty/IOT/build
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath
sudo cp sbin/egtab /etc/egtab


