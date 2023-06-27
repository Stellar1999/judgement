go build
mkdir build
cd build || return
cmake ../judge-core-src/.
make
mv judge-core ../judge-core
cd ../ || return
rm -rf build
