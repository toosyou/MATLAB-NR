#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>

#include "neuro_retriever_allv3.h"



#if defined(_WIN32) || defined(_WIN64)
    #define RM_F "del/F "
    #define RD_SQ "rd/S/Q "
    #define DECOMPRESSING_GZ "winrar x "
    #define DECOMPRESSING_TAR "winrar x "
#elif defined(__unix__)
    #define RM_F "rm -f "
    #define RD_SQ "rm -rf "
    #define DECOMPRESSING_GZ "gzip -d -k "
    #define DECOMPRESSING_TAR "tar -xzf "
#endif

using namespace std;

int main()
{
    MAJOR major;
    PARA para;
    PARA para0;
    string Main_Path;
    string AM_Path;
    string Results_Path;
    int mask_min = 0;
    int mask_max = 0;
    int mask_step = 0;

    string input_buffer;

    //input X129 parameter.txt
    fstream fid("X129_Parameters.txt",fstream::in);
    if(!fid.is_open()){
        cerr << "***** ERROR : X129_Parameters.txt connot open *****" <<endl;
        return -1;
    }

    while(getline(fid,input_buffer)){//get one line to buffer
        string dontcare; // seriously, dont care this
        istringstream iss(input_buffer);

        if(input_buffer[0] == '#')//comment
            continue;

        if(input_buffer.find("Minimal length = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> major.min_l;
        else if(input_buffer.find("Length factor = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> major.len_fac;
        else if(input_buffer.find("Maximal level = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> major.max_lev;
        else if(input_buffer.find("Number factor = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> major.num_fac;
        else if(input_buffer.find("Power Level = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> para.level;
        else if(input_buffer.find("Power Numer = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> para.number;
        else if(input_buffer.find("Power Length = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> para.length;
        else if(input_buffer.find("Main Path = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> Main_Path;
        else if(input_buffer.find("Results Path = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> Results_Path;
        else if(input_buffer.find("AM Path = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> AM_Path;
        else if(input_buffer.find("Mask Min = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> mask_min;
        else if(input_buffer.find("Mask Max = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> mask_max;
        else if(input_buffer.find("Mask Step = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> mask_step;
        else if(input_buffer.find("Number of FAST Cut = ") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> para.cut_number;
    }
    cout << "Number of FAST Cut = " << para.cut_number <<endl;
    fid.close();

    para0 = para;

    //cout << Main_Path << " " << Results_Path << " " << AM_Path << endl;

    //input X129list00
    fid.open("X129list00",fstream::in);
    if(!fid.is_open()){
        cerr << "***** ERROR : X129list00 cannot open *****" <<endl;
        return -1;
    }
    while(getline(fid,input_buffer)){
        string tline_gfp = input_buffer;
        string sys_comm_rar;
        vector<int> num_underline ;

        cout << "Doing " << input_buffer << endl;

        num_underline = c_strfind(input_buffer,string("_"));
        if(num_underline.size() > 1){//remove words between two '_'s
            int size_to_removed = num_underline[1] - num_underline[0] ;
            input_buffer.replace(num_underline[0],size_to_removed,"");
            cout << "Source Modified = " << input_buffer <<endl;
        }
        cout << input_buffer <<endl;
        //extract .am.gz and .tar.gz
#if defined(_WIN32) || defined(_WIN64)
        sys_comm_rar = string(DECOMPRESSING_GZ) + Main_Path + string("/") + AM_Path + input_buffer + string(".am.gz ") + Main_Path + string("/") + AM_Path ;
        for(int i=0;i<sys_comm_rar.size();++i)
            if(sys_comm_rar[i] == '/')
                sys_comm_rar[i] = '\\';
        cout << sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
        sys_comm_rar = string(DECOMPRESSING_TAR) + Main_Path + string("/") + Results_Path + tline_gfp + string(".tar.gz ") + Main_Path + string("/") + Results_Path ;
        for(int i=0;i<sys_comm_rar.size();++i)
            if(sys_comm_rar[i] == '/')
                sys_comm_rar[i] = '\\';
        cout <<sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
#elif defined(__unix__)
        sys_comm_rar = string(DECOMPRESSING_GZ) + Main_Path + string("/") + AM_Path + input_buffer + string(".am.gz ");
        cout << sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
        sys_comm_rar = string(DECOMPRESSING_TAR) + Main_Path + string("/") + Results_Path + tline_gfp + string(".tar.gz -C ") + Main_Path + string("/") + Results_Path;
        cout <<sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
#endif

        for(int i=0;i<1;++i){
            para.level = para0.level + i*0.5;
            for(int j=0;j<1;++j){
                string Main_AM_Path = Main_Path + string("/") + AM_Path;
                para.length = para0.length + j*0.5;
                neuro_retriever_allv3(Main_Path, Results_Path, Main_AM_Path,
                                      tline_gfp, major, para,
                                      mask_min, mask_max, mask_step);
            }
        }
#if defined(_WIN32) || defined(_WIN64)
        string tmp_path = Main_Path + string("/") + AM_Path + input_buffer + string(".am");
        for(int i=0;i<tmp_path.size();++i){
            if(tmp_path[i] == '/')
                tmp_path[i] = '\\';
        }
        sys_comm_rar = string(RM_F) + tmp_path;
        cout << sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
        tmp_path = Main_Path + string("/") + Results_Path + tline_gfp;
        for(int i=0;i<tmp_path.size();++i){
            if(tmp_path[i] == '/')
                tmp_path[i] = '\\';
        }
        sys_comm_rar = string(RD_SQ) + tmp_path;
        cout << sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
#elif defined(__unix__)
        string tmp_path = Main_Path + string("/") + AM_Path + input_buffer + string(".am");
        sys_comm_rar = string(RM_F) + tmp_path;
        cout << sys_comm_rar <<endl;
        system(sys_comm_rar.c_str());
        tmp_path = Main_Path + string("/") + Results_Path + tline_gfp;
        sys_comm_rar = string(RD_SQ) + tmp_path;
        cout << sys_comm_rar <<endl;
        system(sys_comm_rar.c_str()); 
#endif
    }
    fid.close();
    return 0;
}

