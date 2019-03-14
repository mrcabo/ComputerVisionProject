//
// Created by diego on 14/03/19.
//

#ifndef COMPUTERVISIONPROJECT_MAXTREE3B_H
#define COMPUTERVISIONPROJECT_MAXTREE3B_H

#define NUMDECISIONS 4
#define NUMATTR 19

typedef unsigned char ubyte;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef struct ImageGray ImageGray;
struct ImageGray
{
    ulong Width;
    ulong Height;
    ubyte *Pixmap;
};

typedef struct MaxNode MaxNode;
struct MaxNode
{
    ulong Parent;
    ulong Area;
    void *Attribute;
    ubyte Level;
    ubyte NewLevel;  /* gray level after filtering */
};

typedef struct MaxTree MaxTree;
struct MaxTree
{
    long *Status;
    ulong *NumPixelsBelowLevel;
    ulong *NumNodesAtLevel; /* Number of nodes C^k_h at level h */
    MaxNode *Nodes;
    void *(*NewAuxData)(ulong, ulong, int, ulong *, ImageGray *);
    void (*AddToAuxData)(void *, ulong, ulong, int, ulong *, ImageGray *);
    void (*MergeAuxData)(void *, void *);
    void (*DeleteAuxData)(void *);
};

typedef struct DecisionStruct DecisionStruct;
struct DecisionStruct
{
    char *Name;
    void (*Filter)(MaxTree *, ImageGray *, ImageGray *, ImageGray *, double (*attribute)(void *), double);
};
typedef struct AttribStruct AttribStruct;
struct AttribStruct
{
    char *Name;
    void *(*NewAuxData)(ulong, ulong, int, ulong *, ImageGray *);
    void (*DeleteAuxData)(void *);
    void (*AddToAuxData)(void *, ulong, ulong, int, ulong *, ImageGray *);
    void (*MergeAuxData)(void *, void *);
    double (*Attribute)(void *);
};

ImageGray *ImagePGMRead(char *fname);
ImageGray *GetTemplate(char *templatefname, ImageGray *img);
void ImageGrayDelete(ImageGray *img);
ImageGray *ImageGrayCreate(ulong width, ulong height);
MaxTree *MaxTreeCreate(ImageGray *img, ImageGray *template,
                       void *(*newauxdata)(ulong, ulong, int, ulong *, ImageGray *),
                       void (*addtoauxdata)(void *, ulong, ulong, int, ulong *, ImageGray *),
                       void (*mergeauxdata)(void *, void *),
                       void (*deleteauxdata)(void *));

void MaxTreeDelete(MaxTree *mt);
int ImagePGMBinWrite(ImageGray *img, char *fname);

void MaxTreeFilterMin(MaxTree *mt, ImageGray *img, ImageGray *template,
                      ImageGray *out, double (*attribute)(void *),
                      double lambda);

void MaxTreeFilterDirect(MaxTree *mt, ImageGray *img, ImageGray *template,
                         ImageGray *out, double (*attribute)(void *),
                         double lambda);

void MaxTreeFilterMax(MaxTree *mt, ImageGray *img, ImageGray *template,
                      ImageGray *out, double (*attribute)(void *),
                      double lambda);

void MaxTreeFilterSubtractive(MaxTree *mt, ImageGray *img, ImageGray *template,
                              ImageGray *out, double (*attribute)(void *),
                              double lambda);

void *NewAreaData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteAreaData(void *areaattr);
void AddToAreaData(void *areaattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeAreaData(void *areaattr, void *childattr);
double AreaAttribute(void *areaattr);
void *NewEnclRectData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteEnclRectData(void *rectattr);
void AddToEnclRectData(void *rectattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeEnclRectData(void *rectattr, void *childattr);
double EnclRectAreaAttribute(void *rectattr);
double EnclRectDiagAttribute(void *rectattr);
void *NewPeriCBData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeletePeriCBData(void *periattr);
void AddToPeriCBData(void *periattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergePeriCBData(void *periattr, void *childattr);
double PeriCBPerimeterAttribute(void *periattr);
double PeriCBComplexityAttribute(void *periattr);
double PeriCBSimplicityAttribute(void *periattr);
double PeriCBCompactnessAttribute(void *periattr);
void *NewPeriLargeData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeletePeriLargeData(void *periattr);
void AddToPeriLargeData(void *periattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergePeriLargeData(void *periattr, void *childattr);
double PeriLargePerimeterAttribute(void *periattr);
double PeriLargeCompactnessAttribute(void *periattr);
void *NewPeriSmallData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeletePeriSmallData(void *periattr);
void AddToPeriSmallData(void *periattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergePeriSmallData(void *periattr, void *childattr);
double PeriSmallPerimeterAttribute(void *periattr);
double PeriSmallCompactnessAttribute(void *periattr);
void *NewInertiaData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteInertiaData(void *inertiaattr);
void AddToInertiaData(void *inertiaattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeInertiaData(void *inertiaattr, void *childattr);
double InertiaAttribute(void *inertiaattr);
double InertiaDivA2Attribute(void *inertiaattr);
double MeanXAttribute(void *inertiaattr);
double MeanYAttribute(void *inertiaattr);
void *NewJaggedData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteJaggedData(void *jaggedattr);
void AddToJaggedData(void *jaggedattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeJaggedData(void *jaggedattr, void *childattr);
double JaggednessAttribute(void *jaggedattr);
void *NewEntropyData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteEntropyData(void *entropyattr);
void AddToEntropyData(void *entropyattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeEntropyData(void *entropyattr, void *childattr);
double EntropyAttribute(void *entropyattr);
void *NewLambdamaxData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteLambdamaxData(void *lambdaattr);
void AddToLambdamaxData(void *lambdaattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeLambdamaxData(void *lambdaattr, void *childattr);
double LambdamaxAttribute(void *lambdaattr);
void *NewLevelData(ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void DeleteLevelData(void *levelattr);
void AddToLevelData(void *levelattr, ulong x, ulong y, int numneighbors, ulong *neighbors, ImageGray *img);
void MergeLevelData(void *levelattr, void *childattr);
double LevelAttribute(void *levelattr);

#endif //COMPUTERVISIONPROJECT_MAXTREE3B_H
