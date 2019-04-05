//
// Created by diego on 15/03/19.
//

#include "calculatedisp.h"
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

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

typedef struct DispNodeAux DispNodeAux;
struct DispNodeAux {  // Same indexation as with MaxNodes
    double *attr_diff;  // difference between attribute values of node_l and node_r
    double *disparity;  // Disparity for specific node

    // TODO: Couldn't think of a better way to know if attr_diff is zero because of calloc or because it's found a perfect match
    bool *is_set;
};

DispNodeAux *DispNodeAuxCreate(ulong imgsize)
{
    DispNodeAux *aux;
    /* Allocate structures */
    aux = malloc(sizeof(DispNodeAux));
    if (aux==NULL)
        return(NULL);
    aux->attr_diff = calloc((size_t)imgsize, sizeof(double));
    if (aux->attr_diff==NULL) {
        free(aux);
        return(NULL);
    }
    aux->disparity = calloc((size_t)imgsize, sizeof(double));
    if (aux->disparity==NULL) {
        free(aux->attr_diff);
        free(aux);
        return(NULL);
    }
    aux->is_set = calloc((size_t)imgsize, sizeof(bool));
    if (aux->is_set==NULL) {
        free(aux->attr_diff);
        free(aux->disparity);
        free(aux);
        return(NULL);
    }
    return(aux);
} /* DispNodeAuxCreate */

void DispNodeAuxDelete(DispNodeAux *aux)
{
    free(aux->attr_diff);
    free(aux->disparity);
    free(aux->is_set);
    free(aux);
} /* DispNodeAuxDelete */

bool is_in_range(double ref_val, double new_val, double margin) {
    return ((fabs(ref_val-new_val) <= margin) ? true : false);
}

// TODO: keep thinking what the return value should be.. Probably return pointer to out
int calc_disp(const MaxTree *mt_l, const MaxTree *mt_r, const ImageGray *img_l, const ImageGray *img_r, ImageGray *out,
              double (*attribute)(void *)) {
    MaxNode *node_l, *node_r;
    DispNodeAux *disp_aux;
    ulong imgsize = img_l->Height*img_l->Width;
    ulong nrows = img_l->Height, ncols = img_l->Width;
    int num_nodes = 0;
    for (int l = 0; l < NUMLEVELS; ++l) {
        num_nodes += mt_l->NumNodesAtLevel[l];
    }

    // TODO: maybe not necessary to reset image...
    ImageGrayInit(out, (ubyte) 0); // set image to 0 (all black)

    disp_aux = DispNodeAuxCreate(imgsize);
    if (disp_aux==NULL) {
        return (-1);
    }
    for (ulong r = 0; r < nrows; ++r) {
        for (ulong col_l = 0; col_l < ncols; ++col_l) {
            ulong pix_l = r*ncols + col_l;
            ulong idx_l = mt_l->NumPixelsBelowLevel[img_l->Pixmap[pix_l]] + mt_l->Status[pix_l];
            node_l = &(mt_l->Nodes[idx_l]);
            InertiaData *inertiadata_l = node_l->Attribute;
            double value_l = (*attribute)(node_l->Attribute);

            // Find the equivalent node along the current row
            for (ulong col_r = col_l; col_r < ULONG_MAX; --col_r) { // swipe epipolar line to the left only
                ulong pix_r = r*ncols + col_r;
                ulong idx_r = mt_r->NumPixelsBelowLevel[img_r->Pixmap[pix_r]] + mt_r->Status[pix_r];
                node_r = &(mt_r->Nodes[idx_r]);
                InertiaData *inertiadata_r = node_r->Attribute;
                double value_r = (*attribute)(node_r->Attribute);

                double diff_value = fabs(value_l-value_r);
                if (!disp_aux->is_set[idx_l] || (diff_value < disp_aux->attr_diff[idx_l])) {
                    disp_aux->is_set[idx_l] = true;
                    disp_aux->attr_diff[idx_l] = diff_value;
                    double disparity = (inertiadata_l->SumX / inertiadata_l->Area) - (inertiadata_r->SumX / inertiadata_r->Area);
                    // TODO: Check if this makes worse/better results (looks better tho..)
//                    if (disparity < 0)
//                        disparity = col_l - col_r;
//                    disp_aux->disparity[idx_l] = disparity;

                    // disparities should be positive. If it found a wrong match to the right, set it to 0..
                    disp_aux->disparity[idx_l] = (disparity > 0) ? disparity : 0;
                }
            }
        }
    }
    for (ulong i = 0; i<imgsize; ++i) {
        ulong idx_l = mt_l->NumPixelsBelowLevel[img_l->Pixmap[i]] + mt_l->Status[i];
        out->Pixmap[i] = (ubyte) disp_aux->disparity[idx_l];
    }

    DispNodeAuxDelete(disp_aux);
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

    out = ImageGrayCreate(img_l->Width, img_l->Height);
    if (out==NULL) {
        fprintf(stderr, "Can't create output image\n");
        MaxTreeDelete(mt_l);
        MaxTreeDelete(mt_r);
        return(NULL);
    }

//    Decisions[3].Filter(mt_l, img_l, template_l, out, Attribs[attrib].Attribute, 1.2); // to check how they filer and use the nodes

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

ImageGray *comp_ground_truth(ImageGray *disp, ImageGray *gt) {

    if ((disp->Height != gt->Height) || (disp->Width != gt->Width)) {
        fprintf(stderr, "Disparity image and ground truth are not the same size\n");
        return (NULL);
    }

    ImageGray *out;
    out = ImageGrayCreate(gt->Width, gt->Height);
    if (out==NULL) {
        fprintf(stderr, "Can't create output image\n");
        return(NULL);
    }
    ulong imgsize = gt->Height*gt->Width;
    for (ulong p = 0; p < imgsize; ++p) {
        out->Pixmap[p] = (ubyte) abs(((int) gt->Pixmap[p]) - ((int) disp->Pixmap[p]));
    }

    return (out);
}
