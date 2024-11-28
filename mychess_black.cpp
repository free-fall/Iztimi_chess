#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

#include "chess.h"
#include "Iztimi.h"

using namespace std;

Move Get_move(ChessBoard board){
	vector<Move> legel_moves=board.Get_all_legal_moves();
	for(int i=0;i<legel_moves.size();i++){
		cout<<i<<". ";
		if(legel_moves[i].castling==0)cout<<"白方短易位"<<endl;
		else if(legel_moves[i].castling==1)cout<<"白方长易位"<<endl;
		else if(legel_moves[i].castling==2)cout<<"黑方短易位"<<endl;
		else if(legel_moves[i].castling==3)cout<<"黑方长易位"<<endl;
		else if(legel_moves[i].promotion){
			cout<<legel_moves[i].x<<" "<<legel_moves[i].y<<" "<<legel_moves[i].dx<<" "<<legel_moves[i].dy;
			cout<<"升变为：";
			if(legel_moves[i].promotion==2)cout<<"骑士"<<endl;
			if(legel_moves[i].promotion==3)cout<<"教皇"<<endl;
			if(legel_moves[i].promotion==4)cout<<"堡垒"<<endl;
			if(legel_moves[i].promotion==5)cout<<"皇后"<<endl;
		}
		else{
			cout<<legel_moves[i].x<<" "<<legel_moves[i].y<<" "<<legel_moves[i].dx<<" "<<legel_moves[i].dy<<endl;
		}
	}
	int x;
	cin>>x;
	return legel_moves[x];
}

int main(){
	ChessBoard board;
//	board.Lead_in("1R1bkr2/7p/1np2q2/2NpN3/3P4/8/2P1Q1PP/6K1 b - - 0 28");
	while(!board.Game_over()){
		system("cls");
		board.print();
		cout<<board.fen()<<endl;
		Move move;
		if(board.turn)move=Get_move(board);
		else move=Iztimi.Get_move(board);
		board.execute(move);
	}
	system("pause");
	return 0;
}
