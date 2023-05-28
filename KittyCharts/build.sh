PROJ_PATH="/kitty"

sudo systemctl disable KittyCharts
sudo systemctl stop KittyCharts
sudo mkdir "$PROJ_PATH"
sudo chown $(whoami) "$PROJ_PATH"
sudo chgrp $(whoami) "$PROJ_PATH"
sudo rm -rf "$PROJ_PATH/IOT/KittyCharts"
mkdir "$PROJ_PATH/IOT"
mkdir "$PROJ_PATH/IOT/KittyCharts"
mkdir "$PROJ_PATH/IOT/KittyCharts/sbin"
mkdir "$PROJ_PATH/IOT/KittyCharts/ini"
cp -a sbin/. "$PROJ_PATH/IOT/KittyCharts/sbin/"
cp -a ini/. "$PROJ_PATH/IOT/KittyCharts/ini/"
sudo cp "$PROJ_PATH/IOT/KittyCharts/sbin/KittyCharts.service" /etc/systemd/system/KittyCharts.service

rm -rf "$PROJ_PATH/IOT/build/KittyCharts"
mkdir "$PROJ_PATH/IOT/build"
mkdir "$PROJ_PATH/IOT/build/KittyCharts"
currPath=$(pwd)
projPath="${currPath}/KittyCharts.pro"
echo $projPath
cd "$PROJ_PATH/IOT/build/KittyCharts"
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath
sudo systemctl enable KittyCharts
sudo systemctl start KittyCharts


