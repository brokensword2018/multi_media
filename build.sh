if [[ $1 = '-r' ]]; then
    echo "compile in clean state"
    rm -rf build
fi
mkdir -p build
cd build
cmake ..
make -j4