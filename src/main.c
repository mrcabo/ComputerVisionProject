//
// Created by diego on 13/03/19.
//

#include "maxtree3b.h"
#include "calculatedisp.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
//int filt_maxtree(int argc, char **argv) {
//    ImageGray *img, *template, *out;
//    MaxTree *mt;
//    char *imgfname, *templatefname = NULL, *outfname = "out.pgm";
//    double lambda;
//    int attrib, decision=3, r;
//
//    if (argc<4)
//    {
//        printf("Usage: %s <input image> <attrib> <lambda> [decision] [output image] [template]\n", argv[0]);
//        printf("Where attrib is:\n");
//        for (attrib=0; attrib<NUMATTR; attrib++)
//        {
//            printf("\t%d - %s\n", attrib, Attribs[attrib].Name);
//        }
//        printf("and decision is:\n");
//        for (r=0; r<NUMDECISIONS; r++)
//        {
//            printf("\t%d - %s", r, Decisions[r].Name);
//            if (r==decision)  printf(" (default)");
//            printf("\n");
//        }
//        exit(0);
//    }
//    imgfname = argv[1];
//    attrib = atoi(argv[2]);
//    lambda = atof(argv[3]);
//    if (argc>=5)  decision = atoi(argv[4]);
//    if (argc>=6)  outfname = argv[5];
//    if (argc>=7)  templatefname = argv[6];
//    img = ImagePGMRead(imgfname);
//    if (img==NULL)
//    {
//        fprintf(stderr, "Can't read image '%s'\n", imgfname);
//        return(-1);
//    }
//    template = GetTemplate(templatefname, img);
//    if (template==NULL)
//    {
//        fprintf(stderr, "Can't create template\n");
//        ImageGrayDelete(img);
//        return(-1);
//    }
//    printf("Filtering image '%s' using attribute '%s' with lambda=%f\n", imgfname, Attribs[attrib].Name, lambda);
//    printf("Decision rule: %s   Template: ", Decisions[decision].Name);
//    if (templatefname==NULL)  printf("<not used>\n");
//    else  printf("%s\n", templatefname);
//    printf("Image: Width=%ld Height=%ld\n", img->Width, img->Height);
//    out = ImageGrayCreate(img->Width, img->Height);
//    if (out==NULL)
//    {
//        fprintf(stderr, "Can't create output image\n");
//        ImageGrayDelete(template);
//        ImageGrayDelete(img);
//        return(-1);
//    }
//    mt = MaxTreeCreate(img, template, Attribs[attrib].NewAuxData, Attribs[attrib].AddToAuxData, Attribs[attrib].MergeAuxData, Attribs[attrib].DeleteAuxData);
//    if (mt==NULL)
//    {
//        fprintf(stderr, "Can't create Max-tree\n");
//        ImageGrayDelete(out);
//        ImageGrayDelete(template);
//        ImageGrayDelete(img);
//        return(-1);
//    }
//    Decisions[decision].Filter(mt, img, template, out, Attribs[attrib].Attribute, lambda);
//    MaxTreeDelete(mt);
//    r = ImagePGMBinWrite(out, outfname);
//    if (r)  fprintf(stderr, "Error writing image '%s'\n", outfname);
//    else  printf("Filtered image written to '%s'\n", outfname);
//    ImageGrayDelete(out);
//    ImageGrayDelete(template);
//    ImageGrayDelete(img);
//    return(0);
//}

int main(int argc, char *argv[]) {
//    filt_maxtree(argc, argv); // this would call the original maxtree3b.c functionality.

    ImageGray *img_l, *img_r, *template_l, *template_r, *out;
    char *img_l_fname = "src-images/left-img.pgm", *img_r_fname = "src-images/right-img.pgm", *outfname = "disp.pgm";
    char *templatefname = NULL; // Don't know what this is for. NULL both for now...
    /*pointer to some read-only memory containing the string-literal.
     * will be slightly faster because the string does not have to be copied*/
//    double lambda;
    int attrib;// , decision=3; if we decide to filter tree

    attrib = 12;// atoi(argv[2]);
//    lambda = 2;// atof(argv[3]);

    img_l = ImagePGMRead(img_l_fname);
    if (img_l==NULL) {
        fprintf(stderr, "Can't read src images '%s'\n", img_l_fname);
        return(-1);
    }
    img_r = ImagePGMRead(img_r_fname);
    if (img_r==NULL) {
        fprintf(stderr, "Can't read src images '%s'\n", img_r_fname);
        ImageGrayDelete(img_l);
        return(-1);
    }

    if (img_l->Width!=img_r->Width || img_l->Height!=img_r->Height) {
        fprintf(stderr, "Left and right images are not the same size\n");
        ImageGrayDelete(img_l);
        ImageGrayDelete(img_r);
        return(-1);
    }
    template_l = GetTemplate(templatefname, img_l);
    template_r = GetTemplate(templatefname, img_r);
    if (template_l==NULL || template_r==NULL) {
        fprintf(stderr, "Can't create templates\n");
        ImageGrayDelete(img_l);
        ImageGrayDelete(img_r);
        return(-1);
    }

    out = create_disp_img(img_l, img_r, template_l, template_r, attrib);
    if (out==NULL) {
        fprintf(stderr, "Can't create output image\n");
        ImageGrayDelete(img_l);
        ImageGrayDelete(img_r);
        ImageGrayDelete(template_l);
        ImageGrayDelete(template_r);
        return(-1);
    }

//    memcpy(out->Pixmap, img_l->Pixmap, sizeof(ubyte)*img_l->Width*img_l->Height);

    int r = ImagePGMBinWrite(out, outfname);
    if (r) {
        fprintf(stderr, "Error writing image '%s'\n", outfname);
    }
    else {
        printf("Disparity image written to '%s'\n", outfname);
    }

    // free memory
    ImageGrayDelete(out);
    ImageGrayDelete(img_l);
    ImageGrayDelete(img_r);
    ImageGrayDelete(template_l);
    ImageGrayDelete(template_r);

    return (0);
} /* main */
