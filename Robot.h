#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <time.h>

#include "chess.h"

using namespace std;

const int INF=0x3f3f3f3f;

struct Create_Robot{
	int DEPTH;
	
	unordered_map<string,int> alpha,beta;
	Move best_move;
	
	int location_value[8][8]={
		{0,0,0,0,0,0,0,0},
		{0,1,1,1,1,1,1,0},
		{0,1,2,2,2,2,1,0},
		{0,1,2,3,3,2,1,0},
		{0,1,2,3,3,2,1,0},
		{0,1,2,2,2,2,1,0},
		{0,1,1,1,1,1,1,0},
		{0,0,0,0,0,0,0,0}
	};
	
	Create_Robot(int _DEPTH=5){
		DEPTH=_DEPTH;
		srand(time(0));
		return;
	}
	
	int Evaluate(ChessBoard board){
		if(board.Game_over()){
			if(board.Checkmate()){
				if(board.turn)return -INF;
				else return INF;
			}
			else return 0;
		}
		
		int res=0;
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				int op=0;
				if(board.board[i][j].color)op=1;
				else op=-1;
				res+=location_value[i][j]*op;
				if(board.board[i][j].Type()==1){
					res+=100*op;
					if(op==1){
						if(j==4)res+=10;
						if(j==5)res+=100;
						if(j==6)res+=200;
					}
					if(op==-1){
						if(j==3)res-=10;
						if(j==2)res-=100;
						if(j==1)res-=200;
					}
				}
				if(board.board[i][j].Type()==2)res+=300*op;
				if(board.board[i][j].Type()==3)res+=300*op;
				if(board.board[i][j].Type()==4)res+=500*op;
				if(board.board[i][j].Type()==5){
					res+=900*op;
					if(op==1&&board.fullmove_number<=5&&!(i==3&&j==0))res-=200;
					if(op==-1&&board.fullmove_number<=5&&!(i==3&&j==7))res+=200;
				}
				if(board.board[i][j].Type()==6){
					res+=100000*op;
					if(op==1&&board.fullmove_number<=10&&!(i==4&&j==0))res-=50;
					if(op==-1&&board.fullmove_number<=10&&!(i==4&&j==7))res+=50;
				}
			}
		}
		return res;
	}
	
	void Minimax(ChessBoard now,int dep,bool is_root){
		string now_fen=now.fen();
		
		if(dep==DEPTH||now.Game_over()){
			alpha[now_fen]=beta[now_fen]=Evaluate(now);
			return;
		}
		
		vector<Move> legal_moves=now.Get_all_legal_moves();
		random_shuffle(legal_moves.begin(),legal_moves.end());
		
		for(Move move:legal_moves){
			ChessBoard to=ChessBoard(now);
			to.execute(move);
			string to_fen=to.fen();
			
			alpha[to_fen]=alpha[now_fen];
			beta[to_fen]=beta[now_fen];

			Minimax(to,dep+1,0);
			
			if(now.turn==true&&beta[to_fen]>alpha[now_fen]){
				alpha[now_fen]=beta[to_fen];
				if(is_root)best_move=move;
			}
			if(now.turn==false&&alpha[to_fen]<beta[now_fen]){
				beta[now_fen]=alpha[to_fen];
				if(is_root)best_move=move;
			}
			
			if(alpha[now_fen]>=beta[now_fen])break;
		}
		return;
	}
	
	Move Get_move(ChessBoard board){
		alpha.clear();
		beta.clear();
		
		alpha[board.fen()]=-INF;
		beta[board.fen()]=INF;
		
		best_move=board.Get_all_legal_moves().back();
		
		Minimax(board,0,1);
		
		return best_move;
	}
};

#endif // ROBOT_H
