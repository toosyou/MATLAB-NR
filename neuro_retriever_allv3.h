#ifndef NEURO_RETRIEVER_ALLV3_H
#define NEURO_RETRIEVER_ALLV3_H

#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <dirent.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <math.h>
#include <cstdio>

using namespace std;


template <class T>
T vector_max(vector<T> &input){
    T max = -9999;
    for(int i=0;i<input.size();++i){
        if(max < input[i])
            max = input[i];
    }
    return max;
}

template <class T>
vector<int> vector_find(vector<T> &input,T target){
    vector<int> result;
    for(int i=0;i<input.size();++i){
        if(input[i] == target)
            result.push_back(i);
    }
    return result;
}

struct MAJOR{
    float min_l;
    float len_fac;
    float max_lev;
    float num_fac;
    float max_intensity;
    float max_threshold;

    MAJOR(){
        this->min_l = 0.0;
        this->len_fac = 0.0;
        this->max_lev = 0.0;
        this->num_fac = 0.0;
        this->max_intensity = 0.0;
        this->max_threshold = 0.0;
    }
};

struct PARA{
    float level;
    float number;
    float length;
    int cut_number;
    PARA(){
        this->level = 0.0;
        this->number = 0.0;
        this->length = 0.0;
        this->cut_number = 0;
    }
};

struct TREE_DATA{
    float Threshold;
    float TotalVoxel;
    float BranchSizeThreshold;
    float MaxCode;
    float MaxBoundaryField;
    float MaxBranchLevel;
    float TotalBranchNum;
    float UnTracedVoxel;
    float RatioUnTracedVoxel;
    float BranchPoints;
    float EndPoints;
    float LoopPoints;
    float BulgePoints;
    float BranchTotalVolV;
    float BranchAvgCrossSecV;
    float BranchTotalLenV;
    float BranchTotalEndToEndV;
    float BranchTotalVolR;
    float BranchAvgCrossSecR;
    float BranchTotalLenR;
    float BranchTotalEndToEndR;
    float BulgeTotalVolV;
    float BulgeTotalVolR;
    float SWC_Voxel;
    float SWC_Overlap;
    float SWC_TargetOnly;
    float SWC_SWCOnly;

    vector<float> VoxelSize;
    vector<float> Xrange;
    vector<float> Yrange;
    vector<float> Zrange;
    vector<float> MinBoundBox;
    vector<float> MaxMinAvgIntensity;

    TREE_DATA(){
        Threshold = -1.0;
        TotalVoxel = -1.0;
        BranchSizeThreshold = -1.0;
        MaxCode = -1.0;
        MaxBoundaryField = -1.0;
        MaxBranchLevel = -1.0;
        TotalBranchNum = -1.0;
        UnTracedVoxel = -1.0;
        RatioUnTracedVoxel = -1.0;
        BranchPoints = -1.0;
        EndPoints = -1.0;
        LoopPoints = -1.0;
        BulgePoints = -1.0;
        BranchTotalVolV = -1.0;
        BranchAvgCrossSecV = -1.0;
        BranchTotalLenV = -1.0;
        BranchTotalEndToEndV = -1.0;
        BranchTotalVolR = -1.0;
        BranchAvgCrossSecR = -1.0;
        BranchTotalLenR = -1.0;
        BranchTotalEndToEndR = -1.0;
        BulgeTotalVolV = -1.0;
        BulgeTotalVolR = -1.0;
        SWC_Voxel = -1.0;
        SWC_Overlap = -1.0;
        SWC_TargetOnly = -1.0;
        SWC_SWCOnly = -1.0;

        VoxelSize.resize(3,-1.0);
        Xrange.resize(2,-1.0);
        Yrange.resize(2,-1.0);
        Zrange.resize(2,-1.0);
        MinBoundBox.resize(3,-1.0);
        MaxMinAvgIntensity.resize(3,-1.0);
    }
};

struct BRANCH_DATA{
    string fname;
    float CP;
    float VolV;
    float AveAreaV;
    float LenV;
    float EndToEndV;
    float VolR;
    float AveAreaR;
    float LenR;
    float EndToEndR;
    string Type;
    unsigned long long int BranchNum; // use c99 or later
    unsigned long long int Level;
    int ParentID;

    BRANCH_DATA(){
        CP = -1.0;
        VolV = -1.0;
        AveAreaV = -1.0;
        LenV = -1.0;
        EndToEndV = -1.0;
        VolR = -1.0;
        AveAreaR = -1.0;
        LenR = -1.0;
        EndToEndR = -1.0;
        BranchNum = 0ll;
        Level = 0ll;
        ParentID = -1;
    }
};

struct POINTS{
    vector<vector<int> > CPX;
    vector<vector<int> > CPY;
    vector<vector<int> > CPZ;
    vector<vector<int> > BP;
    vector<vector<int> > LP;
    vector<vector<int> > EP;

    int NumBP;
    int NumLP;
    int NumEP;

    POINTS(){
        this->BP.resize(3);
        this->LP.resize(3);
        this->EP.resize(3);
        this->NumBP = 0;
        this->NumLP = 0;
        this->NumEP = 0;
    }
};

struct sLTH{
    float threshold;

    string name;
    TREE_DATA TreeData;
    vector<BRANCH_DATA> BranchData;
    POINTS Points;
    vector<int> BID;

    sLTH(){
        this->threshold = 0.0;
    }
};

struct OFFSPRING{
    vector<int> level;
    vector<int> num;

    OFFSPRING(){

    }
};

struct LIFE_HISTOGRAM{
    vector<int> bin;

    LIFE_HISTOGRAM(){

    }
};

bool sLTH_cmp(sLTH a, sLTH b);

void getdir(string dir, vector<string> &files);

vector<int> c_strfind(string source,string target);

void neuro_retriever_allv3(
        string Name_Path,string Results_Path,string Path_AM,
        string id_str,MAJOR major,PARA para,
        int life_start,int life_end,int life_step);

void read_alltrees_v2(vector<sLTH> &LTH);

void readtree_single(string foldername,sLTH &single_LTH);

void read_BID(string foldername,vector<int>& branch_id);

void voxel_life_mask_v6(vector<sLTH> &LTH,MAJOR major,PARA para,
                        string lsm_name,string fpath,
                        int life_index_min,int life_index_max,int life_index_step,
                        LIFE_HISTOGRAM &life_histogram);

void tree_offspring(vector<int> &node_parent,
                    vector<int> &num_offspring,
                    vector<int> &level_offspring,
                    vector<int> &offsprings);

void parent2level(vector<int>& parent,vector<int>& node_level);

float quantile0(vector<int> &Z, float P);

vector<float> major_br(float th,MAJOR &major);

#endif // NEURO_RETRIEVER_ALLV3_H
