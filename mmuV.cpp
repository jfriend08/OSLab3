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
--	works fine for r f s c X l N a methods

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
vector<char> printOrder_v;
int num_frames=32, num_pages=64, O_flag=0, P_flag=0, F_flag=0, S_flag=0, p_flag=0, f_flag=0, a_flag=0, R_flag=0, clockP_flag=0; 
int c, opterr=0, n1, n2, ofs=0;
string ovalue, frames, arg_tmp, algo="l", fin_string;
char fin_char;
uint64_t U_count=0, M_count=0, I_count=0, O_count=0, Z_count=0, RW_count=0;
deque<int> FIFO_dq, LRU_dq;
int hand=0, unMap_count=1;
vector<uint32_t> AgingP_v, AgingF_v;

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
	// cout<<"O_flag="<<O_flag<<" P_flag="<<P_flag<<" F_flag="<<F_flag<<" S_flag="<<S_flag<<" p_flag="<<p_flag<<" f_flag="<<f_flag<<" a_flag="<<a_flag<<endl;
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
	if (index==randvals.size()-2){index=0;}
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
		if(algo=="f" | algo=="s"){
			cout<<" || ";
			for (int i=0; i<(signed)FIFO_dq.size(); ++i){cout<<FIFO_dq[i]<<" ";			}
		}
		else if(algo=="c" | algo=="X"){
			cout<<" || hand = "<<hand;					
		}
		else if (algo=="a" | algo=="Y"){
			cout<<" || ";
			for (int i=0; i<AgingF_v.size(); i++){cout<<i<<":"<<AgingF_v[i]<<" ";			}	
		}
		cout<<"\n";
	}	
}
void printf_final(int f_flag, bit* page_table, vector<int>* frame_table){
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

	vector<int> check_existP(bit* page_table, vector<int>* frame_table){
		vector<int> existPage;		
		for (int i=0; i<num_pages; i++){			
			if(page_table[i].P==1){existPage.push_back(i);			}
		}
		return existPage;
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

class FIFO : public Pager{
	int Change (bit* page_table, vector<int>* frame_table){
		int index=frameFull();
		if (index !=-1){
			FIFO_dq.push_back(index);
			return index;
		}
		else{
			int new_index= FIFO_dq[0];			
			FIFO_dq.pop_front();
			FIFO_dq.push_back(new_index);
			return new_index;			
		}
	}
};
class SecondChance : public Pager{
	int Change (bit* page_table, vector<int>* frame_table){
		int index=frameFull();		
		if (index !=-1){
			FIFO_dq.push_back(index);
			return index;
		}	
		else{			
			int new_index= FIFO_dq[0]; //this return the frame index			
			int p_idx = frame_table->at(new_index);
			int Rbit = page_table[p_idx].R;			
			while (Rbit!=0){
				FIFO_dq.pop_front();
				page_table[p_idx].R=0;				
				FIFO_dq.push_back(new_index);
				new_index= FIFO_dq[0];
				p_idx = frame_table->at(new_index);
				Rbit = page_table[p_idx].R;
			}
			FIFO_dq.pop_front();
			FIFO_dq.push_back(new_index);
			return new_index;			
		}
	}
};

class Clock_f : public Pager{
	int Change (bit* page_table, vector<int>* frame_table){
		int index=frameFull();		
		if (index !=-1){
			FIFO_dq.push_back(index);
			return index;
		}	
		else{
			int p_idx=frame_table->at(hand);
			if(page_table[p_idx].R==0){
				int tmp2=hand;
				hand++;
				if(hand>frametable.size()-1)hand=0;
				return tmp2;	
			}
			while(page_table[p_idx].R!=0){				
				page_table[p_idx].R=0;				
				hand++;
				if(hand>frametable.size()-1)hand=0;
				p_idx=frame_table->at(hand);
			}	
			int tmp=hand;
			hand++;				
			if(hand>frametable.size()-1)hand=0;	
			return tmp;
		}
	}
};

class Clock_p : public Pager{
	
	int next_idx(vector<int> existPage, int hand_idx){
		for(int i=0; i<existPage.size(); i++){
			if (hand_idx==existPage[i] && i==existPage.size()-1){return existPage[0];}
			if (hand_idx==existPage[i])return existPage[i+1];
		}		
		return existPage[0];
	}
	int next_init(vector<int> existPage, int hand_idx){
		for(int i=0; i<existPage.size(); i++){
			if (hand_idx==existPage[i]){return existPage[i];}						
		}
		for(int i=0; i<existPage.size(); i++){
			if (hand_idx>existPage[i] && i==existPage.size()-1 ){return existPage[0];}
			else if(hand_idx>existPage[i] && hand_idx<existPage[i+1]){return existPage[i+1];}			
		}					
		return existPage[0];
	}
	
	int Change (bit* page_table, vector<int>* frame_table){
		int index=frameFull();		
		if (index !=-1){
			FIFO_dq.push_back(index);
			return index;
		}	
		else{			
			vector<int> existP=check_existP(page_table, frame_table);						
			hand=next_init(existP, hand);
			if(page_table[hand].R==0){
				int tmp2=hand; hand++;
				return page_table[tmp2].idx; 
			}
			while (page_table[hand].R!=0){
				page_table[hand].R=0;				
				hand=next_idx(existP, hand);			
			}
			int tmp=hand; hand++;			
			return page_table[tmp].idx;
		}
	}
};

class LRU : public Pager{
	int Change (bit* page_table, vector<int>* frame_table){
		int index=frameFull();		
		if (index !=-1){
			LRU_dq.push_back(index);
			return index;
		}	
		else{
			int new_index= LRU_dq[0];			
			LRU_dq.pop_front();
			LRU_dq.push_back(new_index);
			return new_index;			
		}
	}
};

class NRU : public Pager{
	void unRef(bit* page_table){
		for (int i=0; i<num_pages; i++){
			if (page_table[i].P==1){
				page_table[i].R=0;
			}
		}
	}
	int lowest_Pidx(bit* page_table, int index){		
		int closest_idx=index;
		while(page_table[index].P!=1){
			index--;
			closest_idx=index;
		}
		return closest_idx;
	}
	vector<int> findLowestClass(bit* page_table){
		vector<int> R0M0_v, R0M1_v, R1M0_v, R1M1_v;
		for (int i=0; i<num_pages; i++){
			if (page_table[i].P==1 && page_table[i].R==0 && page_table[i].M==0){ R0M0_v.push_back(i);}
			else if (page_table[i].P==1 && page_table[i].R==0 && page_table[i].M==1){ R0M1_v.push_back(i);}
			else if (page_table[i].P==1 && page_table[i].R==1 && page_table[i].M==0){ R1M0_v.push_back(i);}
			else if (page_table[i].P==1 && page_table[i].R==1 && page_table[i].M==1){ R1M1_v.push_back(i);}			
		}
		if (R0M0_v.size()>0) return R0M0_v;
		else if (R0M1_v.size()>0) return R0M1_v;
		else if (R1M0_v.size()>0) return R1M0_v;
		else return R1M1_v;
	}	
	int Change (bit* page_table, vector<int>* frame_table){
		int page_idx;
		int index=frameFull();		
		if (index !=-1){
			LRU_dq.push_back(index);
			return index;
		}	
		else{			
			vector<int> lowest_class=findLowestClass(page_table);			
			index= myrandom(lowest_class.size(), ofs)-1;			
			page_idx=lowest_class[index];			
			if (unMap_count==10){unRef(page_table);unMap_count=0;}			
			unMap_count++;
			return page_table[page_idx].idx;
		}
	}
};
class Aging_F : public Pager{
	int findSmallIdx(vector<uint32_t> vector){
		int idx=0, allZero=0;
		long value=4294967296;
		for(int i =0; i<vector.size(); i++){		
			if(vector[i]==0){allZero++;}	
		}		
		if (allZero!=vector.size()){
			for(int i =0; i<vector.size(); i++){
				if(vector[i]!=0 && vector[i]<value){value=vector[i]; idx=i;}	
			}
		}		
		return idx;
	}	
	void shiftFrame(){
		for(int i =0; i<AgingF_v.size(); i++){
			AgingF_v[i]>>=1;
		}
	}
	vector<int> readRbit(vector<uint32_t> vectorin){
		vector<int> Rvector;
		for (int i=0; i<vectorin.size(); i++){
			if ((vectorin[i] & 2147483648) == 2147483648){Rvector.push_back(1);}
			else Rvector.push_back(0);
		}
		return Rvector;
	}
	void AddRbit(bit* page_table, vector<int>* frame_table){
		for(int i=0; i<frame_table->size(); i++){
			int page_index=frame_table->at(i);
			// if(page_table[page_index].R==1){AgingF_v[i]|=80000000; page_table[page_index].R=0;}
			if(page_table[page_index].R==1){AgingF_v[i]|=2147483648; page_table[page_index].R=0;}
		}
	}

	int Change (bit* page_table, vector<int>* frame_table){		
		int index=frameFull();		
		if (index !=-1){
			AgingF_v.push_back(0);
			return index;
		}	
		else{						
			shiftFrame();
			AddRbit(page_table, frame_table);			
			int new_index=findSmallIdx(AgingF_v);
			AgingF_v[new_index]=0;
			return new_index;			
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
		workon_F->at(frame_index)=-1;
		string tmp="";
		if(O_flag==1)printf("%d: ZERO %4s %3d\n", inst ,tmp.c_str(), frame_index);	
		Z_count++;
	} 

	//// remove idx at page table, remove present R bit. I did not remove M bit ////
	void Unmap(int page_index, int frame_index, int inst){				
		int index_to_change=workon_F->at(frame_index);
		workon_P[index_to_change].P=0;
		workon_P[index_to_change].R=0; 
		// workon_P[index_to_change].M=0; 
		workon_P[index_to_change].idx=0; 
		if(O_flag==1)printf("%d: UNMAP %3d %3d\n", inst ,workon_F->at(frame_index), frame_index);
		U_count++;
	}
	//// add S bit ////
	void Pageout(int rw, int page_index, int frame_index, int inst){
		int index_to_change=workon_F->at(frame_index);
		workon_P[index_to_change].S=1;
		if(O_flag==1)printf("%d: OUT %5d %3d\n", inst ,workon_F->at(frame_index), frame_index);			
		O_count++;
	}
	void Pagein(int rw, int page_index, int frame_index, int inst){				
		workon_P[page_index].R=1;		
		workon_P[page_index].idx=frame_index;
		if (rw==1){workon_P[page_index].M=1;}else{workon_P[page_index].M=0;}		
		if(O_flag==1)printf("%d: IN %6d %3d\n", inst ,page_index, frame_index);					
		I_count++;
	}	

	//// at P and R bit, add index to frame, add M bit depends on rw, then print ////
	void Map(int rw, int page_index, int frame_index, int inst){
		workon_P[page_index].R=1;		
		workon_P[page_index].idx=frame_index;
		workon_F->at(frame_index)=page_index; // assign page index to frame
		workon_P[page_index].P=1;
		if (rw==1){workon_P[page_index].M=1;}else{workon_P[page_index].M=0;}		
		if(O_flag==1)printf("%d: MAP %5d %3d\n", inst ,page_index, frame_index);					
		M_count++;
	}
	
	//// just add R & M bit ////
	void Modify (int rw, int page_index, int inst){
		workon_P[page_index].R=1;workon_P[page_index].M=1;		
	}
	//// add R bit, and reorder LRU_dq for l method ////
	void Reference (int rw, int page_index, int inst){
		workon_P[page_index].R=1;	
		if(algo=="l"){
			int frame_idx=workon_P[page_index].idx;
			int idx_of_frame_idx=-1;
			for (int i=0; i< LRU_dq.size(); i++){
				if (frame_idx==LRU_dq[i])idx_of_frame_idx=i;			
			}
			int element= LRU_dq[idx_of_frame_idx];			
			LRU_dq.erase(LRU_dq.begin()+idx_of_frame_idx);
			LRU_dq.push_back(element);
		}		
	}
	
	Pager* pager;
	bit* workon_P;
	vector<int>* workon_F;
	void Process(){				
		for (int task_idx=0; task_idx<tasks.size(); task_idx++){
			if(O_flag==1)cout<<"==> inst: "<<tasks[task_idx][0]<<" "<<tasks[task_idx][1]<<endl;						
			int rw=tasks[task_idx][0];
			int page_index=tasks[task_idx][1];
			if (workon_P[page_index].P==0){
				int full=pager->frameFull();
				if(full==-1){
					if (workon_P[page_index].S==1){
						if (R_flag==1)cout<<"condition1_1"<<endl;
						int frame_index = pager->Change(workon_P, workon_F);	
						Unmap(page_index,frame_index, task_idx);							
						if (workon_P[workon_F->at(frame_index)].M == 1){ Pageout (rw, page_index, frame_index, task_idx);}
						Pagein(rw, page_index, frame_index, task_idx);
						Map(rw, page_index, frame_index, task_idx);											
						RW_count++;
					}
					else if(workon_P[page_index].S==0){
						if (R_flag==1)cout<<"condition1_2"<<endl;
						int frame_index = pager->Change(workon_P, workon_F);	
						Unmap(page_index,frame_index, task_idx);							
						if (workon_P[workon_F->at(frame_index)].M == 1){ Pageout (rw, page_index, frame_index, task_idx);}
						Zero(rw, page_index, frame_index, task_idx);
						Map(rw, page_index, frame_index, task_idx);											
						RW_count++;
					}
				}
				else if(full!=-1){
					if (R_flag==1)cout<<"condition1_3"<<endl;
					int frame_index = pager->Change(workon_P, workon_F);	
					// cout<<"frame_index:"<<frame_index<<endl;
					Zero(rw, page_index, frame_index, task_idx);
					Map(rw, page_index, frame_index, task_idx);							
					RW_count++;
				}
			}
			
			else if (workon_P[page_index].P==1){
				int full=pager->frameFull();
				if(rw==1){
					if (R_flag==1)cout<<"condition2_1"<<endl;
					Reference(rw, page_index, task_idx);
					Modify(rw, page_index, task_idx);					
					RW_count++;
				}
				else if (rw==0){
					if (R_flag==1)cout<<"condition2_2"<<endl;
					Reference(rw, page_index, task_idx);
					RW_count++;
				}				
			}
			printp(p_flag, workon_P, workon_F);	
			printf(f_flag, workon_P, workon_F);	
			// cout<<"LRU_dq:";
			// for(int i=0; i<LRU_dq.size(); i++){
			// 	cout<<LRU_dq[i]<<" ";
			// }
			// cout<<endl;
		}		
	}

	void printReport(){
		for(int i=0; i<printOrder_v.size(); i++){
			if (printOrder_v[i]=='P')printp(1, workon_P, workon_F);
			else if (printOrder_v[i]=='F')printf_final(1, workon_P, workon_F);
			else if (printOrder_v[i]=='S'){
				uint64_t task_totle=tasks.size();
				uint64_t SUM=(M_count+U_count)*400+(I_count+O_count)*3000+Z_count*150+RW_count*1;
				cout<<"SUM "<<task_totle<<" U="<<U_count<<" M="<<M_count<<" I="<<I_count<<" O="<<O_count<<" Z="<<Z_count<<" ===> "<<SUM<<endl;
			}
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
	// cout<<"algo="<<algo<<" ovalue="<<ovalue<<" num_frames="<<num_frames<<endl;
	
	if (algo=="r") {mmu.pager = new Random;}
	if (algo=="f") {mmu.pager = new FIFO;}
	if (algo=="s") {mmu.pager = new SecondChance;}
	if (algo=="c") {mmu.pager = new Clock_f;}
	if (algo=="X") {mmu.pager = new Clock_p;}
	if (algo=="l") {mmu.pager = new LRU;}
	if (algo=="N") {mmu.pager = new NRU;}
	if (algo=="a") {
		mmu.pager = new Aging_F;
		// for (int i =0; i<num_pages; i++){AgingP_v.push_back(0);		}
	}

	mmu.frametable_init(num_frames);
	bit page_table[64] = { };
	mmu.workon_P=page_table;
	mmu.workon_F=&frametable;
	input_init(argv[argc-2]);	
	rfile_init(argv[argc-1]);			
	
	mmu.Process();
	mmu.printReport();
	
	// for (int i=0; i<20002; i++){
	// 	int t = myrandom(num_frames, ofs);	
	// 	cout<<"ofs:"<<ofs<<" "<<t<<endl;
	// }


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



















