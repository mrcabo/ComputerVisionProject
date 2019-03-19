//
// Created by diego on 15/03/19.
//

#include "calculatedisp.h"
#include <stdio.h>
#include <limits.h>
#include <math.h>

// TODO: maybe filter the tree before comparing both?
DecisionStruct Decisions[NUMDECISIONS] = {
                {"Min", MaxTreeFilterMin},
                {"Direct", MaxTreeFilterDirect},
                {"Max", MaxTreeFilterMax},
                {"Subtractive", MaxTreeFilterSubtractive},
        };
AttribStruct Attribs[NUMATTR] = {
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
struct SimilarNodes {
    MaxNode *NodesL;
    MaxNode *NodesR;
    int IdxSimilarNodes;
};

bool is_in_range(double ref_val, double new_val, double margin) {
    return ((fabs(ref_val-new_val) <= margin) ? true : false);
}

// TODO: keep thinking what the return value should be.. Probably
int calc_disp(const MaxTree *mt_l, const MaxTree *mt_r, const ImageGray *img_l, const ImageGray *img_r, ImageGray *out,
              double (*attribute)(void *)) {
    MaxNode *node_l, *node_r;
    ulong imgsize = img_l->Height*img_l->Width;
    ulong nrows = img_l->Height, ncols = img_l->Width;
    int num_nodes = 0;
    for (int l = 0; l < NUMLEVELS; ++l) {
        num_nodes += mt_l->NumNodesAtLevel[l];
    }

    ImageGrayInit(out, (ubyte) 0); // set image to 0 (all black)

    // Not sure about this because it's going pixel by pixel. Gotta try to do it node by node.
    for (ulong r = 0; r < nrows; ++r) {
        for (ulong col_l = 0; col_l < ncols; ++col_l) {
            ulong pix_l = r*ncols + col_l;
            ulong idx = mt_l->NumPixelsBelowLevel[img_l->Pixmap[pix_l]] + mt_l->Status[pix_l];
            node_l = &(mt_l->Nodes[idx]);
            InertiaData *inertiadata_l = node_l->Attribute;
            double value_l = (*attribute)(node_l->Attribute);
            double sumX_l = inertiadata_l->SumX;

            // Find the equivalent node along the current row
            for (ulong col_r = col_l; col_r < ULONG_MAX; --col_r) { // swipe epipolar line to the left only
                ulong pix_r = r*ncols + col_r;
                ulong idx = mt_r->NumPixelsBelowLevel[img_r->Pixmap[pix_r]] + mt_r->Status[pix_r];
                node_r = &(mt_r->Nodes[idx]);
                InertiaData *inertiadata_r = node_r->Attribute;
                double value_r = (*attribute)(node_r->Attribute);
                double sumX_r = inertiadata_r->SumX;
                double margin = 0.001;

                if (is_in_range(value_l, value_r, margin)) {
                    out->Pixmap[pix_l] = (ubyte) (sumX_l-sumX_r);
                    break;
                }
            }
        }
    }

//    // Problem with this is I would have to search the whole tree...
//    for (int l = 0; l < NUMLEVELS; ++l) {
//        for (ulong i = 0; i < mt_l->NumNodesAtLevel[l]; ++i) {
//            ulong idx = mt_l->NumPixelsBelowLevel[l] + i;
//            node_l = &(mt_l->Nodes[idx]);
//            ulong parent = node_l->Parent; // why..?
//
//
////            if (idx!=parent){
////                if ((*attribute)(node->Attribute) < lambda)  node->NewLevel = mt_l->Nodes[parent].NewLevel;
////                else  node->NewLevel = node->Level;
////            }
//        }
//    }
return (0);
}

ImageGray *create_disp_img(ImageGray *img_l, ImageGray *img_r, ImageGray *template_l, ImageGray *template_r, int attrib) {
    ImageGray *out;
    MaxTree *mt_l, *mt_r;
    mt_l = MaxTreeCreate(img_l, template_l, Attribs[attrib].NewAuxData, Attribs[attrib].AddToAuxData, Attribs[attrib].MergeAuxData, Attribs[attrib].DeleteAuxData);
    if (mt_l==NULL) {
        fprintf(stderr, "Can't create left Max-tree\n");
        return(NULL);
    }
    mt_r = MaxTreeCreate(img_r, template_r, Attribs[attrib].NewAuxData, Attribs[attrib].AddToAuxData, Attribs[attrib].MergeAuxData, Attribs[attrib].DeleteAuxData);
    if (mt_r==NULL) {
        fprintf(stderr, "Can't create right Max-tree\n");
        MaxTreeDelete(mt_l);
        return(NULL);
    }

//    Decisions[2].Filter(mt_l, img_l, template_l, out, Attribs[attrib].Attribute, 2); // to check how they filer and use the nodes
    out = ImageGrayCreate(img_l->Width, img_l->Height);
    if (out==NULL) {
        fprintf(stderr, "Can't create output image\n");
        MaxTreeDelete(mt_l);
        MaxTreeDelete(mt_r);
        return(NULL);
    }

    int st = calc_disp(mt_l, mt_r, img_l, img_r, out, Attribs[attrib].Attribute);
    if (st!=0) {
        fprintf(stderr, "Error calculating disparity\n");
        MaxTreeDelete(mt_l);
        MaxTreeDelete(mt_r);
        return(NULL);
    }
    MaxTreeDelete(mt_l);
    MaxTreeDelete(mt_r);

    return (out);
}

