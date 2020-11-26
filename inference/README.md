# msl-tf-lite-inference

Steps to install

make sure gcc and g++ 5 are installed

1. Recursively clone directory.
2. From the root of this repository: ```cd libs/tensorflow```, then ```./tensorflow/lite/tools/download_dependencies.sh``` and ```./tensorflow/lite/tools/build_lib.sh```
3. Open inference project in eclipse and build.
4. Alternatively: ```mkdir build; cd build; cmake ..; cd ..; make```.
