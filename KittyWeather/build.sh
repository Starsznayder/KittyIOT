PROJ_PATH="/kitty"
sudo systemctl disable KittyWeather
sudo systemctl stop KittyWeather
sudo rm -rf "$PROJ_PATH/IOT/Weather"
sudo mkdir "$PROJ_PATH"
sudo chown $(whoami) "$PROJ_PATH"
sudo chgrp $(whoami) "$PROJ_PATH"
mkdir "$PROJ_PATH/IOT"
mkdir "$PROJ_PATH/IOT/Weather"
cp -R ini "$PROJ_PATH/IOT/Weather/ini"

mkdir "$PROJ_PATH/IOT/Weather/build"
currPath=$(pwd)
projPath="${currPath}/KittyWeather.pro"
echo $projPath
cd "$PROJ_PATH/IOT/Weather/build"
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath

sudo cp "sbin/KittyWeather.service" /etc/systemd/system/KittyWeather.service
sudo systemctl enable KittyWeather
sudo systemctl start KittyWeather

