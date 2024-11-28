#ifndef IZTIMI_H
#define IZTIMI_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <math.h>
#include <algorithm>
#include <time.h>

#include "chess.h"
#include "Robot.h"
#include "DataStore.h"

using namespace std;

struct Create_Iztimi{
	const double UCBC=1.44;
	const double INF=1e9;
	
	DataStore total_award=DataStore("total_award");
	DataStore total_visit=DataStore("total_visit");
	
	Create_Robot robot4=Create_Robot(4);
	Create_Robot robot5=Create_Robot(5);
	
	Create_Iztimi(){
		srand(time(0));
	}
	
	bool Explored(ChessBoard now){
		if(total_visit.Query(now.fen()))return true;
		return false;
	}
	
	double UCB(double v,double n,double N){
		return v/n+UCBC*sqrt(log(N)/n);
	}
	
	double Simulation(ChessBoard now){
		cout<<"Simulation :"<<endl;
		now.print();
		cout<<endl;
		
//		double res;
//		cin>>res;
//		return res;
		
		
		if(now.Game_over()){
			if(now.Checkmate()){
				if(now.turn)return 0;
				return 1;
			}
			return 0.5;
		}
		
		vector<Move> legel_moves=now.Get_all_legal_moves();
		random_shuffle(legel_moves.begin(),legel_moves.end());
		
		ChessBoard to=ChessBoard(now);
		
//		to.execute(legel_moves.back());
		to.execute(robot4.Get_move(now));
		
		return Simulation(to);
	}
	
	void Backpropagation(string fen,double res,bool turn){
		total_award.Update(fen,total_award.Query(fen)+res);
		total_visit.Update(fen,total_visit.Query(fen)+1);
		return;
	}
	
	double Selection(ChessBoard now){
		string now_fen=now.fen();
		
		now.print();
		cout<<total_award.Query(now_fen)<<" "<<total_visit.Query(now_fen)<<endl<<endl;
		
		if(now.Game_over()){
			if(now.Checkmate()){
				if(now.turn)return 0;
				return 1;
			}
			return 0.5;
		}
		
		double best_score=-INF,res=0;
		Move best_move;
		
		vector<Move> legel_moves=now.Get_all_legal_moves();
		random_shuffle(legel_moves.begin(),legel_moves.end());
		
		for(Move move:legel_moves){
			ChessBoard to=ChessBoard(now);
			to.execute(move);
			string to_fen=to.fen();
			
			if(!Explored(to)){
				res=Simulation(to);
				
				Backpropagation(to_fen,res,to.turn);
				Backpropagation(now_fen,res,now.turn);
				return res;
			}
			
			double val=total_award.Query(to_fen),tmp;
			if(!now.turn)val=total_visit.Query(to_fen)-val;
			tmp=UCB(val,total_visit.Query(to_fen),total_visit.Query(now_fen));
			
			if(tmp>best_score){
				best_score=tmp;
				best_move=move;
			}
		}
		
		ChessBoard to=ChessBoard(now);
		to.execute(best_move);
		string to_fen=to.fen();
		
		res=Selection(to);
		Backpropagation(now_fen,res,now.turn);
		return res;
	}
	
	Move Get_move(ChessBoard now){
		if(Explored(now)){
			double best_score=-INF;
			Move best_move;
			
			vector<Move> legel_moves=now.Get_all_legal_moves();
			random_shuffle(legel_moves.begin(),legel_moves.end());
			
			for(Move move:legel_moves){
				ChessBoard to=ChessBoard(now);
				to.execute(move);
				string to_fen=to.fen();
				
				if(total_visit.Query(to_fen)>best_score){
					best_score=total_visit.Query(to_fen);
					best_move=move;
				}
			}
			return best_move;
		}
		return robot5.Get_move(now);
	}
}Iztimi;

#endif // IZTIMI_H
