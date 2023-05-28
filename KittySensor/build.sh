PROJ_PATH="/kitty"

sudo systemctl disable KittySensor
sudo systemctl stop KittySensor
sudo rm -rf "$PROJ_PATH/dev"
sudo mkdir "$PROJ_PATH"
sudo chown $(whoami) "$PROJ_PATH"
sudo chgrp $(whoami) "$PROJ_PATH"
sudo rm -rf "$PROJ_PATH/IOT/KittySensor"
mkdir "$PROJ_PATH/IOT"
mkdir "$PROJ_PATH/IOT/KittySensor"
mkdir "$PROJ_PATH/dev"
mkdir "$PROJ_PATH/IOT/KittySensor/sbin"
mkdir "$PROJ_PATH/IOT/KittySensor/ini"
cp -a sbin/. "$PROJ_PATH/IOT/KittySensor/sbin/"
cp -a ini/. "$PROJ_PATH/IOT/KittySensor/ini/"
sudo cp "$PROJ_PATH/IOT/KittySensor/sbin/KittySensor.service" /etc/systemd/system/KittySensor.service

rm -rf "$PROJ_PATH/IOT/build/KittySensor"
mkdir "$PROJ_PATH/IOT/build"
mkdir "$PROJ_PATH/IOT/build/KittySensor"
currPath=$(pwd)
projPath="${currPath}/KittySensor.pro"
echo $projPath
cd "$PROJ_PATH/IOT/build/KittySensor"
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath
sudo systemctl enable KittySensor
sudo systemctl start KittySensor


