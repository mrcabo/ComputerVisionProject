//
// Created by diego on 15/03/19.
//

#include "calculatedisp.h"
#include <stdio.h>


// TODO: maybe filter the tree before comparing both?
//DecisionStruct Decisions[NUMDECISIONS] =
//        {
//                {"Min", MaxTreeFilterMin},
//                {"Direct", MaxTreeFilterDirect},
//                {"Max", MaxTreeFilterMax},
//                {"Subtractive", MaxTreeFilterSubtractive},
//        };
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

// TODO: is this struct necessary?
typedef struct SimilarNodes SimilarNodes;
struct SimilarNodes
{
    MaxNode *NodesL;
    MaxNode *NodesR;
    int IdxSimilarNodes;
};

// TODO: keep thinking what the return value should be.. Probably
void compare_trees(MaxTree *mt_l, MaxTree *mt_r) {

}

ImageGray *calc_disp(ImageGray *img_l, ImageGray *img_r, ImageGray *template_l, ImageGray *template_r, int attrib)
{
    ImageGray *out;
    MaxTree *mt_l, *mt_r;
    ulong imgsize = img_l->Height*img_l->Width;
    out = ImageGrayCreate(img_l->Width, img_l->Height);
    if (out==NULL)
    {
        fprintf(stderr, "Can't create output image\n");
        ImageGrayDelete(img_l);
        ImageGrayDelete(img_r);
        ImageGrayDelete(template_l);
        ImageGrayDelete(template_r);
        return(NULL);
    }
    mt_l = MaxTreeCreate(img_l, template_l, Attribs[attrib].NewAuxData, Attribs[attrib].AddToAuxData, Attribs[attrib].MergeAuxData, Attribs[attrib].DeleteAuxData);
    if (mt_l==NULL)
    {
        fprintf(stderr, "Can't create Max-tree\n");
        ImageGrayDelete(out);
        ImageGrayDelete(img_l);
        ImageGrayDelete(img_r);
        ImageGrayDelete(template_l);
        ImageGrayDelete(template_r);
        return(NULL);
    }
    mt_r = MaxTreeCreate(img_r, template_r, Attribs[attrib].NewAuxData, Attribs[attrib].AddToAuxData, Attribs[attrib].MergeAuxData, Attribs[attrib].DeleteAuxData);
    if (mt_r==NULL)
    {
        fprintf(stderr, "Can't create Max-tree\n");
        ImageGrayDelete(out);
        ImageGrayDelete(img_l);
        ImageGrayDelete(img_r);
        ImageGrayDelete(template_l);
        ImageGrayDelete(template_r);
        return(NULL);
    }

    int del = compare_trees(mt_l, mt_r);

    MaxTreeDelete(mt_l);
    MaxTreeDelete(mt_r);
    return (out);
}

