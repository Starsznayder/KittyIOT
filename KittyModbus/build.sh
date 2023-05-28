PROJ_PATH="/kitty"

sudo systemctl disable com6
sudo systemctl stop com6
sudo systemctl disable KittyModbus
sudo systemctl stop KittyModbus
#sudo rm -rf "$PROJ_PATH/IOT"
sudo rm -rf "$PROJ_PATH/dev"
sudo mkdir "$PROJ_PATH"
sudo chown $(whoami) "$PROJ_PATH"
sudo chgrp $(whoami) "$PROJ_PATH"
sudo rm -rf "$PROJ_PATH/IOT/KittyModbus"
mkdir "$PROJ_PATH/IOT"
mkdir "$PROJ_PATH/IOT/KittyModbus"
mkdir "$PROJ_PATH/dev"
mkdir "$PROJ_PATH/IOT/KittyModbus/sbin"
mkdir "$PROJ_PATH/IOT/KittyModbus/ini"
cp -a sbin/. "$PROJ_PATH/IOT/KittyModbus/sbin/"
cp -a ini/. "$PROJ_PATH/IOT/KittyModbus/ini/"
sudo cp "$PROJ_PATH/IOT/KittyModbus/sbin/com6.service" /etc/systemd/system/com6.service
sudo systemctl enable com6
sudo systemctl start com6
sudo cp "$PROJ_PATH/IOT/KittyModbus/sbin/KittyModbus.service" /etc/systemd/system/KittyModbus.service

rm -rf "$PROJ_PATH/IOT/build/KittyModbus"
mkdir "$PROJ_PATH/IOT/build"
mkdir "$PROJ_PATH/IOT/build/KittyModbus"
currPath=$(pwd)
projPath="${currPath}/KittyModbus.pro"
echo $projPath
cd "$PROJ_PATH/IOT/build/KittyModbus"
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath
sudo systemctl enable KittyModbus
sudo systemctl start KittyModbus


