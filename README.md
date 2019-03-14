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
http://vision.middlebury.edu/stereo/data/scenes2001/
```
## Build
```
cmake --build <path-to-project>/cmake-build-debug --target ComputerVisionProject -- -j 2
```
## Maxtree info
Images need to be grayscale. to convert use this command:
```
convert -colorspace GRAY <iamge>.ppm <image>.tiff
```
Options for maxtree calculation:
```
Usage: ./cmake-build-debug/ComputerVisionProject <input image> <attrib> <lambda> [decision] [output image] [template]
Where attrib is:
        0 - Area
        1 - Area of min. enclosing rectangle
        2 - Square of diagonal of min. enclosing rectangle
        3 - Cityblock perimeter
        4 - Cityblock complexity (Perimeter/Area)
        5 - Cityblock simplicity (Area/Perimeter)
        6 - Cityblock compactness (Perimeter^2/(4*PI*Area))
        7 - Large perimeter
        8 - Large compactness (Perimeter^2/(4*PI*Area))
        9 - Small perimeter
        10 - Small compactness (Perimeter^2/(4*PI*Area))
        11 - Moment of Inertia
        12 - Elongation: (Moment of Inertia) / (area)^2
        13 - Mean X position
        14 - Mean Y position
        15 - Jaggedness: Area*Perimeter^2/(8*PI^2*Inertia)
        16 - Entropy
        17 - Lambda-max (Max.child gray level - current gray level)
        18 - Gray level
and decision is:
        0 - Min
        1 - Direct
        2 - Max
        3 - Subtractive (default)
```
convert -colorspace GRAY scene1.row3.col3.ppm scene1.row3.col3.tiff