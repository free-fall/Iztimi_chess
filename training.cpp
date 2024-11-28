#pragma GCC optimize(2,3,"Ofast","inline","-ffast-math")
#pragma GCC target("avx,sse2,sse3,sse4,mmx")

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <time.h>

#include "chess.h"
#include "Iztimi.h"

using namespace std;

int cnt=0;

double hours=10;

int main(){
	int start_time=clock();
	while(clock()-start_time<hours*3600000){
		ChessBoard board;
		Iztimi.Selection(board);
		
		cout<<"have searched "<<++cnt<<" times"<<endl;
	}
	return 0;
}
