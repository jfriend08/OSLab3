/*
Usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile
--	detect output flag, algorithm methods, and number of frames
--	init input, init rfile
--	myrandom function
--	will able to change, printp, printf
--	use bitwise operator
--	adding algo=r method, random generator is fine, need to add unmap method
--	have better OO design
--	now r method looks good. Only need to print final report
--	can print O, F, P as the order of option string

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
vector<char> printOrder_v;
int num_frames=32, num_pages=64, O_flag=0, P_flag=0, F_flag=0, S_flag=0, p_flag=0, f_flag=0, a_flag=0, R_flag=0; 
int c, opterr=0, n1, n2, ofs=0;
string ovalue, frames, arg_tmp, algo="l", fin_string;
char fin_char;

struct bit {
		 unsigned int P:1;
		 unsigned int R:1;
		 unsigned int M:1;
		 unsigned int S:1;
		 unsigned int idx:6;		 
};

void flagAssign(string outputVal){
	for (int i=0; i<outputVal.length(); i++){
		if (outputVal[i]=='O'){O_flag=1;printOrder_v.push_back('O');}
		else if (outputVal[i]=='P'){P_flag=1;printOrder_v.push_back('P');}
		else if (outputVal[i]=='F'){F_flag=1;printOrder_v.push_back('F');}
		else if (outputVal[i]=='S'){S_flag=1;printOrder_v.push_back('S');}
		else if (outputVal[i]=='p'){p_flag=1;}
		else if (outputVal[i]=='f'){f_flag=1;}
		else if (outputVal[i]=='a'){a_flag=1;}		
		else if (outputVal[i]=='R'){R_flag=1;}		
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

void printp(int p_flag, bit* page_table, vector<int>* frame_table){
	if (p_flag==1){
		for(int i=0; i<num_pages; i++){			
			if(	page_table[i].P!=1 && page_table[i].S !=1){
				cout<<"* ";
			}							
			else if(page_table[i].P!=1 && page_table[i].S ==1)cout<<"# ";
			else if(page_table[i].P ==1){
				cout<<i<<":";
				if(page_table[i].R ==1)cout<<"R"; else cout<<"-";
				if(page_table[i].M ==1)cout<<"M"; else cout<<"-";
				if(page_table[i].S ==1)cout<<"S "; else cout<<"- ";
			}
		}
		cout<<endl;
	}	
}

void printf(int f_flag, bit* page_table, vector<int>* frame_table){
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
	virtual int Change(bit* page_table, vector<int>* frame_table){return 0;} 
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
	int Change (bit* page_table, vector<int>* frame_table){
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
	int FrameIndexPresent(int value){
		value<<=13; value>>=13;
		if (value>0) return 1; 
		else return 0;
	}
	////Zero: zero frame table [i], then print out ////
	void Zero(int rw, int page_index, int frame_index, int inst){		
		workon_F->at(frame_index)=0;
		string tmp="";
		if(O_flag==1)printf("%d: ZERO %4s %3d\n", inst ,tmp.c_str(), frame_index);	
	} 

	//// remove idx at page table, remove present R bit. I did not remove M bit ////
	void Unmap(int page_index, int frame_index, int inst){				
		int index_to_change=workon_F->at(frame_index);
		workon_P[index_to_change].idx=0; workon_P[index_to_change].P=0;
		workon_P[index_to_change].R=0; 
		if(O_flag==1)printf("%d: UNMAP %3d %3d\n", inst ,workon_F->at(frame_index), frame_index);
	}
	//// add S bit ////
	void Pageout(int rw, int page_index, int frame_index, int inst){
		int index_to_change=workon_F->at(frame_index);
		workon_P[index_to_change].S=1;
		if(O_flag==1)printf("%d: OUT %5d %3d\n", inst ,workon_F->at(frame_index), frame_index);			
	}
	void Pagein(int rw, int page_index, int frame_index, int inst){
				
		if(O_flag==1)printf("%d: IN %6d %3d\n", inst ,page_index, frame_index);					
	}	

	//// at P and R bit, add index to frame, add M bit depends on rw, then print ////
	void Map(int rw, int page_index, int frame_index, int inst){
		workon_P[page_index].R=1;		
		workon_F->at(frame_index)=page_index; // assign page index to frame
		workon_P[page_index].P=1;
		workon_P[page_index].idx=frame_index;
		if (rw==1){workon_P[page_index].M=1;}else{workon_P[page_index].M=0;}		
		if(O_flag==1)printf("%d: MAP %5d %3d\n", inst ,page_index, frame_index);					
	}
	
	//// just add M bit ////
	void Modify (int rw, int page_index, int inst){
		workon_P[page_index].M=1;		
	}
	
	Pager* pager;
	bit* workon_P;
	vector<int>* workon_F;
	void Process(){				
		for (int task_idx=0; task_idx<tasks.size(); task_idx++){
			if(O_flag==1)cout<<"==> inst: "<<tasks[task_idx][0]<<" "<<tasks[task_idx][1]<<endl;
			// cout<<"==> inst: "<<tasks[task_idx][0]<<" "<<tasks[task_idx][1]<<" value:"<<workon_P[tasks[task_idx][1]].idx<<endl;
			int rw=tasks[task_idx][0];
			int page_index=tasks[task_idx][1];
			
			//page table not present, no swaped bit, frame not full
			if (workon_P[page_index].P==0 && workon_P[page_index].S!=1 && pager->frameFull()!=-1){		
				if (R_flag==1)cout<<"condition1"<<endl;
				int frame_index=pager->Change(workon_P, workon_F);	
				Zero(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);							
			}

			//page table not present, no swaped bit, frame full
			else if (workon_P[page_index].P ==0 && workon_P[page_index].S!=1 && pager->frameFull()==-1 ){		
				if (R_flag==1)cout<<"condition2"<<endl;
				int frame_index = pager->Change(workon_P, workon_F);	
				Unmap(page_index,frame_index, task_idx);							
				if (workon_P[workon_F->at(frame_index)].M == 1){ Pageout (rw, page_index, frame_index, task_idx);}
				Zero(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);											
			}

			//page table not present, has swaped bit, frame full
			else if (workon_P[page_index].P ==0 && workon_P[page_index].S==1 && pager->frameFull()==-1 ){		
				if (R_flag==1)cout<<"condition3"<<endl;
				int frame_index = pager->Change(workon_P, workon_F);	
				Unmap(page_index,frame_index, task_idx);							
				if (workon_P[workon_F->at(frame_index)].M == 1){ Pageout (rw, page_index, frame_index, task_idx);}
				Pagein(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);											
			}

			//page table present, frame full, read page only --> do nothing
			else if (workon_P[page_index].P==1 && pager->frameFull()==-1  && rw == 0){		
				if (R_flag==1)cout<<"condition4"<<endl;
			}		
			
			//page table present, frame full, write page --> add modify bit
			else if (workon_P[page_index].P==1 && pager->frameFull()==-1  && rw == 1){		
				if (R_flag==1)cout<<"condition5"<<endl;
				Modify(rw, page_index, task_idx);
			}	
			
			////pager->frameFull() == -1 --> page fault////			
			else if (pager->frameFull()==-1){				// frame is full
				if (R_flag==1)cout<<"condition6"<<endl;
				int frame_index = pager->Change(workon_P, workon_F);	
				Unmap(page_index,frame_index, task_idx);							
				if (workon_P[workon_F->at(frame_index)].M == 1){ Pageout (rw, page_index, frame_index, task_idx);}
				Zero(rw, page_index, frame_index, task_idx);
				Map(rw, page_index, frame_index, task_idx);							
			}
			// for (int i=0; i<64; i++){cout<<i<<":"<<workon_P[i]<<" ";}			
			// cout<<endl;
			printp(p_flag, workon_P, workon_F);	
			printf(f_flag, workon_P, workon_F);	
		}		
	}

	void printReport(){
		for(int i=0; i<printOrder_v.size(); i++){
			if (printOrder_v[i]=='P')printp(1, workon_P, workon_F);
			else if (printOrder_v[i]=='F')printf(1, workon_P, workon_F);
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
				if (ovalue!="")flagAssign(ovalue);
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
	bit page_table[64] = { };
	mmu.workon_P=page_table;
	mmu.workon_F=&frametable;
	input_init(argv[argc-2]);	
	rfile_init(argv[argc-1]);			
	
	mmu.Process();
	cout<<"size:"<<printOrder_v.size()<<endl;
	mmu.printReport();


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



















