PROJ_PATH="/kitty"

sudo systemctl disable KittyDevice
sudo systemctl stop KittyDevice
sudo mkdir "$PROJ_PATH"
sudo chown $(whoami) "$PROJ_PATH"
sudo chgrp $(whoami) "$PROJ_PATH"
rm -rf "$PROJ_PATH/IOT/KittyDevice"
mkdir "$PROJ_PATH/IOT"
mkdir "$PROJ_PATH/IOT/KittyDevice"
mkdir "$PROJ_PATH/dev"
mkdir "$PROJ_PATH/IOT/KittyDevice/sbin"
mkdir "$PROJ_PATH/IOT/KittyDevice/ini"
cp -a sbin/. "$PROJ_PATH/IOT/KittyDevice/sbin/"
cp -a ini/. "$PROJ_PATH/IOT/KittyDevice/ini/"
sudo cp "$PROJ_PATH/IOT/KittyDevice/sbin/KittyDevice.service" /etc/systemd/system/KittyDevice.service

mkdir "$PROJ_PATH/IOT/build"
rm -rf "$PROJ_PATH/IOT/build/KittyDevice"
mkdir "$PROJ_PATH/IOT/build/KittyDevice"
currPath=$(pwd)
projPath="${currPath}/KittyDevice.pro"
echo $projPath
cd "$PROJ_PATH/IOT/build/KittyDevice"
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath
sudo cp sbin/egtab /etc/egtab

sudo systemctl enable KittyDevice
sudo systemctl start KittyDevice
