# !/usr/bin

sudo apt-get update
sudo apt-get upgrade

sudo apt-get install git git-core wget tar cmake libssl-dev
cd /tmp

# git clone https://bitbucket.org/sol_prog/raspberry-pi-gcc-binary.git
# cd raspberry-pi-gcc-binary
# tar xjf gcc-9.1.0-armhf-raspbian.tar.bz2
# sudo cp -R /tmp/raspberry-pi-gcc-binary/gcc-9.1.0 /opt
# cd /tmp
# rm -rf raspberry-pi-gcc-binary

# installing wiring PI library
if [ "$1" = "installWiringPi" ] 
then
    cd /tmp
    git clone git://git.drogon.net/wiringPi
    cd wiringPi
    ./build
    cd ..
    rm -rf wiringPi
fi

cd /tmp
git clone -b 'v1.1.0' --single-branch --depth 1 https://github.com/Tencent/rapidjson.git
cd rapidjson
mkdir build
cd build
cmake -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DCMAKE_PREFIX_PATH=/usr ..
sudo make install
cd ../..
rm -rf rapidjson

cd /tmp
git clone -b 'v1.3.1' --single-branch --depth 1 https://github.com/gabime/spdlog.git
cd spdlog
mkdir build
cd build
cmake -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_BUILD_EXAMPLES=OFF -DSPDLOG_BUILD_BENCH=OFF -DCMAKE_PREFIX_PATH=/usr ..
sudo make install
cd ../..
rm -rf spdlog

cd /tmp
wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz
tar -xvzf boost_1_70_0.tar.gz
cd boost_1_70_0
chmod +x bootstrap.sh
./bootstrap.sh
sudo ./b2 install --with-program_options --with-filesystem --with-system --with-coroutine
cd ../..
rm -rf boost_1_70_0

cd /tmp
git clone https://github.com/martinmozi/jsonVariant.git
cd jsonVariant
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
sudo make install
cd ../..
rm -rf jsonVariant

cd /tmp
git clone https://github.com/martinmozi/restapi-server.git
cd restapi-server
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
sudo make install
cd ../..
rm -rf restapi-server


#export CC=/opt/gcc-9.1.0/bin/gcc-9.1
#export CXX=/opt/gcc-9.1.0/bin/g++-9.1
