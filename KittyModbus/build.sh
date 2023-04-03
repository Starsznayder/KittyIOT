PROJ_PATH="/kitty"

sudo systemctl disable com6
sudo systemctl stop com6
sudo rm -rf "$PROJ_PATH/IOT"
sudo rm -rf "$PROJ_PATH/dev"
sudo mkdir "$PROJ_PATH"
sudo chown $(whoami) "$PROJ_PATH"
sudo chgrp $(whoami) "$PROJ_PATH"
mkdir "$PROJ_PATH/IOT"
mkdir "$PROJ_PATH/dev"
cp -R sbin "$PROJ_PATH/IOT/sbin"
cp -R ini "$PROJ_PATH/IOT/ini"
sudo cp "$PROJ_PATH/IOT/sbin/com6.service" /etc/systemd/system/com6.service
sudo systemctl enable com6
sudo systemctl start com6

mkdir "$PROJ_PATH/IOT/build"
currPath=$(pwd)
projPath="${currPath}/KittyModbus.pro"
echo $projPath
cd "$PROJ_PATH/IOT/build"
qmake $projPath -spec linux-g++
make -j2
make install
cd $currPath
sudo cp sbin/egtab /etc/egtab


