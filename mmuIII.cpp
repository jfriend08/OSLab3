/*
Usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile
--	detect output flag, algorithm methods, and number of frames
--	init input, init rfile
--	myrandom function
--	will able to change, printp, printf
--	use bitwise operator
--	adding algo=r method, random generator is fine, need to add unmap method
--	have better OO design
--	==> inst 27: 1 6 has issue

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

void printp(int p_flag, int* page_table, vector<int>* frame_table){
	if (p_flag==1){
		for(int i=0; i<num_pages; i++){
			int control_bit=page_table[i];
			control_bit>>=6;			
			if((control_bit & 8) !=8 && (control_bit & 1) != 1){				
				printf("* ");			
			}
			else if((control_bit & 1) == 1)cout<<"# ";
			else if((control_bit & 8) == 8){
				cout<<i<<":";
				// cout<<i<<":"<<"control_bit:"<<control_bit;
				if((control_bit & 4) == 4)cout<<"R"; else cout<<"-";
				if((control_bit & 2) == 2)cout<<"M"; else cout<<"-";
				if((control_bit & 1) == 1)cout<<"S "; else cout<<"- ";
			}
		}
		cout<<endl;
	}	
}

void printf(int f_flag, int* page_table, vector<int>* frame_table){
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


class Pager{
public:
	virtual int Change(int* page_table, vector<int>* frame_table){return 0;} 
	int frameFull(){
		for (int i =0; i<num_frames; i++){
			if (frametable[i]==-1){
				return i;
			}		
		}
		return -1;
	}
};
class Random : public Pager {
public:
	int Change (int* page_table, vector<int>* frame_table){
		int index =frameFull();
			if (index != -1) return index;
			else{
				int index= myrandom(num_frames, ofs);
				return index-1;
			} 
	}
};

class MMU{
public:
	void frametable_init(int frames){
		for (int i=0; i<frames; i++){
			frametable.push_back(-1);
		}
	}
	void Zero(int rw, int page_index, int frame_index, int inst){
		workon_F->at(frame_index)=0;
		string tmp="";
		printf("%d: ZERO %4s %3d\n", inst ,tmp.c_str(), frame_index);	
	} 
	void Map(int rw, int page_index, int frame_index, int inst){
		int value=workon_P[page_index];		
		printf("%d: MAP %5d %3d\n", inst ,page_index, frame_index);			
		workon_F->at(frame_index)=page_index;
		// cout<<"pre Map value/rw:"<<value<<"/"<<rw<<" page_index:"<<page_index<<" workon_P[page_index]"<<workon_P[page_index]<<endl;
		if (rw==1){value|=896;}
		else if (rw==0) {value<<=13;value>>=13;value|=768;}
		// cout<<"post Map value/rw:"<<value<<"/"<<rw<<endl;
		value|=frame_index;
		workon_P[page_index]=value;
	}
	void Unmap(int page_index, int frame_index, int inst){
		// cout<<"Unmap page_index:"<<page_index<<" frame_index:"<<frame_index<<endl;
		int value=workon_P[workon_F->at(frame_index)];
		// cout<<workon_F->at(frame_index)<<":"<<workon_P[workon_F->at(frame_index)]<<endl;
		// cout<<"unmap value_pre:"<<value<<endl;
		value>>=6;value<<=6;value^=512;workon_P[workon_F->at(frame_index)]=value;
		// cout<<"unmap value_post:"<<value<<endl;
		printf("%d: UNMAP %3d %3d\n", inst ,workon_F->at(frame_index), frame_index);			
		// cout<<"workon_P[workon_F->at(frame_index)]"<<workon_P[workon_F->at(frame_index)]<<endl;
	}
	void Pageout(int rw, int page_index, int frame_index, int inst){
		workon_P[workon_F->at(frame_index)]|=64;
		printf("%d: OUT %5d %3d\n", inst ,workon_F->at(frame_index), frame_index);			
	}
	void Pagein(int rw, int page_index, int frame_index, int inst){}	
	
	Pager* pager;
	int* workon_P;
	vector<int>* workon_F;
	void Process(){
		
		// for (int i=0; i<num_frames; i++){cout<<"FrameTable:"<<workon_F->at(i)<<endl;}
		for (int task_idx=0; task_idx<40; task_idx++){
			cout<<"==> inst: "<<tasks[task_idx][0]<<" "<<tasks[task_idx][1]<<endl;
			int rw=tasks[task_idx][0];
			int page_index=tasks[task_idx][1];
			////pager->frameFull() != -1 --> no page fault////
			if (workon_P[tasks[task_idx][1]]==0 && pager->frameFull()!=-1){				
				int frame_index=pager->Change(workon_P, workon_F);	
				Zero(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);							
			}
			else if (pager->frameFull()==-1 && workon_P[page_index] ==0){
				int frame_index = pager->Change(workon_P, workon_F);	
				Unmap(page_index,frame_index, task_idx);							
				if ((workon_P[workon_F->at(frame_index)] & 128) == 128){ Pageout (rw, page_index, frame_index, task_idx);}
				Zero(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);							

			}
			else if (pager->frameFull()==-1 && workon_P[page_index] !=0 && rw == 0){}
			////pager->frameFull() == -1 --> page fault////			
			else if (pager->frameFull()==-1){				
				int frame_index = pager->Change(workon_P, workon_F);	
				Unmap(page_index,frame_index, task_idx);							
				if ((workon_P[workon_F->at(frame_index)] & 128) == 128){ Pageout (rw, page_index, frame_index, task_idx);}
				Zero(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);							
			}
			// for (int i=0; i<64; i++){cout<<i<<":"<<workon_P[i]<<" ";}			
			// cout<<endl;
			printp(p_flag, workon_P, workon_F);	
			printf(f_flag, workon_P, workon_F);	
		}
	}

};

int main(int argc, char *argv[]){
	MMU mmu;
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
	
	if (algo=="r") {mmu.pager = new Random;}
	mmu.frametable_init(num_frames);
	int page_table[64] = { };
	mmu.workon_P=page_table;
	mmu.workon_F=&frametable;
	input_init(argv[argc-2]);	
	rfile_init(argv[argc-1]);			
	
	mmu.Process();
	// for (int i=0; i<64; i++){cout<<mmu.workon_P[i]<<endl;}
	// for (int i=0; i<mmu.workon_F->size(); i++){cout<<mmu.workon_F->at(i)<<endl;}
	// fram2page_init(num_frames);
	// for (int task_idx=0; task_idx<24; task_idx++){
	// // for (int task_idx=0; task_idx<tasks.size(); task_idx++){
	// 	cout<<"==> inst: "<<tasks[task_idx][0]<<" "<<tasks[task_idx][1]<<endl;
	// 	// change(tasks[task_idx][0], tasks[task_idx][1], task_idx);
	// 	// printp(p_flag);	
	// 	// printf(f_flag);	
	// }


	return 0;
}



















