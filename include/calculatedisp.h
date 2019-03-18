//
// Created by diego on 15/03/19.
//

#ifndef COMPUTERVISIONPROJECT_CALCULATEDISP_H
#define COMPUTERVISIONPROJECT_CALCULATEDISP_H

#include "maxtree3b.h"

ImageGray *create_disp_img(ImageGray *img_l, ImageGray *img_r, ImageGray *template_l, ImageGray *template_r, int attrib);

#endif //COMPUTERVISIONPROJECT_CALCULATEDISP_H
