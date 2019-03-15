//
// Created by diego on 13/03/19.
//

#include "maxtree3b.h"
#include "calculatedisp.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


DecisionStruct Decisions[NUMDECISIONS] =
        {
                {"Min", MaxTreeFilterMin},
                {"Direct", MaxTreeFilterDirect},
                {"Max", MaxTreeFilterMax},
                {"Subtractive", MaxTreeFilterSubtractive},
        };
AttribStruct Attribs[NUMATTR] =
        {
                {"Area", NewAreaData, DeleteAreaData, AddToAreaData, MergeAreaData, AreaAttribute},
                {"Area of min. enclosing rectangle", NewEnclRectData, DeleteEnclRectData, AddToEnclRectData, MergeEnclRectData, EnclRectAreaAttribute},
                {"Square of diagonal of min. enclosing rectangle", NewEnclRectData, DeleteEnclRectData, AddToEnclRectData, MergeEnclRectData, EnclRectDiagAttribute},
                {"Cityblock perimeter", NewPeriCBData, DeletePeriCBData, AddToPeriCBData, MergePeriCBData, PeriCBPerimeterAttribute},
                {"Cityblock complexity (Perimeter/Area)", NewPeriCBData, DeletePeriCBData, AddToPeriCBData, MergePeriCBData, PeriCBComplexityAttribute},
                {"Cityblock simplicity (Area/Perimeter)", NewPeriCBData, DeletePeriCBData, AddToPeriCBData, MergePeriCBData, PeriCBSimplicityAttribute},
                {"Cityblock compactness (Perimeter^2/(4*PI*Area))", NewPeriCBData, DeletePeriCBData, AddToPeriCBData, MergePeriCBData, PeriCBCompactnessAttribute},
                {"Large perimeter", NewPeriLargeData, DeletePeriLargeData, AddToPeriLargeData, MergePeriLargeData, PeriLargePerimeterAttribute},
                {"Large compactness (Perimeter^2/(4*PI*Area))", NewPeriLargeData, DeletePeriLargeData, AddToPeriLargeData, MergePeriLargeData, PeriLargeCompactnessAttribute},
                {"Small perimeter", NewPeriSmallData, DeletePeriSmallData, AddToPeriSmallData, MergePeriSmallData, PeriSmallPerimeterAttribute},
                {"Small compactness (Perimeter^2/(4*PI*Area))", NewPeriSmallData, DeletePeriSmallData, AddToPeriSmallData, MergePeriSmallData, PeriSmallCompactnessAttribute},
                {"Moment of Inertia", NewInertiaData, DeleteInertiaData, AddToInertiaData, MergeInertiaData, InertiaAttribute},
                {"Elongation: (Moment of Inertia) / (area)^2", NewInertiaData, DeleteInertiaData, AddToInertiaData, MergeInertiaData, InertiaDivA2Attribute},
                {"Mean X position", NewInertiaData, DeleteInertiaData, AddToInertiaData, MergeInertiaData, MeanXAttribute},
                {"Mean Y position", NewInertiaData, DeleteInertiaData, AddToInertiaData, MergeInertiaData, MeanYAttribute},
                {"Jaggedness: Area*Perimeter^2/(8*PI^2*Inertia)", NewJaggedData, DeleteJaggedData, AddToJaggedData, MergeJaggedData, JaggednessAttribute},
                {"Entropy", NewEntropyData, DeleteEntropyData, AddToEntropyData, MergeEntropyData, EntropyAttribute},
                {"Lambda-max (Max.child gray level - current gray level)", NewLambdamaxData, DeleteLambdamaxData, AddToLambdamaxData, MergeLambdamaxData, LambdamaxAttribute},
                {"Gray level", NewLevelData, DeleteLevelData, AddToLevelData, MergeLevelData, LevelAttribute}
        };

int filt_maxtree(int argc, char **argv)
{
    ImageGray *img, *template, *out;
    MaxTree *mt;
    char *imgfname, *templatefname = NULL, *outfname = "out.pgm";
    double lambda;
    int attrib, decision=3, r;

    if (argc<4)
    {
        printf("Usage: %s <input image> <attrib> <lambda> [decision] [output image] [template]\n", argv[0]);
        printf("Where attrib is:\n");
        for (attrib=0; attrib<NUMATTR; attrib++)
        {
            printf("\t%d - %s\n", attrib, Attribs[attrib].Name);
        }
        printf("and decision is:\n");
        for (r=0; r<NUMDECISIONS; r++)
        {
            printf("\t%d - %s", r, Decisions[r].Name);
            if (r==decision)  printf(" (default)");
            printf("\n");
        }
        exit(0);
    }
    imgfname = argv[1];
    attrib = atoi(argv[2]);
    lambda = atof(argv[3]);
    if (argc>=5)  decision = atoi(argv[4]);
    if (argc>=6)  outfname = argv[5];
    if (argc>=7)  templatefname = argv[6];
    img = ImagePGMRead(imgfname);
    if (img==NULL)
    {
        fprintf(stderr, "Can't read image '%s'\n", imgfname);
        return(-1);
    }
    template = GetTemplate(templatefname, img);
    if (template==NULL)
    {
        fprintf(stderr, "Can't create template\n");
        ImageGrayDelete(img);
        return(-1);
    }
    printf("Filtering image '%s' using attribute '%s' with lambda=%f\n", imgfname, Attribs[attrib].Name, lambda);
    printf("Decision rule: %s   Template: ", Decisions[decision].Name);
    if (templatefname==NULL)  printf("<not used>\n");
    else  printf("%s\n", templatefname);
    printf("Image: Width=%ld Height=%ld\n", img->Width, img->Height);
    out = ImageGrayCreate(img->Width, img->Height);
    if (out==NULL)
    {
        fprintf(stderr, "Can't create output image\n");
        ImageGrayDelete(template);
        ImageGrayDelete(img);
        return(-1);
    }
    mt = MaxTreeCreate(img, template, Attribs[attrib].NewAuxData, Attribs[attrib].AddToAuxData, Attribs[attrib].MergeAuxData, Attribs[attrib].DeleteAuxData);
    if (mt==NULL)
    {
        fprintf(stderr, "Can't create Max-tree\n");
        ImageGrayDelete(out);
        ImageGrayDelete(template);
        ImageGrayDelete(img);
        return(-1);
    }
    Decisions[decision].Filter(mt, img, template, out, Attribs[attrib].Attribute, lambda);
    MaxTreeDelete(mt);
    r = ImagePGMBinWrite(out, outfname);
    if (r)  fprintf(stderr, "Error writing image '%s'\n", outfname);
    else  printf("Filtered image written to '%s'\n", outfname);
    ImageGrayDelete(out);
    ImageGrayDelete(template);
    ImageGrayDelete(img);
    return(0);

}

int main(int argc, char *argv[])
{
//    filt_maxtree(argc, argv); // this would call the original maxtree3b.c functionality.

    ImageGray *img_l, *img_r, *out;
    MaxTree *mt;
    char *img_l_fname = "src-images/left-img.pgm", *img_r_fname = "src-images/right-img.pgm", *outfname = "disp.pgm";
    /*pointer to some read-only memory containing the string-literal.
     * will be slightly faster because the string does not have to be copied*/
    double lambda;
    int attrib, decision=3, r;
    img_l = ImagePGMRead(img_l_fname);
    img_r = ImagePGMRead(img_r_fname);
    if (img_l==NULL || img_r==NULL)
    {
        fprintf(stderr, "Can't read src images '%s'\n");
        return(-1);
    }

    out = ImageGrayCreate(img_l->Width, img_l->Height);
    *out->Pixmap = *img_l->Pixmap;
//    MaxTreeDelete(mt);
    r = ImagePGMBinWrite(out, outfname);
    ImageGrayDelete(out);
    ImageGrayDelete(img_l);
    ImageGrayDelete(img_r);

    return (0);
} /* main */
