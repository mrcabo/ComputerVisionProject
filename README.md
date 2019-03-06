# ComputerVisionProject

## Requirements
CMake

FreeImage


## Installation
For FreeImage:
```
sudo apt install libfreeimage3 libfreeimage-dev
```
Image data can be downloaded from:
```
http://www.cvlibs.net/datasets/kitti/eval_scene_flow.php?benchmark=stereo
http://vision.middlebury.edu/stereo/data/
```
## Build
```
cmake --build <path-to-project>/cmake-build-debug --target ComputerVisionProject -- -j 2
```
