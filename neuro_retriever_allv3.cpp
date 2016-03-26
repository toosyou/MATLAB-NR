#include "neuro_retriever_allv3.h"

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
    char* itoa(int input,char *buffer,int radix_notusing){
        sprintf(buffer,"%d",input);
        return buffer;
    }
#endif

bool sLTH_cmp(sLTH a, sLTH b){
    return a.threshold < b.threshold;
}

void getdir(string dir, vector<string> &files){
    DIR *dp;
    dirent *dirp;
    if((dp = opendir(dir.c_str()) ) == NULL){
        cerr << "***** ERROR : cannot getdir *****" <<endl;
		return;
    }
    while( (dirp = readdir(dp) ) != NULL ){
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return;
}

vector<int> c_strfind(string source,string target){
    vector<int> result;
    size_t index_found = source.find(target);
    while(index_found != string::npos){
        result.push_back(index_found);
        index_found = source.find(target,index_found+1);
    }
    return result;
}

int sys_cd(string path){
    int return_value = chdir(path.c_str());
    if(return_value == -1){
        cerr << "***** ERROR : cannot change working directory to " << path << " *****" <<endl;
        cerr << strerror(errno) <<endl;
    }
    //string cd_cmd = string("cd ") + path;
    //system(cd_cmd.c_str());
    return return_value;
}

void neuro_retriever_allv3(string Name_Path, string Results_Path, string Path_AM,
        string id_str, MAJOR major, PARA para,
        int life_start, int life_end, int life_step){

//    % Parameters for NeuroRetriever to calculate the life-index
//            % Minimal length for a branch
//            %major.min_l = 4;
//            % If branch length > major.min_l*major.len_fac, the score_length will be counted even the offspring generation/number are small
//            %major.len_fac = 1;
//            % Upper limit for the offspring generation threshold
//            %major.max_lev = 20;
//            % Offspring number threshold = (offspring generation threshold)*major.num_fac
//            %major.num_fac = 3;

//    % Function types: POWER function, input the exponent for para.level etc.
//            %para.level = 1;
//            %para.number = 1;
//            %para.length = 2;

    //tic
    clock_t c = clock();

    string Path_Data = Name_Path + string("/") + Results_Path;
    vector<string> List_Neuron;
    //vector<string> Name_Neuron;
    int n_file =0;

    sys_cd(Path_Data);

    //List_Neuron=dir;
    //get all
    getdir(Path_Data,List_Neuron);

    for(int i=0;i<List_Neuron.size();++i){
        if(List_Neuron[i] == id_str){
            sys_cd(List_Neuron[i]);

            //string mat_name;
            string lsm_name;
            string fpath;
            vector<sLTH> LTH;
            LIFE_HISTOGRAM life_hist;

            n_file ++;

            cout << endl << "Running " << n_file <<"-th neuron, name=" << List_Neuron[i] <<endl;

            read_alltrees_v2(LTH);

            if(LTH.size() < para.cut_number){
                cout << "Cut Number = " << LTH.size() << ", too few! Please check." <<endl;
                sys_cd(Path_Data);
                return;
            }
            else{
                int original_size_LTH = LTH.size();
                cout << "Original Cut Number = " << original_size_LTH << ", reduced to ";
                for(int j=0;j<( original_size_LTH - para.cut_number);++j){
                    LTH.pop_back();
                }
                cout << LTH.size() <<endl;
            }

            lsm_name = Path_AM + List_Neuron[i]+string(".am");
            fpath = Path_Data + List_Neuron[i];
            voxel_life_mask_v6(LTH,major,para,
                               lsm_name,fpath,
                               life_start,life_end,life_step,
                               life_hist);


            sys_cd(Path_Data);
        }
    }

    //toc
    c = clock() - c;
    cout << "neuro_retriever_allv3 takes : " << ((float)c) / (CLOCKS_PER_SEC) << " s" <<endl;
    return;
}

void read_alltrees_v2(vector<sLTH> &LTH){
//    % 本程式讀取所有目錄內的 FAST 不同 intesity threshold 下 output 之資料，注意 FAST
//    % 的輸出資料目錄名稱必須以「-Results」為結尾。所得之資料儲存於 LTH 結構陣列中，LTH(i) 儲存的是第 i 個 threshold
//    % 值對應之 FAST 結果所有的資訊
//    % This function also reads the branch ID files of the trees
//    % 第 39 行使用之 readbranch 函數說明：
//    %   舊版各 Branch 資料放在 \Branches 資料夾中，使用舊函數 readtree
//    %   新版所有 Branch 資料放在 *-Brances.txt 單一檔案中，使用新函數 readtree_single

//    % 讀取目錄內所有子目錄與檔案之名稱
    LTH.clear();

    vector<string> a;
    vector<float> th;
    int n_file = 0;

    getdir(string(".") , a);
    for(int i=0;i<a.size();++i){
//        % 當目錄或檔名包含「-Results」字串者始被計入，所以要注意 FAST output 檔名之設定
        if(a[i].find("-Results") != string::npos){
            n_file++;
        }
    }

    th.resize(n_file);

    for(int i=0,j=0;j<a.size();++j){
        string b = a[j];
        b[ b.find("-") ] = ' ';
        if(b.find("Results") != string::npos){
            istringstream iss(b);
            LTH.resize(i+1);
            sLTH &tmp_LTH = LTH[i];

            iss >> tmp_LTH.threshold;
            tmp_LTH.name = a[j];
            cout << i << "-th, name=" << tmp_LTH.name <<endl;
//            % 以 readtree 函式讀取各子目錄中之資料
//            % 舊版各 Branch 資料放在 \Branches 資料夾中，使用舊函數 readtree
//            % 新版所有 Branch 資料放在 *-Brances.txt 單一檔案中，使用新函數 readtree_single
//            %[LTH0(i).TreeData LTH0(i).BranchData LTH0(i).Points] = readtree(LTH0(i).name);

            readtree_single(a[j],tmp_LTH);
            cout << "Reading Branch ID..." ;
            read_BID(a[j],tmp_LTH.BID);
            cout << "\tdone" <<endl;

            i++;
        }
    }
//    % 由於讀取順序（由 a=dir; 指令決定）與實際 threshold 大小排序可能不一樣，故再根據實際所讀取得之 intensity
//    % threshold 值（th）重新排序一次
    sort(LTH.begin(),LTH.end(),sLTH_cmp);
    return;

}



void readtree_single(string foldername, sLTH &single_LTH){
//    % 讀取將 FAST 在特定 threshold 下所計算輸出之資料
//    % 所有 branch 的資料存在單一檔案
//    % Usage: [TreeData BranchNum TreeNum] = readtree(foldername)
//    % Input:
//    %   foldername (string): name of the folder with the data output by FASTA
//    % Outpuet:
//    %   TreeData (cell): Detailed data for the brances
//    %
//    % a=textscan(foldername,'%s','delimiter','-Result');
    POINTS &Points = single_LTH.Points;

    string a(foldername.c_str(),foldername.size()-8);
    string b = a + string("-BranchInfo.txt");
    string branch_file = a + string("-Branches.txt");
    string fname;
    string bname;
    int n_bp =0;
    int n_lp =0;
    int n_ep =0;

    fname = foldername + string("/") + b;
    bname = foldername + string("/") + branch_file;

    fstream fid(fname.c_str(),fstream::in);//info
    fstream bchid(bname.c_str(),fstream::in);

    if(!fid.is_open()){
        cerr << "***** ERROR : cannot open " << fname << " *****" <<endl;
        return;
    }
    if(!bchid.is_open()){
        cerr << "***** ERROR : cannot open " << bname << " *****" <<endl;
        return;
    }

    string input_buffer;

    while(getline(fid,input_buffer)){
        string dontcare;
        istringstream iss(input_buffer);

        if(input_buffer.find("Voxel intensity threshold =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Threshold;
        else if(input_buffer.find("Total voxel # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.TotalVoxel;
        else if(input_buffer.find("Voxel size X =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.VoxelSize[0];
        else if(input_buffer.find("Voxel size Y =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.VoxelSize[1];
        else if(input_buffer.find("Voxel size Z =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.VoxelSize[2];
        else if(input_buffer.find("Min. X =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Xrange[0];
        else if(input_buffer.find("Max. X =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Xrange[1];
        else if(input_buffer.find("Min. Y =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Yrange[0];
        else if(input_buffer.find("Max. Y =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Yrange[1];
        else if(input_buffer.find("Min. Z =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Zrange[0];
        else if(input_buffer.find("Max. Z =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.Zrange[1];
        else if(input_buffer.find("Bounding box min. X =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MinBoundBox[0];
        else if(input_buffer.find("Bounding box min. Y =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MinBoundBox[1];
        else if(input_buffer.find("Bounding box min. Z =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MinBoundBox[2];
        else if(input_buffer.find("Max. intensity =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MaxMinAvgIntensity[0];
        else if(input_buffer.find("Min. intensity =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MaxMinAvgIntensity[1];
        else if(input_buffer.find("Ave. intensity =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MaxMinAvgIntensity[2];
        else if(input_buffer.find("Branch size threshold =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchSizeThreshold;
        else if(input_buffer.find("Max. code =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MaxCode;
        else if(input_buffer.find("Max. boundary field =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MaxBoundaryField;
        else if(input_buffer.find("Max. branch level =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.MaxBranchLevel;
        else if(input_buffer.find("Total branch # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.TotalBranchNum;
        else if(input_buffer.find("Total un-traced voxel # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.UnTracedVoxel;
        else if(input_buffer.find("Un-traced voxel ratio =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.RatioUnTracedVoxel;
        else if(input_buffer.find("Total branch point # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchPoints;
        else if(input_buffer.find("Total end point # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.EndPoints;
        else if(input_buffer.find("Total loop point # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.LoopPoints;
        else if(input_buffer.find("Total bulge point # =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BulgePoints;
        else if(input_buffer.find("Branch total volume (V) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchTotalVolV;
        else if(input_buffer.find("Branch ave. cross-section area (V) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchAvgCrossSecV;
        else if(input_buffer.find("Branch total length (V) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchTotalLenV;
        else if(input_buffer.find("Branch total EndToEnd (V) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchTotalEndToEndV;
        else if(input_buffer.find("Branch total volume (R) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchTotalVolR;
        else if(input_buffer.find("Branch ave. cross-section area (R) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchAvgCrossSecR;
        else if(input_buffer.find("Branch total length (R) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchTotalLenR;
        else if(input_buffer.find("Branch total EndToEnd (R) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BranchTotalEndToEndR;
        else if(input_buffer.find("Bulge total volume (V) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BulgeTotalVolV;
        else if(input_buffer.find("Bulge total volume (R) =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.BulgeTotalVolR;
        else if(input_buffer.find("Voxel # reconstructed by SWC =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.SWC_Voxel;
        else if(input_buffer.find("Voxel # overlapping =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.SWC_Overlap;
        else if(input_buffer.find("Voxel # only in target =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.SWC_TargetOnly;
        else if(input_buffer.find("Voxel # only in SWC =") != string::npos)
            iss >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> dontcare >> single_LTH.TreeData.SWC_SWCOnly;
        else if(input_buffer.find("Branch file name") != string::npos){
            getline(fid,input_buffer);
            break;
        }
        //finally QQ
    }

    for(int i=0;i<single_LTH.TreeData.TotalBranchNum;++i){
        getline(fid,input_buffer);
        istringstream iss(input_buffer);

        BRANCH_DATA tmp;

        iss >> tmp.fname;
        iss >> tmp.CP;
        iss >> tmp.VolV;
        iss >> tmp.AveAreaV;
        iss >> tmp.LenV;
        iss >> tmp.EndToEndV;
        iss >> tmp.VolR;
        iss >> tmp.AveAreaR;
        iss >> tmp.LenR;
        iss >> tmp.EndToEndR;
        iss >> tmp.Type;
        iss >> tmp.BranchNum;
        iss >> tmp.Level;
        iss >> tmp.ParentID;
        tmp.ParentID--;

        single_LTH.BranchData.push_back(tmp);

    }
    fid.close();

    vector<int> BPX;
    vector<int> BPY;
    vector<int> BPZ;

    vector<int> LPX;
    vector<int> LPY;
    vector<int> LPZ;

    vector<int> EPX;
    vector<int> EPY;
    vector<int> EPZ;

    getline(bchid,input_buffer);
    for(int i=0;i<single_LTH.TreeData.TotalBranchNum;++i){
        int ns = 0;
        float cp = single_LTH.BranchData[i].CP;

        if(cp == 0)
            ns = 18;
        else
            ns = cp+15;

        vector<int> tmp_CPX;
        vector<int> tmp_CPY;
        vector<int> tmp_CPZ;
        int tmp_X = 0;
        int tmp_Y = 0;
        int tmp_Z = 0;
        string tmp_Type;

        for(int j=0;j<ns;++j){
            getline(bchid,input_buffer);
            if(cp!=0 && (j>=15)){
                istringstream iss(input_buffer);
                int tmp_int;
                iss >> tmp_int;
                tmp_CPZ.push_back(tmp_int);
                iss >> tmp_int;
                tmp_CPY.push_back(tmp_int);
                iss >> tmp_int;
                tmp_CPX.push_back(tmp_int);
            }
            if(cp == 0 && j == 16){
                istringstream iss(input_buffer);
                iss >> tmp_Type ;
            }
        }
        istringstream tmp_iss(input_buffer);
        tmp_iss >> tmp_Z >> tmp_Y >> tmp_X;

        if(cp != 0){
            getline(bchid,input_buffer);
            getline(bchid,input_buffer);
            istringstream iss(input_buffer);
            iss >> tmp_Type;
        }

        if(tmp_Type == "BP"){
            n_bp++;
            BPX.push_back(tmp_X);
            BPY.push_back(tmp_Y);
            BPZ.push_back(tmp_Z);
        }
        else if(tmp_Type == "LP"){
            n_lp++;
            LPX.push_back(tmp_X);
            LPY.push_back(tmp_Y);
            LPZ.push_back(tmp_Z);
        }
        else if(tmp_Type == "EP"){
            n_ep++;
            EPX.push_back(tmp_X);
            EPY.push_back(tmp_Y);
            EPZ.push_back(tmp_Z);
        }
        Points.CPX.push_back( tmp_CPX );
        Points.CPY.push_back( tmp_CPY );
        Points.CPZ.push_back( tmp_CPZ );

        while(getline(bchid,input_buffer)){
            if(input_buffer[0] == '@')
                break;
        }
    }
    bchid.close();
    if(n_bp){
        Points.BP[0] = BPX;
        Points.BP[1] = BPY;
        Points.BP[2] = BPZ;
    }
    if(n_lp){
        Points.LP[0] = LPX;
        Points.LP[1] = LPY;
        Points.LP[2] = LPZ;
    }
    if(n_ep){
        Points.EP[0] = EPX;
        Points.EP[1] = EPY;
        Points.EP[2] = EPZ;
    }
    Points.NumBP = n_bp;
    Points.NumLP = n_lp;
    Points.NumEP = n_ep;

    return;
}

void read_BID(string foldername,vector<int>& branch_id){
    string a(foldername.c_str(),foldername.size()-8);
    string b = a + string("-ID.am");
#if defined(_WIN32) || defined(_WIN64)
    string fname = foldername + string("\\") + b;
#elif defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
    string fname = foldername + string("/") + b;
#endif
    fstream fid(fname.c_str(),fstream::in);
    if(!fid.is_open()){
        cerr << "***** ERROR : cannot open " << fname << " *****" <<endl;
        return;
    }

    string input_buffer;

    branch_id.clear();

    while(input_buffer != "@1")
        getline(fid,input_buffer);

    while(getline(fid,input_buffer)){
        if(input_buffer.size() == 0)
            break;
        int tmp = atoi(input_buffer.c_str());
        branch_id.push_back(tmp);
    }
    fid.close();
    return ;
}

void voxel_life_mask_v6(vector<sLTH> &LTH, MAJOR major, PARA para,
                        string lsm_name, string fpath,
                        int life_index_min, int life_index_max, int life_index_step,
                        LIFE_HISTOGRAM &life_histogram){
//    % Mask with life-index cutoff
//    % 輸出：life_histogram 為 life-index 之 histogram 統計，另外會輸出一個處理以 life-index 當
//    %             mask 把原始 am 檔切出來的新 am 檔
//    % 輸入：LTH：以 read_alltrees_v2 程式，從 FASTA output 所讀出之各資料
//    %             lsm：原始影像之路徑與檔名
//    %             fpath：輸出檔案之路徑
//    %             major, para: Tuning parameters for life-index

//    % 骨幹（Central points）是否要加計 life-index
    char dontcare_char[20];

    int WT_CENTRAL_POINTS = 0;
    string para_string = string("_") + string(itoa(major.min_l,dontcare_char,10));
    para_string += string("_") + string(itoa(major.len_fac,dontcare_char,10));
    para_string += string("_") + string(itoa(major.max_lev,dontcare_char,10));
    para_string += string("_") + string(itoa(major.num_fac,dontcare_char,10));
    para_string += string("_") + string(itoa(para.level,dontcare_char,10));
    para_string += string("_") + string(itoa(para.number,dontcare_char,10));
    para_string += string("_") + string(itoa(para.length,dontcare_char,10));
    string input_buffer;
    vector<int> lsm_box;
    vector<vector<vector<int> > > lsm_mesh;
    int x1 = 0;
    int x2 = 0;
    int x0 = 0;
    int y1 = 0;
    int y2 = 0;
    int y0 = 0;
    int z1 = 0;
    int z2 = 0;
    int z0 = 0;
    vector<vector<vector<float> > > life_index;
    vector<int> node_parent;
    vector<int> num_offsprings;
    vector<int> level_offspring;
    vector<int> offsprings_name;
    vector<OFFSPRING> offsprings;
    int life_max = 0;
    int life_min = 0;
    int life_step = 0;

//    % 設定 life-index 上限
    int life_max_limit = 10000;
    int nbins = 100;
    float wt_level = 0.0;
    float wt_number = 0;
    float wt_length = 0;
    int n_th = 0;
//    % 設定每個 branch 貢獻 life-index 上限
//    %life_branch_limit = ceil(life_max_limit/length(LTH));
    int life_branch_limit = life_max_limit;
//    % output_life=1 則輸出 life_index
    int output_life = 1;
//    % output_mask=1 則輸出用 life_index 與當 mask 切出原始的 intensity
    int output_mask = 1;
    if(life_index_max == life_index_min){
        output_life = 1;
        output_mask = 0;
    }

//    % 開啟並讀入原始 am 檔
    vector<int> number_underline = c_strfind(lsm_name,string("_") );
    if(number_underline.size() > 1){
        int size_to_removed = number_underline[1] - number_underline[0] ;
        lsm_name.replace(number_underline[0],size_to_removed,"");
        cout << "Source = " << lsm_name <<endl;
    }
    cout << lsm_name <<endl;

    fstream fid(lsm_name.c_str(),fstream::in);
    if(!fid.is_open()){
        cerr << "***** ERROR : cannot open " << lsm_name << " *****" <<endl;
        return;
    }

    while(input_buffer !="@1"){
        getline(fid,input_buffer);
        if(input_buffer.find("define Lattice") != string::npos){
            istringstream iss(input_buffer);
            string dontcare;
            lsm_box.resize(3,-1);

            iss >> dontcare >> dontcare;
            iss >> lsm_box[0] >> lsm_box[1] >> lsm_box[2];
        }
    }
    //resize lsm_mesh b0 * b1 * b2
    lsm_mesh.resize(lsm_box[0]);
    for(int i=0;i<lsm_mesh.size();++i){
        lsm_mesh[i].resize(lsm_box[1]);
        for(int j=0;j<lsm_box[1];++j)
            lsm_mesh[i][j].resize(lsm_box[2]);
    }
    //input mesh
    for(int k=0;k<lsm_box[2];++k){
        for(int j=0;j<lsm_box[1];++j){
            for(int i=0;i<lsm_box[0];++i){
                getline(fid,input_buffer);
                int tmp = atoi(input_buffer.c_str());
                if(tmp >= LTH[0].threshold)
                    lsm_mesh[i][j][k] = tmp;
                else
                    lsm_mesh[i][j][k] = 0;
            }
        }
    }
    fid.close();
    cout << "finished reading mesh" <<endl;

//    %  branch 在下游 branch 數、下游 branch 世代數未達計分標準，但長度超過 major.len_fac 時 life-index 亦加分
    major.len_fac = (float)(int)(major.min_l * major.len_fac + 0.5);
//    % 原始檔案中之最大 intensity
    major.max_intensity = LTH[0].TreeData.MaxMinAvgIntensity[0];

    n_th = LTH.size();

    sLTH &tmp_LTH = LTH[0];
    x1 = tmp_LTH.TreeData.Xrange[0];
    x2 = tmp_LTH.TreeData.Xrange[1];
    x0 = x2 - x1 + 1;

    y1 = tmp_LTH.TreeData.Yrange[0];
    y2 = tmp_LTH.TreeData.Yrange[1];
    y0 = y2 - y1 + 1;

    z1 = tmp_LTH.TreeData.Zrange[0];
    z2 = tmp_LTH.TreeData.Zrange[1];
    z0 = z2 - z1 + 1;

//    % FAST 計算中之最大 global intensity cutoff
    major.max_threshold = LTH[n_th-1].threshold;
//    % 每個 voxel 的 life-index 歸零
    life_index.resize(x2+1);
    for(int i=0;i<=x2;++i){
        life_index[i].resize(y2+1);
        for(int j=0;j<=y2;++j)
            life_index[i][j].resize(z2+1,0);
    }
//    % 列出最小 FAST threshold 時，所有 branch 的 parent branch ID, 此處的 node 為一個 branch
    for(int i=0;i<LTH[0].BranchData.size();++i){
        node_parent.push_back( LTH[0].BranchData[i].ParentID );
    }
//    % 計算 FAST 最小 threshold 時，每個 branch 的後代代數 (level_offspring)、後代總數 (num_offspring)、以及所有後代之 branch ID (offspring_name
    tree_offspring(node_parent,num_offsprings,level_offspring,offsprings_name);

    offsprings.resize(n_th);

    offsprings[0].level = level_offspring;
    offsprings[0].num = num_offsprings;

    vector<int> a;
    for(int i=0;i<offsprings[0].level.size();++i){
        int tmp = offsprings[0].level[i];
        if(tmp > 0)
            a.push_back(tmp);
    }

    float g1 = quantile0(a,0.75);cout << "g1 : " << g1  <<" major.max_lev : " << major.max_lev <<endl;
    if(g1 < major.max_lev)
        major.max_lev = g1;

    cout << fpath <<": max-th=" << (int)LTH[n_th-1].threshold << ", min-th=" << (int)LTH[0].threshold <<endl;
    cout << "min_l=" << major.min_l << ", len_fac" << major.len_fac << ", max_lev=" << major.max_lev << ", number_fac=" << major.num_fac <<endl;

    vector< vector<float> >min_all(n_th);
    for(int i=0;i<n_th;++i)
        min_all[i].resize(3,0);

    for(int i=0;i<n_th;++i){
//        % n_branch: 每個 FAST threshold 下的 branch 總數
        int n_branch = LTH[i].BranchData.size();
//        % branch_score: 各 branch 得到的分數
        vector<float> branch_score(n_branch,0.0);
//        % len_branch: 各 branch 長度（由 FAST 輸出之 branch length in real space, not in voxel）
        vector<float> len_branch(n_branch,0.0);
        float min_voxelsize = 999.9;
        for(int k=0;k<LTH[i].TreeData.VoxelSize.size();++k){
            if(min_voxelsize > LTH[i].TreeData.VoxelSize[k])
                min_voxelsize = LTH[i].TreeData.VoxelSize[k];
        }
        for(int j=0;j<n_branch;++j){

            len_branch[j] = LTH[i].BranchData[j].LenR / min_voxelsize;
        }

//        % 列出各 FAST threshold 時，所有 branch 的 parent branch ID, 此處的 node 為一個 branch
        node_parent.clear();
        for(int j=0;j<LTH[i].BranchData.size();++j)
            node_parent.push_back( LTH[i].BranchData[j].ParentID );
//        % 計算各 FAST threshold 時，每個 branch 的後代代數 (level_offspring)、後代總數 (num_offspring)、以及所有後代之 branch ID (offspring_name)
        tree_offspring(node_parent,num_offsprings,level_offspring,offsprings_name);
        offsprings[i].level = level_offspring;
        offsprings[i].num = num_offsprings;
//        % mins[0]: 在此 FAST threshold 下，計入 branch life-index 之最小長度
//        % mins[1]: 在此 FAST threshold 下，計入 branch life-index 之最小下游代數
//        % mins[2]: 在此 FAST threshold 下，計入 branch life-index 之最小下游 branch 數
//        % mins[1] 與 mins[2] 會隨著 FAST threshold 升高而降低（放寬計入 life-index 之標準）
        vector<float> mins = major_br(LTH[i].threshold,major);
        min_all[i] = mins;

        for(int j=0;j<n_branch;++j){
//            % 依據「下游代數」所加計之 life-index 分數
            float score_level = max(0.0f,level_offspring[j] - mins[1]);
            score_level = pow(score_level,para.level);
            wt_level += score_level;
//            % 依據「下游 branch 數」所加計之 life-index 分數
            float score_number = floor( ((float)num_offsprings[j])/mins[2] );
            score_number = pow(score_number,para.number);
            wt_number += score_number;
//            % 依據「branch 長度」所加計之 life-index 分數，目前採計「major.len_fac 倍數的平方」
            float score_lengh = floor( ((float)len_branch[j]) / major.len_fac );
            score_lengh = pow(score_lengh,para.length);
            wt_length += score_lengh;
//            % 計算三項總分，若破表超過 life_branch_limit（每個 branch 的最大分數）則砍到
//            % life_branch_limit
            float wt = score_lengh + score_level + score_number;
            wt = min((float)life_branch_limit,wt);
//            % 累加第 i 個 FAST threshold 下，第 j 個 branch 的分數
            branch_score[j]+=wt;
//            % 如果 WT_CENTRAL_POINTS 設為 1，則加計骨幹分數
            if(WT_CENTRAL_POINTS){
                int ncp = LTH[i].Points.CPX[j].size();
                for(int k=0;k<ncp;++k){
                    int ix = LTH[i].Points.CPX[j][k];
                    int iy = LTH[i].Points.CPY[j][k];
                    int iz = LTH[i].Points.CPZ[j][k];
//                    % 所有的 central points 分數加計 2 倍（也就是非 central points 的三倍）
                    life_index[ix][iy][iz] += 2.0*wt;
                }
            }
//            % 當末端 branch 長度超過 major.len_fac標準，但其上游若有 branch 沒有得到任何分數，這根長 branch
//            % 會因此無法根本體連結也被刪除，故必須 trace back，把其上游所有的 branch 都加上跟此長 branch 一樣的
//            % life-index 以確保上游連結
            int jm = node_parent[j];
            int trace_back = 0;
//            % 回溯次數只需到 mins(2)+1 為止，因為再往上的 branch 必定會因「下游代數」加分
            while( (trace_back<= mins[1]+1) && (jm>=0) ){
                branch_score[jm] = max( branch_score[j], branch_score[jm] );
                int ncpm = LTH[i].Points.CPX[jm].size();
                if(WT_CENTRAL_POINTS){
                    for(int k=0;k<ncpm;++k){
                        int ix = LTH[i].Points.CPX[jm][k];
                        int iy = LTH[i].Points.CPY[jm][k];
                        int iz = LTH[i].Points.CPZ[jm][k];
                        life_index[ix][iy][iz] += 2.0*wt;
                    }
                }
                jm = node_parent[jm];
                trace_back++;
            }
        }
//        % 所有 voxel 所屬之 branch ID
        vector<int> orig_data = LTH[i].BID;
//        % 只撈出 branch ID 非零的點（屬於neuron的點）
        vector<int> bid_idx;
        vector<int> bid;
        for(int j=0;j<orig_data.size();++j){
            if(orig_data[j] != 0){
                bid_idx.push_back(j);
                bid.push_back(orig_data[j]);
            }
        }
        float id_xmin=LTH[i].TreeData.Xrange[0];
        float id_xmax=LTH[i].TreeData.Xrange[1];
        float id_ymin=LTH[i].TreeData.Yrange[0];
        float id_ymax=LTH[i].TreeData.Yrange[1];
        float id_zmin=LTH[i].TreeData.Zrange[0];
        float id_zmax=LTH[i].TreeData.Zrange[1];
        int rx=id_xmax-id_xmin+1;
        int ry=id_ymax-id_ymin+1;
        int rz=id_zmax-id_zmin+1;
        float rr = rx * ry;
//        % 將每個 branch 所涵蓋之 voxel 於此 FAST threshold 所得之 life-index 分數計入 life_time
//        % 這個陣列中
        for(int ii=0;ii<bid_idx.size();++ii){
            int bid_ii = bid[ii];
            int ibd = bid_idx[ii]+1;
            int iz=(int)((ibd-1)/rr)+id_zmin+1;
            int iy=(int)((ibd-1-rr*(iz-id_zmin-1))/rx)+id_ymin+1;
            int ix=((ibd-1)%rx)+id_xmin+1;
            life_index[ix-1][iy-1][iz-1] += branch_score[ bid[ii]-1 ];
        }

        life_max = -9999;
        life_min = 9999;
        for(int j=0;j<life_index.size();++j){
            for(int k=0;k<life_index[j].size();++k){
                for(int m=0;m<life_index[j][k].size();++m){
                    life_index[j][k][m] = (float)(int)(life_index[j][k][m]);

                    if(life_max < life_index[j][k][m])
                        life_max = life_index[j][k][m];
                    if(life_min > life_index[j][k][m])
                        life_min = life_index[j][k][m];
                }
            }
        }
//        % 計算 life-index 分布之 histogram
        life_step = (life_max - life_min) / nbins;
//        life_histogram.bin=life_min:life_step:life_max;
//        life_histogram.hist=hist(life_index(:),life_min:life_step:life_max);

    }

//    % 針對不同的 life-index threshold 當作 mask，所有 life-index 低於 life-cut 的 voxel
//    % 會被捨棄，其他的點會以原有的 intensity 值輸出

    time_t time_now = time(NULL);
    string time_string = asctime( localtime(&time_now) );
    int lsm_mesh_max = -999;
    int lsm_mesh_min = 999;
    for(int i=0;i<lsm_mesh.size();++i){
        for(int j=0;j<lsm_mesh[i].size();++j){
            for(int k=0;k<lsm_mesh[i][j].size();++k){
                if(lsm_mesh_max < lsm_mesh[i][j][k])
                    lsm_mesh_max = lsm_mesh[i][j][k];
                if(lsm_mesh_min > lsm_mesh[i][j][k])
                    lsm_mesh_min = lsm_mesh[i][j][k];
            }
        }
    }
    vector<int> loc = c_strfind(fpath,string("/"));
    int st = loc[ loc.size()-1 ] +1 ;

    if(output_mask == 1){
        for(int life_cut = life_index_min; life_cut<= life_index_max; life_cut+= life_index_step){
            cout << "Output the final results for life-index cut = " << life_cut << "...." <<endl;
//            str_time=datestr(now);
//            str_time=strrep(str_time,' ','-');
//            str_time=strrep(str_time,':','-');


            string outname = fpath + string("/") + string(itoa(life_cut,dontcare_char,10)) + string("_") + string(fpath.begin()+st , fpath.end()) + para_string + string(".am");
            cout <<"outputing : " << outname <<endl;
            FILE* fout = fopen(outname.c_str(),"w");
            if(fout == NULL){
                cerr << "***** ERROR : cannot open " << outname <<" *****" <<endl;
                return;
            }
            fprintf(fout,"# AmiraMesh 3D ASCII 2.0\n\n");
            fprintf(fout,"# Creation date: %s", time_string.c_str());
            fprintf(fout,"# Created by program: voxel_life_mask_v6.m\n");
            fprintf(fout,"# Author: Chi-Tin Shih\n\n");
            fprintf(fout,"# NeuroRetriver Parameters:\n");
            fprintf(fout,"# Minimal length of branches = %f\n", major.min_l);
            fprintf(fout,"# Length factor = %f\n", major.len_fac);
            fprintf(fout,"# Number factor = %f\n", major.num_fac);
            fprintf(fout,"# Power of level = %f\n", para.level);
            fprintf(fout,"# Power of number = %f\n", para.number);
            fprintf(fout,"# Power of length = %f\n", para.length);
            fprintf(fout,"##############################\n");
            fprintf(fout,"# Extra information:\n");
            fprintf(fout,"# This is the life-time for each branches segmented and traced by FASTA for a neuroimage.\n");
            fprintf(fout,"# Max. X = %d\n", x2);
            fprintf(fout,"# Min. X = %d\n", x1);
            fprintf(fout,"# Max. Y = %d\n", y2);
            fprintf(fout,"# Min. Y = %d\n", y1);
            fprintf(fout,"# Max. Z = %d\n", z2);
            fprintf(fout,"# Min. Z = %d\n", z1);
            fprintf(fout,"# Max. voxel life time = %d\n", life_max);
            fprintf(fout,"# Min. voxel life time = %d\n", life_min);
            fprintf(fout,"# Voxel Size X = %f\n", LTH[0].TreeData.VoxelSize[0]);
            fprintf(fout,"# Voxel Size Y = %f\n", LTH[0].TreeData.VoxelSize[1]);
            fprintf(fout,"# Voxel Size Z = %f\n\n", LTH[0].TreeData.VoxelSize[2]);
            fprintf(fout,"define Lattice %d %d %d\n\n", x0, y0, z0);
            fprintf(fout,"Parameters {\n");
            fprintf(fout,"    Content \"%dx%dx%d short, uniform coordinates\",\n", x0, y0, z0);
            fprintf(fout,"    DataWindow %d %d\n", lsm_mesh_min, lsm_mesh_max);

            float bdx1=(x1)*LTH[0].TreeData.VoxelSize[0]+LTH[0].TreeData.MinBoundBox[0];
            float bdx2=(x2)*LTH[0].TreeData.VoxelSize[0]+LTH[0].TreeData.MinBoundBox[0];
            float bdy1=(y1)*LTH[0].TreeData.VoxelSize[1]+LTH[0].TreeData.MinBoundBox[1];
            float bdy2=(y2)*LTH[0].TreeData.VoxelSize[1]+LTH[0].TreeData.MinBoundBox[1];
            float bdz1=(z1)*LTH[0].TreeData.VoxelSize[2]+LTH[0].TreeData.MinBoundBox[2];
            float bdz2=(z2)*LTH[0].TreeData.VoxelSize[2]+LTH[0].TreeData.MinBoundBox[2];

            fprintf(fout,"    BoundingBox   %f   %f   %f   %f   %f   %f,\n", bdx1, bdx2, bdy1, bdy2, bdz1, bdz2);
            fprintf(fout,"    CoordType \"uniform\"\n");
            fprintf(fout,"}\n\n");
            fprintf(fout,"Lattice { short Data } @1\n\n");
            fprintf(fout,"# Data section follows\n");
            fprintf(fout,"@1\n");

            for(int iz=z1;iz<=z2;++iz){
                for(int iy=y1;iy<=y2;++iy){
                    for(int ix=x1;ix<=x2;++ix){
                        if(life_index[ix][iy][iz] > life_cut)
                            fprintf(fout,"%d\n",lsm_mesh[ix][iy][iz]);
                        else
                            fprintf(fout,"0\n");
                    }
                }
            }

            fclose(fout);
        }
    }

    if(output_life == 1){
        cout << "Output the final results for life-index file...." <<endl;

        string outname = fpath + string("/life-") + string(fpath.begin()+st , fpath.end()) + para_string + string(".am");
        cout <<"outputing : " << outname <<endl;
        FILE *fout = fopen(outname.c_str(),"w");
        if(fout == NULL){
            cerr << "***** ERROR : cannot open " << outname <<" *****" <<endl;
            return;
        }

        fprintf(fout,"# AmiraMesh 3D ASCII 2.0\n\n");
        fprintf(fout,"# Creation date: %s", time_string.c_str());
        fprintf(fout,"# Created by program: voxel_life.m\n");
        fprintf(fout,"# Author: Chi-Tin Shih\n\n");
        fprintf(fout,"# Life-Index Standard:\n");
        fprintf(fout,"#     Max. Level=%d (squared)\n", (int)major.max_lev);
        fprintf(fout,"#     Num. Factor=%d (linear)\n", (int)major.num_fac);
        fprintf(fout,"#     Min. Length=%d (squared)\n",(int) major.len_fac);
        fprintf(fout,"# Life-Index Contribution:\n");
        fprintf(fout,"#     Level=%d\n", (int)wt_level);
        fprintf(fout,"#     Number=%d\n", (int)wt_number);
        fprintf(fout,"#     Length=%d\n", (int)wt_length);
        fprintf(fout,"# Extra information:\n");
        fprintf(fout,"# This is the life-time for each branches segmented and traced by FASTA for a neuroimage.\n");
        fprintf(fout,"# Max. X = %d\n", x2);
        fprintf(fout,"# Min. X = %d\n", x1);
        fprintf(fout,"# Max. Y = %d\n", y2);
        fprintf(fout,"# Min. Y = %d\n", y1);
        fprintf(fout,"# Max. Z = %d\n", z2);
        fprintf(fout,"# Min. Z = %d\n", z1);
        fprintf(fout,"# Max. voxel life time = %d\n", life_max);
        fprintf(fout,"# Min. voxel life time = %d\n", life_min);
        fprintf(fout,"# Voxel Size X = %f\n", LTH[0].TreeData.VoxelSize[0]);
        fprintf(fout,"# Voxel Size Y = %f\n", LTH[0].TreeData.VoxelSize[1]);
        fprintf(fout,"# Voxel Size Z = %f\n\n", LTH[0].TreeData.VoxelSize[2]);
        fprintf(fout,"define Lattice %d %d %d\n\n", x0, y0, z0);
        fprintf(fout,"Parameters {\n");
        fprintf(fout,"    Content \"%dx%dx%d short, uniform coordinates\",\n", x0, y0, z0);
        fprintf(fout,"    DataWindow %d %d\n",lsm_mesh_min, lsm_mesh_max);
        float bdx1=(x1)*LTH[0].TreeData.VoxelSize[0]+LTH[0].TreeData.MinBoundBox[0];
        float bdx2=(x2)*LTH[0].TreeData.VoxelSize[0]+LTH[0].TreeData.MinBoundBox[0];
        float bdy1=(y1)*LTH[0].TreeData.VoxelSize[1]+LTH[0].TreeData.MinBoundBox[1];
        float bdy2=(y2)*LTH[0].TreeData.VoxelSize[1]+LTH[0].TreeData.MinBoundBox[1];
        float bdz1=(z1)*LTH[0].TreeData.VoxelSize[2]+LTH[0].TreeData.MinBoundBox[2];
        float bdz2=(z2)*LTH[0].TreeData.VoxelSize[2]+LTH[0].TreeData.MinBoundBox[2];
        fprintf(fout,"    BoundingBox   %f   %f   %f   %f   %f   %f,\n", bdx1, bdx2, bdy1, bdy2, bdz1, bdz2);
        fprintf(fout,"    CoordType \"uniform\"\n");
        fprintf(fout,"}\n\n");
        fprintf(fout,"Lattice { short Data } @1\n\n");
        fprintf(fout,"# Data section follows\n");
        fprintf(fout,"@1\n");

        for(int iz=z1;iz<=z2;++iz){
            for(int iy=y1;iy<=y2;++iy){
                for(int ix=x1;ix<=x2;++ix){
                    fprintf(fout,"%d\n",(int)life_index[ix][iy][iz]);
                }
            }
        }

        fclose(fout);
    }
#if defined(_WIN32) || defined(_WIN64)
    string sys_comm_rar = string("winrar A ") + string(fpath.begin()+st , fpath.end()) + para_string + string(".rar *am");
    cout << sys_comm_rar <<endl;
    system(sys_comm_rar.c_str());
    system("copy *.rar ..\\NR_Results\\");
    system("del *.rar");
    system("del *.am");
#elif defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
    string sys_comm_rar = string("tar zcf ") + string(fpath.begin()+st , fpath.end()) + para_string + string(".tar.gz *am");
    cout << sys_comm_rar <<endl;
    system(sys_comm_rar.c_str());
    system("mkdir ../NR_Results");
    system("mv *gz ../NR_Results/");
    system("rm -f *gz");
    system("rm -f *am");
#endif
    return;
}

void tree_offspring(vector<int> &node_parent,
                    vector<int> &num_offspring,
                    vector<int> &level_offspring,
                    vector<int> &offsprings){

    num_offspring.clear();
    level_offspring.clear();
    offsprings.clear();

    vector<int> node_level;
    parent2level(node_parent,node_level);

    int max_level = vector_max(node_level);
    int num_node = node_parent.size();
    num_offspring.resize(num_node,0);
    level_offspring.resize(num_node,0);

    for(int i=max_level-1;i>=0;--i){
        vector<int> nodes;
        for(int j=0;j<node_level.size();++j)
            if(node_level[j] == i)
                nodes.push_back(j);
        for(int j=0;j<nodes.size();++j){
            vector<int> a;
            for(int k=0;k<node_parent.size();++k)
                if(node_parent[k] == nodes[j])
                    a.push_back(k);
            if(a.size() > 0){
                int sum_num_off = 0;
                for(int k=0;k<a.size();++k){
                    sum_num_off += num_offspring[ a[k] ];
                }
                num_offspring[ nodes[j] ] += sum_num_off + a.size();
                // get max(level_offspring(a))
                int max_level_offspring = -9999;
                for(int k=0;k<a.size();++k){
                    if(max_level_offspring < level_offspring[ a[k] ])
                        max_level_offspring = level_offspring[ a[k] ];
                }
                level_offspring[ nodes[j] ] = max_level_offspring +1;
            }// if a.size > 0
        }//j loop
    }//i loop

    return;
}

void parent2level(vector<int> &parent, vector<int> &node_level){

    int n_node = parent.size();
    node_level.resize(n_node,-1);

    vector<int> current_parent(1,-1);
    int current_level = 0;
    while(vector_find(node_level,-1).size() > 0){
        vector<int> daughters;
        for(int i=0;i<current_parent.size();++i){
            vector<int> nn = vector_find(parent,current_parent[i]);
            for(int j=0;j<nn.size();++j){
                daughters.push_back(nn[j]);
            }
        }
        current_level++;
        for(int i=0;i<daughters.size();++i){
            node_level[ daughters[i] ] = current_level;
        }
        current_parent = daughters;
    }

    return;
}

int findNearestNeighbourIndex( float value, vector< float > &x )
{
    float dist = FLT_MAX;
    int idx = -1;
    for ( int i = 0; i < x.size(); ++i ) {
        float newDist = value - x[i];
        if ( newDist > 0 && newDist < dist ) {
            dist = newDist;
            idx = i;
        }
    }

    return idx;
}

vector< float > interp1( vector< float > &x, vector< float > &y, vector< float > &x_new )
{
    vector< float > y_new;
    y_new.reserve( x_new.size() );

    std::vector< float > dx, dy, slope, intercept;
    dx.reserve( x.size() );
    dy.reserve( x.size() );
    slope.reserve( x.size() );
    intercept.reserve( x.size() );
    for( int i = 0; i < x.size(); ++i ){
        if( i < x.size()-1 )
        {
            dx.push_back( x[i+1] - x[i] );
            dy.push_back( y[i+1] - y[i] );
            slope.push_back( dy[i] / dx[i] );
            intercept.push_back( y[i] - x[i] * slope[i] );
        }
        else
        {
            dx.push_back( dx[i-1] );
            dy.push_back( dy[i-1] );
            slope.push_back( slope[i-1] );
            intercept.push_back( intercept[i-1] );
        }
    }

    for ( int i = 0; i < x_new.size(); ++i )
    {
        int idx = findNearestNeighbourIndex( x_new[i], x );
        y_new.push_back( slope[idx] * x_new[i] + intercept[idx] );

    }

}

float quantile0(vector<int> &Z, float P){
    sort(Z.begin(),Z.end());// 1 2 3 4

    vector<float> x;
    vector<float> s;
    vector<float> x_new(1, P);
    //make x
    x.push_back(0.0);
    for(int i=0;i<Z.size();++i){
        x.push_back( ( 0.5+(float)i ) / (float)Z.size() );
    }
    x.push_back(1.0);

    //make s
    s.push_back((float)Z.front());
    for(int i=0;i<Z.size();++i){
        s.push_back( (float)Z[i] );
    }
    s.push_back((float)Z.back());

    return interp1( x, s, x_new)[0];
}

vector<float> major_br(float th, MAJOR &major){
    vector<float> mins;
    float max_threshold = major.max_threshold;
    int th_step = min( 100,(int)(max_threshold/20 + 0.5) );
    int p = (int)( major.max_lev*(1.0-th/max_threshold) +0.5);
    if(p<1)
        p=1;

    mins.push_back(major.min_l);
    mins.push_back((float)p);
    mins.push_back((float)p * major.num_fac);
    return mins;
}
