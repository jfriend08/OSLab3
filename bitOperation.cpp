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

struct bit {
		 unsigned int P:1;
		 unsigned int R:1;
		 unsigned int M:1;
		 unsigned int S:1;
		 unsigned int idx:6;		 
};

// class BitOPT{
// public:
// 	int checkPresentBit(int value){
// 		value<<=13; value>>=22;
// 		if (value==1) return 1;
// 		else return 0;
// 	}
// 	void add_P_Bit(packed_struct &value){value|=512;	}
// 	void add_R_Bit(packed_struct &value){value|=256;	}
// 	void add_M_Bit(packed_struct &value){value|=128;	}
// 	void add_S_Bit(packed_struct &value){value|=64;	}
// 	void addIndex(packed_struct &value, int Index){value|=Index;	}
// 	void add_PR_Bit(packed_struct &value){value|=512; value|=256;}
// 	void add_PRM_Bit(packed_struct &value){value|=512; value|=256;value|=128;}
// 	int getIndex(packed_struct &value){value<<=17; value>>=17; return value;}
// 	void rm_P_Bit(packed_struct &value){}
// };

// class MMU{
// public:
// 	packed_struct* Workon;
// 	// BitOPT bitopt;
// };

int main(){
	bit pagetable[65] = { };
	cout<<pagetable[0].idx<<endl;
	// mmu.Workon=pagetable;

	// cout<<"Present Bit:"<<mmu.bitopt.checkPresentBit(pagetable[0])<<endl;
	// mmu.bitopt.add_P_Bit(pagetable[0]);
	// cout<<pagetable[0]<<endl;
	// mmu.bitopt.addIndex(pagetable[0], 4);
	// cout<<pagetable[0]<<endl;
	// cout<<mmu.bitopt.getIndex(pagetable[0])<<endl;
	
}




