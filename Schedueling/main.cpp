#include "bb.h"

int main(){
	float o[JOBN][MACHN] = {{2 , 1, 3},{1, 5, 2}, {4, 1, 2}};// o[job][machine
	// @parameter: 
	//	o[][]: pij; 
	//	int k : for k power problem; 
	//	bool b : if true, for makespan problem, otherwise for k power
	MaxLoading run = MaxLoading(o,3,false);
	run.bbRun();
	run.osrslt().print();
	cout<< run.get_result();
	getchar();
	return 0;
}
