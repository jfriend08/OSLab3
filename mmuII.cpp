/*
Usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile
--	detect output flag, algorithm methods, and number of frames
--	init input, init rfile
--	myrandom function
--	will able to change, printp, printf
--	use bitwise operator
--	adding algo=r method, random generator is fine, need to add unmap method

Todo:	replacement algorithms is needed

*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector> 
#include <list>
#include <sstream>
#include <iterator>
#include <map> 
#include <iomanip>
#include <utility> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <limits>
#include <queue>

using namespace std;
vector<int> randvals;
vector<int> tasks_tmp;
vector<vector<int> > tasks;
vector<int> frametable;
vector<int> fram2page_v;
int num_frames=32, num_pages=64, O_flag=0, P_flag=0, F_flag=0, S_flag=0, p_flag=0, f_flag=0, a_flag=0; 
int c, opterr=0, n1, n2, ofs=0;
string ovalue, frames, arg_tmp, algo="l", fin_string;
char fin_char;

void flagAssign(string outputVal){
	for (int i=0; i<outputVal.length(); i++){
		if (outputVal[i]=='O'){O_flag=1;}
		else if (outputVal[i]=='P'){P_flag=1;}
		else if (outputVal[i]=='F'){F_flag=1;}
		else if (outputVal[i]=='S'){S_flag=1;}
		else if (outputVal[i]=='p'){p_flag=1;}
		else if (outputVal[i]=='f'){f_flag=1;}
		else if (outputVal[i]=='a'){a_flag=1;}		
	}
	cout<<"O_flag="<<O_flag<<" P_flag="<<P_flag<<" F_flag="<<F_flag<<" S_flag="<<S_flag<<" p_flag="<<p_flag<<" f_flag="<<f_flag<<" a_flag="<<a_flag<<endl;
}

void input_init(string input1){
	string line;
	int found;
	ifstream fin0 ( input1 );
	if (!fin0.is_open()){cout<<"Cannot open the file0"<<endl;	}
	else{
		while(!fin0.eof()){
			getline(fin0, line);
			if (line[0]!='#' && line !=""){				
				std::stringstream(line)>>n1>>n2;tasks_tmp.push_back(n1);tasks_tmp.push_back(n2);
        		tasks.push_back(tasks_tmp);tasks_tmp.clear();
			}					
		}		
	}
	fin0.close();
}

void rfile_init(string input2){
	ifstream fin ( input2 );
	if (!fin.is_open()){cout<<"Cannot open the file1"<<endl;	}
	else{
		for (int i=0; !fin.eof();i++){
			int temp;fin>>temp;randvals.push_back(temp);			
		}
	}
}
int myrandom(int burst, int &index) { 	
	if (index==randvals.size()){index=1;}
	index++;		
	return 1 + (randvals[index] % burst);	
}

void frametable_init(int frames){
	for (int i=0; i<frames; i++){
		frametable.push_back(-1);
	}
}
void fram2page_init(int frames){
	for (int i=0; i<frames; i++){
		fram2page_v.push_back(-1);
	}
}

class PTE {
public:
	int value;	
	PTE(){value=0;}
	// PTE(){value=0; value|=512; cout<<value<<endl;}
	void Zero(int rw, int page_index, int frame_index, int inst){				
		string tmp="";
		printf("%d: ZERO %4s %3d\n", inst ,tmp.c_str(), frame_index);	
	}
	void Map(int rw, int page_index, int frame_index, int inst){				
		printf("%d: MAP %5d %3d\n", inst ,page_index, frame_index);			
		frametable[frame_index]=page_index;
		if (rw==1){value|=896;}
		else value|=768;
		value|=frame_index;
		// PRESENT=1; REFERENCED=1; MODIFIED=1; PAGEDOUT=0; idx2frame=frame_index;		
	}

	int frameFull(){
		for (int i =0; i<num_frames; i++){
			if (frametable[i]==-1){
				return i;
			}		
		}
		return -1;
	}

	int replace(){
		if (algo=="l"){
			int index =frameFull();
			if (index != -1) return index;
			else return 0;
		}
		if (algo=="r"){
			int index =frameFull();
			if (index != -1) return index;
			else{
				int index= myrandom(num_frames, ofs);
				return index-1;
			} 
		}
		else return 0;
	}
};


class PTE page_table[64];		

void printp(int p_flag){
	if (p_flag==1){
		for(int i=0; i<num_pages; i++){
			int control_bit=page_table[i].value;
			control_bit>>=6;
			if((control_bit & 8) !=8){				
				printf("* ");			
			}
			else if((control_bit & 8) == 8){
				cout<<i<<":";
				if((control_bit & 4) == 4)cout<<"R"; else cout<<"-";
				if((control_bit & 2) == 2)cout<<"M"; else cout<<"-";
				if((control_bit & 1) == 1)cout<<"S "; else cout<<"- ";
			}
		}
		cout<<endl;
	}	
}
void printf(int f_flag){
	if (f_flag==1){
		for(int i=0; i<num_frames; i++){
			if(frametable[i]==-1){cout<<"* ";			}
			else {
				cout<<frametable[i]<<" ";
			}				
		}
		cout<<"\n";
	}	
}


void change(int rw, int page_index, int inst){	
	if (page_table[page_index].value==0){
		int frame_index=page_table[page_index].replace();
		page_table[page_index].Zero(rw, page_index, frame_index, inst);
		page_table[page_index].Map(rw, page_index, frame_index, inst);
	}
}

int main(int argc, char *argv[]){
	while((c=getopt(argc,argv,"a:o:f:")) !=-1){
		switch (c){
			case 'a':
				algo.assign(optarg);
				break;
			case 'o':
				ovalue.assign(optarg);				
				flagAssign(ovalue);
				break;
			case 'f':					
				num_frames=atoi(optarg);				
				break;
			default:
				abort();
		}
	}
	cout<<"algo="<<algo<<" ovalue="<<ovalue<<" num_frames="<<num_frames<<endl;
	
	input_init(argv[argc-2]);	
	rfile_init(argv[argc-1]);			
	frametable_init(num_frames);
	fram2page_init(num_frames);
	// for (int task_idx=0; task_idx<24; task_idx++){
	for (int task_idx=0; task_idx<tasks.size(); task_idx++){
		cout<<"==> inst: "<<tasks[task_idx][0]<<" "<<tasks[task_idx][1]<<endl;
		change(tasks[task_idx][0], tasks[task_idx][1], task_idx);
		printp(p_flag);	
		printf(f_flag);	
	}


	return 0;
}

