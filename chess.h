#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int symbol_to_type[205];

string type_to_symbol;

struct ChessInitlize{
	ChessInitlize(){
		type_to_symbol=".pnbrqkPNBRQK";
		
		symbol_to_type['.']=0;
		symbol_to_type['p']=1;
		symbol_to_type['n']=2;
		symbol_to_type['b']=3;
		symbol_to_type['r']=4;
		symbol_to_type['q']=5;
		symbol_to_type['k']=6;
		symbol_to_type['P']=7;
		symbol_to_type['N']=8;
		symbol_to_type['B']=9;
		symbol_to_type['R']=10;
		symbol_to_type['Q']=11;
		symbol_to_type['K']=12;
		return;
	}
}__chess_initlize;

struct Piece{
	int piece_type,color;
	
	Piece(char c='.'){
		piece_type=0,color=-1;
		if(islower(c))color=0;
		if(isupper(c))color=1;
		piece_type=symbol_to_type[c];
		return;
	}
	
	bool operator<(const Piece &other)const{
		if(piece_type!=other.piece_type)return piece_type<other.piece_type;
		if(color!=other.color)return color<other.color;
		return false;
	}
	
	bool operator==(const Piece &other)const{
		if(piece_type!=other.piece_type)return false;
		if(color!=other.color)return false;
		return true;
	}
	
	bool operator!=(const Piece &other)const{
		return !(*this==other);
	}
	
	char Symbol(){
		return type_to_symbol[piece_type];
	}
	
	int Type(){
		if(piece_type<=6)return piece_type;
		return piece_type-6;
	}
};

struct Move{
//	当前坐标，移动向量 
	int x,y,dx,dy;
	
//	如果是王车移位 0~3 分别表示 KQkq，如果不是则为 -1 
	int castling;
	
//	如果是升变用编号表示升变的棋子，否则为 0 
	int promotion;
	
//	如果是吃过路兵为 true，否则为 false
	bool en_passant;
	
//	如果是白方走子为 true，否则为 false 
	bool side;
	
	Move(int _side=true,int _x=0,int _y=0,int _dx=0,int _dy=0){
		side=_side,x=_x,y=_y,dx=_dx,dy=_dy;
		en_passant=promotion=0;
		castling=-1;
		return;
	}
	
	bool operator<(const Move &other)const{
		if(x!=other.x)return x<other.x;
		if(y!=other.y)return y<other.y;
		if(dx!=other.dx)return dx<other.dx;
		if(dy!=other.dy)return dy<other.dy;
		if(castling!=other.castling)return castling<other.castling;
		if(promotion!=other.promotion)return promotion<other.promotion;
		if(en_passant!=other.en_passant)return en_passant<other.en_passant;
		if(side!=other.side)return side<other.side;
		return false;
	}
	
	bool operator==(const Move &other)const{
		if(x!=other.x)return false;
		if(y!=other.y)return false;
		if(dx!=other.dx)return false;
		if(dy!=other.dy)return false;
		if(castling!=other.castling)return false;
		if(promotion!=other.promotion)return false;
		if(en_passant!=other.en_passant)return false;
		if(side!=other.side)return false;
		return true;
	}
	
	bool operator!=(const Move &other)const{
		return !(*this==other);
	}
	
	void print(){
		cout<<x<<" "<<y<<" "<<dx<<" "<<dy<<" "<<castling<<" "<<promotion<<" "<<en_passant<<endl;
		return;
	}
};

struct ChessBoard{
//	半回合数 
	int halfmove_clock;
	
//	回合数 
	int fullmove_number;
	
//	true 为白，false 为黑 
	bool turn;
	
//	王车易位 KQkq
	bool castling_availability[4];
	
//	过路兵捕捉格 
	string en_passant_target_square;
	
	//第一维：a~h 第二维：1~8
	Piece board[8][8]; 
	
	ChessBoard(){
		init();
		return;
	}
	
	ChessBoard(const ChessBoard &other){
		turn=other.turn;
		halfmove_clock=other.halfmove_clock;
		fullmove_number=other.fullmove_number;
		en_passant_target_square=other.en_passant_target_square;
		
		for(int i=0;i<4;i++){
			castling_availability[i]=other.castling_availability[i];
		}
		
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				board[i][j]=other.board[i][j];
			}
		}
		return;
    }
    
    void init(){
    	halfmove_clock=0;
		fullmove_number=1;
		turn=true;
		for(int i=0;i<4;i++)castling_availability[i]=true;
		en_passant_target_square="-";
		
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				board[i][j]=Piece();
			}
		}
		
		for(int i=0;i<8;i++){
			board[i][6]=Piece('p');
			board[i][1]=Piece('P');
		}
		
		board[0][7]=board[7][7]=Piece('r');
		board[1][7]=board[6][7]=Piece('n');
		board[2][7]=board[5][7]=Piece('b');
		board[3][7]=Piece('q'),board[4][7]=Piece('k');
		
		board[0][0]=board[7][0]=Piece('R');
		board[1][0]=board[6][0]=Piece('N');
		board[2][0]=board[5][0]=Piece('B');
		board[3][0]=Piece('Q'),board[4][0]=Piece('K');
    	return;
	}
    
    bool operator<(const ChessBoard &other)const{
    	if(turn!=other.turn)return turn<other.turn;
    	if(halfmove_clock!=other.halfmove_clock)return halfmove_clock<other.halfmove_clock;
    	if(fullmove_number!=other.fullmove_number)return fullmove_number<other.fullmove_number;
    	if(en_passant_target_square!=other.en_passant_target_square)return en_passant_target_square<other.en_passant_target_square;
    	
    	for(int i=0;i<4;i++){
    		if(castling_availability[i]!=other.castling_availability[i])return castling_availability[i]<other.castling_availability[i];
		}
		
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				if(board[i][j]!=other.board[i][j])return board[i][j]<other.board[i][j];
			}
		}
		return false;
	}
	
	bool operator==(const ChessBoard &other)const{
		if(turn!=other.turn)return false;
    	if(halfmove_clock!=other.halfmove_clock)return false;
    	if(fullmove_number!=other.fullmove_number)return false;
    	if(en_passant_target_square!=other.en_passant_target_square)return false;
    	
    	for(int i=0;i<4;i++){
    		if(castling_availability[i]!=other.castling_availability[i])return false;
		}
		
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				if(board[i][j]!=other.board[i][j])return false;
			}
		}
		return true;
	}
	
	bool operator!=(const ChessBoard &other)const{
		return !(*this==other);
	}
	
	void Lead_in(string fen){
		int now=-1;
		
		for(int i=7;i>=0;i--){
			int j=-1;
			now++;
			while(fen[now]!='/'&&fen[now]!=' '){
				if(isdigit(fen[now])){
					for(int k=1;k<=fen[now]-'0';k++)board[++j][i]=Piece();
				}
				else{
					board[++j][i]=Piece(fen[now]);
				}
				now++;
			}
		}
		
		now++;
		if(fen[now]=='w')turn=true;
		if(fen[now]=='b')turn=false;
		
		now+=2;
		for(int i=0;i<4;i++)castling_availability[i]=false;
		while(fen[now]!=' '){
			if(fen[now]=='K')castling_availability[0]=true;
			if(fen[now]=='Q')castling_availability[1]=true;
			if(fen[now]=='k')castling_availability[2]=true;
			if(fen[now]=='q')castling_availability[3]=true;
			now++;
		}
		
		now++;
		en_passant_target_square="";
		while(fen[now]!=' '){
			en_passant_target_square+=fen[now];
			now++;
		}
		
		now++;
		halfmove_clock=0;
		while(fen[now]!=' '){
			halfmove_clock=halfmove_clock*10+fen[now]-'0';
			now++;
		}
		
		now++;
		fullmove_number=0;
		while(now<fen.size()){
			fullmove_number=fullmove_number*10+fen[now]-'0';
			now++;
		}
		return;
	}
	
	string fen(){
		string fen;
		
//		棋子位置数值区域
		for(int i=7;i>=0;i--){
			int cnt=0;
			for(int j=0;j<=7;j++){
				if(board[j][i].Symbol()=='.')cnt++;
				else{
					if(cnt)fen.push_back(cnt+'0');
					fen.push_back(board[j][i].Symbol());
					cnt=0;
				}
			}
			if(cnt)fen.push_back(cnt+'0');
			if(i)fen.push_back('/');
		}
		
//		轮走棋方
		fen.push_back(' ');
		if(turn)fen.push_back('w');
		else fen.push_back('b');
		
//		易位可行性
		fen.push_back(' ');
		string tmp="";
		if(castling_availability[0])tmp.push_back('K');
		if(castling_availability[1])tmp.push_back('Q');
		if(castling_availability[2])tmp.push_back('k');
		if(castling_availability[3])tmp.push_back('q');
		if(!tmp.size())fen.push_back('-');
		else fen+=tmp;
		
//		吃过路兵目标格
		fen.push_back(' ');
		fen+=en_passant_target_square;
		
//		半回合计数
		fen.push_back(' ');
		fen+=to_string(halfmove_clock);
		
//		回合数
		fen.push_back(' ');
		fen+=to_string(fullmove_number);
		return fen;
	}
	
	bool In_boundary(int x,int y){
		if(x<0||x>7||y<0||y>7)return false;
		return true;
	}
	
//	被将军返回 true，否则为 false 
	bool Check(ChessBoard now){
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				if(now.board[i][j].piece_type==0||now.board[i][j].color==now.turn)continue;
				
				if(now.board[i][j].Type()==1){
					int dx[2]={-1,1};
					int dy=now.board[i][j].color?1:-1;
					for(int k=0;k<2;k++){
						int x=i+dx[k],y=j+dy;
						if(!In_boundary(x,y))continue;
						if(now.board[i][j].color&&now.board[x][y].piece_type==6)return true;
						if(!now.board[i][j].color&&now.board[x][y].piece_type==12)return true;
					}
				}
				
				if(now.board[i][j].Type()==2){
					int dx[8]={ 1, 2, 2, 1,-1,-2,-2,-1};
					int dy[8]={ 2, 1,-1,-2,-2,-1, 1, 2};
					for(int k=0;k<8;k++){
						int x=i+dx[k],y=j+dy[k];
						if(!In_boundary(x,y))continue;
						if(now.board[i][j].color&&now.board[x][y].piece_type==6)return true;
						if(!now.board[i][j].color&&now.board[x][y].piece_type==12)return true;
					}
				}
				
				if(now.board[i][j].Type()==3){
					int dx[4]={ 1, 1,-1,-1};
					int dy[4]={ 1,-1,-1, 1};
					for(int k=0;k<4;k++){
						for(int t=1;t<8;t++){
							int x=i+dx[k]*t,y=j+dy[k]*t;
							if(!In_boundary(x,y))break;
							if(now.board[i][j].color&&now.board[x][y].piece_type==6)return true;
							if(!now.board[i][j].color&&now.board[x][y].piece_type==12)return true;
							if(now.board[x][y].piece_type)break;
						}
					}
				}
				
				if(now.board[i][j].Type()==4){
					int dx[4]={ 0, 1, 0,-1};
					int dy[4]={ 1, 0,-1, 0};
					for(int k=0;k<4;k++){
						for(int t=1;t<8;t++){
							int x=i+dx[k]*t,y=j+dy[k]*t;
							if(!In_boundary(x,y))break;
							if(now.board[i][j].color&&now.board[x][y].piece_type==6)return true;
							if(!now.board[i][j].color&&now.board[x][y].piece_type==12)return true;
							if(now.board[x][y].piece_type)break;
						}
					}
				}
				
				if(now.board[i][j].Type()==5){
					int dx[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
					int dy[8]={ 1, 1, 0,-1,-1,-1, 0, 1};
					for(int k=0;k<8;k++){
						for(int t=1;t<8;t++){
							int x=i+dx[k]*t,y=j+dy[k]*t;
							if(!In_boundary(x,y))break;
							if(now.board[i][j].color&&now.board[x][y].piece_type==6)return true;
							if(!now.board[i][j].color&&now.board[x][y].piece_type==12)return true;
							if(now.board[x][y].piece_type)break;
						}
					}
				}
				
				if(now.board[i][j].Type()==6){
					int dx[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
					int dy[8]={ 1, 1, 0,-1,-1,-1, 0, 1};
					for(int k=0;k<8;k++){
						int x=i+dx[k],y=j+dy[k];
						if(!In_boundary(x,y))continue;
						if(now.board[i][j].color&&now.board[x][y].piece_type==6)return true;
						if(!now.board[i][j].color&&now.board[x][y].piece_type==12)return true;
					}
				}
			}
		}
		return false;
	}
	
	void __execute(Move move){
		if(board[move.x][move.y].Type()==1||board[move.x+move.dx][move.y+move.dy].color==!turn)halfmove_clock=0;
		
		if(move.castling!=-1){
			if(move.castling==0){
				board[4][0]=Piece();
				board[5][0]=Piece('R');
				board[6][0]=Piece('K');
				board[7][0]=Piece();
				castling_availability[0]=castling_availability[1]=false;
			}
			
			if(move.castling==1){
				board[0][0]=Piece();
				board[2][0]=Piece('K');
				board[3][0]=Piece('R');
				board[4][0]=Piece();
				castling_availability[0]=castling_availability[1]=false;
			}
			
			if(move.castling==2){
				board[4][7]=Piece();
				board[5][7]=Piece('r');
				board[6][7]=Piece('k');
				board[7][7]=Piece();
				castling_availability[2]=castling_availability[3]=false;
			}
			
			if(move.castling==3){
				board[0][7]=Piece();
				board[2][7]=Piece('k');
				board[3][7]=Piece('r');
				board[4][7]=Piece();
				castling_availability[2]=castling_availability[3]=false;
			}
			return;
		}
		
		if(move.en_passant){
			board[move.x+move.dx][move.y+move.dy]=board[move.x][move.y];
			board[move.x][move.y]=Piece();
			board[move.x+move.dx][move.y]=Piece();
			return;
		}
		
		if(move.promotion){
			board[move.x][move.y]=Piece();
			if(move.side){
				if(move.promotion==2)board[move.x+move.dx][move.y+move.dy]=Piece('N');
				if(move.promotion==3)board[move.x+move.dx][move.y+move.dy]=Piece('B');
				if(move.promotion==4)board[move.x+move.dx][move.y+move.dy]=Piece('R');
				if(move.promotion==5)board[move.x+move.dx][move.y+move.dy]=Piece('Q');
			}
			else{
				if(move.promotion==2)board[move.x+move.dx][move.y+move.dy]=Piece('n');
				if(move.promotion==3)board[move.x+move.dx][move.y+move.dy]=Piece('b');
				if(move.promotion==4)board[move.x+move.dx][move.y+move.dy]=Piece('r');
				if(move.promotion==5)board[move.x+move.dx][move.y+move.dy]=Piece('q');
			}
			return;
		}
		
		if(board[move.x][move.y].piece_type==12)castling_availability[0]=castling_availability[1]=false;
		if(board[move.x][move.y].piece_type==6)castling_availability[2]=castling_availability[3]=false;
		
		if(move.x==0&&move.y==0&&castling_availability[1])castling_availability[1]=false;
		if(move.x==7&&move.y==0&&castling_availability[0])castling_availability[0]=false;
		if(move.x==0&&move.y==7&&castling_availability[3])castling_availability[3]=false;
		if(move.x==7&&move.y==7&&castling_availability[2])castling_availability[2]=false;
		
		board[move.x+move.dx][move.y+move.dy]=board[move.x][move.y];
		board[move.x][move.y]=Piece();
		return;
	}
	
	void execute(Move move){
		halfmove_clock++;
		if(!turn)fullmove_number++;
		
		en_passant_target_square="-";
    	if(board[move.x][move.y].Type()==1&&abs(move.dy)==2){
    		int xx=move.x+move.dx,yy=move.y+move.dy;
    		if(In_boundary(xx,yy)&&board[xx+1][yy].Type()==1&&board[xx+1][yy].color!=turn){
    			en_passant_target_square.clear();
    			en_passant_target_square.push_back('a'+xx);
    			en_passant_target_square.push_back('1'+move.y+move.dy/2);
			}
			if(In_boundary(xx,yy)&&board[xx-1][yy].Type()==1&&board[xx-1][yy].color!=turn){
    			en_passant_target_square.clear();
    			en_passant_target_square.push_back('a'+xx);
    			en_passant_target_square.push_back('1'+move.y+move.dy/2);
			}
		}
		
		__execute(move);
		
		turn=!turn;
		return;
	}
	
	bool Try_move(Move move){
		ChessBoard now=ChessBoard(*this);
		now.execute(move);
		now.turn=turn;
		if(Check(now))return false;
		return true;
	}
	
	vector<Move> Get_all_legal_moves(){
		vector<Move> legal_moves;
		
		for(int i=0;i<8;i++){
			for(int j=0;j<8;j++){
				if(board[i][j].piece_type==0||board[i][j].color!=turn)continue;
				
				if(board[i][j].Type()==1){
					int dx[2]={-1,1};
					int dy=turn?1:-1;
					
//					走子 
					for(int t=1;t<=2;t++){
						int x=i,y=j+dy*t;
						if(!In_boundary(x,y)||board[x][y].piece_type||t==2&&(turn&&j!=1||!turn&&j!=6))break;
						Move move=Move(turn,i,j,0,dy*t);
						
//						升变 
						if(turn&&y==7||!turn&&y==0){
							for(int p=2;p<=5;p++){
								move.promotion=p;
								if(Try_move(move))legal_moves.push_back(move);
							}
						}
						
//						普通走子 
						else if(Try_move(move))legal_moves.push_back(move);
					}
					
//					吃子 
					for(int k=0;k<2;k++){
						int x=i+dx[k],y=j+dy;
						if(!In_boundary(x,y)||board[x][y].color==turn)continue;
						
//						吃过路兵 
						if(board[x][y].color==-1&&x==en_passant_target_square[0]-'a'&&y==en_passant_target_square[1]-'1'){
							Move move=Move(turn,i,j,dx[k],dy);
							move.en_passant=true;
							
							if(Try_move(move))legal_moves.push_back(move);
						}
						
						if(board[x][y].color==!turn){
							Move move=Move(turn,i,j,dx[k],dy);
							
//							吃子升变 
							if(turn&&y==7||!turn&&y==0){
								for(int p=2;p<=5;p++){
									move.promotion=p;
									if(Try_move(move))legal_moves.push_back(move);
								}
							}
							
//							普通吃子 
							else if(Try_move(move))legal_moves.push_back(move);
						}
					}
				}
				
				if(board[i][j].Type()==2){
					int dx[8]={ 1, 2, 2, 1,-1,-2,-2,-1};
					int dy[8]={ 2, 1,-1,-2,-2,-1, 1, 2};
					for(int k=0;k<8;k++){
						int x=i+dx[k],y=j+dy[k];
						if(!In_boundary(x,y)||board[x][y].color==turn)continue;
						
						Move move=Move(turn,i,j,dx[k],dy[k]);
						if(Try_move(move))legal_moves.push_back(move);
					}
				}
				
				if(board[i][j].Type()==3){
					int dx[4]={ 1, 1,-1,-1};
					int dy[4]={ 1,-1,-1, 1};
					for(int k=0;k<4;k++){
						for(int t=1;t<8;t++){
							int x=i+dx[k]*t,y=j+dy[k]*t;
							if(!In_boundary(x,y)||board[x][y].color==turn)break;
							
							Move move=Move(turn,i,j,dx[k]*t,dy[k]*t);
							if(Try_move(move))legal_moves.push_back(move);
							
							if(board[x][y].piece_type)break;
						}
					}
				}
				
				if(board[i][j].Type()==4){
					int dx[4]={ 0, 1, 0,-1};
					int dy[4]={ 1, 0,-1, 0};
					for(int k=0;k<4;k++){
						for(int t=1;t<8;t++){
							int x=i+dx[k]*t,y=j+dy[k]*t;
							if(!In_boundary(x,y)||board[x][y].color==turn)break;
							
							Move move=Move(turn,i,j,dx[k]*t,dy[k]*t);
							if(Try_move(move))legal_moves.push_back(move);
							
							if(board[x][y].piece_type)break;
						}
					}
				}
				
				if(board[i][j].Type()==5){
					int dx[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
					int dy[8]={ 1, 1, 0,-1,-1,-1, 0, 1};
					for(int k=0;k<8;k++){
						for(int t=1;t<8;t++){
							int x=i+dx[k]*t,y=j+dy[k]*t;
							if(!In_boundary(x,y)||board[x][y].color==turn)break;
							
							Move move=Move(turn,i,j,dx[k]*t,dy[k]*t);
							if(Try_move(move))legal_moves.push_back(move);
							
							if(board[x][y].piece_type)break;
						}
					}
				}
				
				if(board[i][j].Type()==6){
					int dx[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
					int dy[8]={ 1, 1, 0,-1,-1,-1, 0, 1};
					for(int k=0;k<8;k++){
						int x=i+dx[k],y=j+dy[k];
						if(!In_boundary(x,y)||board[x][y].color==turn)continue;
						
						Move move=Move(turn,i,j,dx[k],dy[k]);
						if(Try_move(move))legal_moves.push_back(move);
					}
				}
			}
		}
		
//		王车易位 
		if(turn){
//			白方短易位 
			if(castling_availability[0]){
				Move move=Move(true,4,0,2,0);
				move.castling=0;
				
				if(!board[5][0].piece_type&&!board[6][0].piece_type){
					ChessBoard now=ChessBoard(*this);
					if(!Check(now)){
						now.board[4][0]=Piece();
						now.board[5][0]=Piece('K');
						if(!Check(now)&&Try_move(move))legal_moves.push_back(move);
					}
				}
			}
			
//			白方长易位 
			if(castling_availability[1]){
				Move move=Move(true,4,0,-2,0);
				move.castling=1;
				
				if(!board[1][0].piece_type&&!board[2][0].piece_type&&!board[3][0].piece_type){
					ChessBoard now=ChessBoard(*this);
					if(!Check(now)){
						now.board[4][0]=Piece();
						now.board[3][0]=Piece('K');
						if(!Check(now)){
							now.board[3][0]=Piece();
							now.board[2][0]=Piece('K');
							if(!Check(now)&&Try_move(move))legal_moves.push_back(move);
						}
					}
				}
			}
		}
		else{
//			黑方短易位 
			if(castling_availability[2]){
				Move move=Move(false,4,7,2,0);
				move.castling=2;
				
				if(!board[5][7].piece_type&&!board[6][7].piece_type){
					ChessBoard now=ChessBoard(*this);
					if(!Check(now)){
						now.board[4][7]=Piece();
						now.board[5][7]=Piece('k');
						if(!Check(now)&&Try_move(move))legal_moves.push_back(move);
					}
				}
			}
			
//			黑方长易位 
			if(castling_availability[3]){
				Move move=Move(false,4,7,-2,0);
				move.castling=3;
				
				if(!board[1][7].piece_type&&!board[2][7].piece_type&&!board[3][7].piece_type){
					ChessBoard now=ChessBoard(*this);
					if(!Check(now)){
						now.board[4][7]=Piece();
						now.board[3][7]=Piece('k');
						if(!Check(now)){
							now.board[3][7]=Piece();
							now.board[2][7]=Piece('k');
							if(!Check(now)&&Try_move(move))legal_moves.push_back(move);
						}
					}
				}
			}
		}
		return legal_moves;
	}
	
//	被将杀返回 true，否则为 false 
	bool Checkmate(){
		if(Check(ChessBoard(*this))&&Get_all_legal_moves().empty())return true;
		return false;
	}
	
	bool Game_over(){
		if(Get_all_legal_moves().empty()||fullmove_number>100)return true;
		return false;
	}
	
	void print(){
		for(int i=7;i>=0;i--){
			for(int j=0;j<=7;j++){
				cout<<board[j][i].Symbol()<<" ";
			}
			cout<<endl;
		}
		return;
	}
};

#endif // CHESS_H
