call emsdk activate latest

python ../../oxygine/oxygine-framework//tools/others/embed_folder_js.py -s ../data

mkdir build_emsc
cd build_emsc
cmake -DCMAKE_TOOLCHAIN_FILE="D:/gdev/prj/emscripten/emsdk/fastcomp/emscripten/cmake/Modules/Platform/emscripten.cmake" -G"Unix Makefiles" ..
make
cd ..