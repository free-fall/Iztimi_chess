#pragma GCC optimize(2,3,"Ofast","inline","-ffast-math")
#pragma GCC target("avx,sse2,sse3,sse4,mmx")

#include <iostream>
#include <fstream>
#include <thread>
#include <map>
#include <windows.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "chess.h"
#include "Iztimi.h"

using namespace std;

const int FPS=50;
const string INITIAL_FEN="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

bool running=true;

//	在下面那一侧的颜色,true 为白方，false为黑方 
bool side;
	
bool left_mouse_down,left_mouse_click;

bool need_flip;

//true 时不能输入，false 时可以 
bool input_lock=true;

int mouse_x,mouse_y;

string input_text;

string file_path;

ifstream fin;

ofstream fout;

mt19937 mt(time(0));

SDL_Window* window;

SDL_Renderer* renderer;

TTF_Font* font;

SDL_Rect position[8][8],promotion_position[4];

SDL_Rect history_position[5],title_rect;

SDL_Rect comfirm_position,new_position,page_up_position,page_down_position;

SDL_Rect turn_position;

map<string,SDL_Texture*> texture;

int rand(int l,int r){
	return mt()%(r-l+1)+l;
}

void Pretreatment(){
	if(TTF_Init()==-1){
        cerr<<"无法初始化TTF: "<<TTF_GetError()<<endl;
        exit(0);
    }
	
	window=SDL_CreateWindow("mychess",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,800,550,0);
    if (!window){
        cerr<<"无法创建窗口: "<<SDL_GetError()<<endl;
        SDL_Quit();
        exit(0);
    }
    
    SDL_Surface* icon=SDL_LoadBMP("images/icon.bmp");
    if(icon){
        SDL_SetWindowIcon(window,icon);
        SDL_FreeSurface(icon);
    }
	else cerr<<"无法载入图标: "<<SDL_GetError()<<endl;
	
	renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
	
	if(!renderer){
		cerr<<"无法创建渲染器: "<<SDL_GetError()<<endl;
    	SDL_DestroyWindow(window);
        SDL_Quit();
		exit(0);
	}
	
	font=TTF_OpenFont("font/msyh.ttf",36);
    if(!font){
        cerr<<"无法加载字体: "<<TTF_GetError()<<endl;
        exit(0);
    }
	
    if(IMG_Init(IMG_INIT_PNG)==0){
        cerr<<"无法初始化IMG: "<<IMG_GetError()<<endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(0);
    }
	
	texture["title"]=IMG_LoadTexture(renderer,"images/title.png");
	
	texture["bar1"]=IMG_LoadTexture(renderer,"images/bar1.png");
	texture["bar1_choosed"]=IMG_LoadTexture(renderer,"images/bar1_choosed.png");
	texture["bar2"]=IMG_LoadTexture(renderer,"images/bar2.png");
	texture["bar2_choosed"]=IMG_LoadTexture(renderer,"images/bar2_choosed.png");
	
	texture["page_up"]=IMG_LoadTexture(renderer,"images/page_up.png");
	texture["page_up_choosed"]=IMG_LoadTexture(renderer,"images/page_up_choosed.png");
	texture["page_down"]=IMG_LoadTexture(renderer,"images/page_down.png");
	texture["page_down_choosed"]=IMG_LoadTexture(renderer,"images/page_down_choosed.png");
	
	texture["p"]=IMG_LoadTexture(renderer,"images/black_pawn.png");
	texture["n"]=IMG_LoadTexture(renderer,"images/black_knight.png");
	texture["b"]=IMG_LoadTexture(renderer,"images/black_bishop.png");
	texture["r"]=IMG_LoadTexture(renderer,"images/black_rook.png");
	texture["q"]=IMG_LoadTexture(renderer,"images/black_queen.png");
	texture["k"]=IMG_LoadTexture(renderer,"images/black_king.png");
	texture["P"]=IMG_LoadTexture(renderer,"images/white_pawn.png");
	texture["N"]=IMG_LoadTexture(renderer,"images/white_knight.png");
	texture["B"]=IMG_LoadTexture(renderer,"images/white_bishop.png");
	texture["R"]=IMG_LoadTexture(renderer,"images/white_rook.png");
	texture["Q"]=IMG_LoadTexture(renderer,"images/white_queen.png");
	texture["K"]=IMG_LoadTexture(renderer,"images/white_king.png");
	
	texture["white_square"]=IMG_LoadTexture(renderer,"images/white_square.png");
	texture["black_square"]=IMG_LoadTexture(renderer,"images/black_square.png");
	
	texture["choose_white"]=IMG_LoadTexture(renderer,"images/choose_white.png");
	texture["choose_black"]=IMG_LoadTexture(renderer,"images/choose_black.png");
	texture["choose_side"]=IMG_LoadTexture(renderer,"images/choose_side.png");
	
	texture["reachable_point"]=IMG_LoadTexture(renderer,"images/reachable_point.png");
	texture["attack"]=IMG_LoadTexture(renderer,"images/attack.png");
	
	for(auto x:texture){
		if(!x.second){
	        cerr<<"无法加载图片: "<<IMG_GetError()<<endl;
	        IMG_Quit();
	        SDL_DestroyRenderer(renderer);
	        SDL_DestroyWindow(window);
	        SDL_Quit();
	        exit(0);
	    }
	}
	
    SDL_SetTextureAlphaMod(texture["reachable_point"],128);
		
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			position[i][j]={68+i*64,467-j*64,64,64};
		}
	}
	
	for(int i=0;i<4;i++){
		promotion_position[i]={2,19+i*64,64,64};
	}
	
	for(int i=0;i<5;i++){
		history_position[i]={100,200+i*55,600,50};
	}
	
	title_rect={80,40,640,128};
	
	comfirm_position={150,475,250,50};
	
	new_position={400,475,250,50};
	
	page_up_position={100,475,50,50};
	
	page_down_position={650,475,50,50};
	
	turn_position={588,19,40,40};
	return;
}

SDL_Texture* Make_text(string text,SDL_Color textColor){
	SDL_Surface* textSurface=TTF_RenderText_Solid(font,text.c_str(),textColor);
    if(!textSurface){
        cerr<<"无法创建文本表面: "<<TTF_GetError()<<endl;
        exit(0);
    }
    SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,textSurface);
    SDL_FreeSurface(textSurface);
    return texture;
}

void Render_text(SDL_Texture* texture,int x=0,int y=0,int w=-1,int h=-1){
	int textw=0,texth=0;
    SDL_QueryTexture(texture,NULL,NULL,&textw,&texth);
    
    SDL_Rect dstrect={x,y,w,h};
    if(w==-1)dstrect.w=textw;
    if(h==-1)dstrect.h=texth;
    
    SDL_RenderCopy(renderer,texture,NULL,&dstrect);
	return;
}
	
void Draw_board(ChessBoard board){
    SDL_RenderClear(renderer);
    
    if(board.turn){
    	SDL_RenderCopy(renderer,texture["K"],NULL,&turn_position);
    	Render_text(Make_text("white turn",{255,255,255}),630,19,160,40);
	}
    else{
		SDL_RenderCopy(renderer,texture["k"],NULL,&turn_position);
		Render_text(Make_text("black turn",{0,0,0}),630,19,160,40);
	}
    
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			if((i+j)%2)SDL_RenderCopy(renderer,texture["black_square"],NULL,&position[i][j]);
			else SDL_RenderCopy(renderer,texture["white_square"],NULL,&position[i][j]);
		}
	}
	
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			if(board.board[i][j].piece_type){
				string piece;
				piece.push_back(board.board[i][j].Symbol());
				if(side)SDL_RenderCopy(renderer,texture[piece],NULL,&position[i][j]);
				else SDL_RenderCopy(renderer,texture[piece],NULL,&position[7-i][7-j]);
			}
		}
	}
	return;
}

bool In_rect(int x,int y,SDL_Rect rect){
	return rect.x<=x&&x<=rect.x+rect.w&&rect.y<=y&&y<=rect.y+rect.h;
}

struct Player{
//	true 为电脑，false 为玩家 
	bool is_computer;
	
	Player(bool _is_computer=false){
		is_computer=_is_computer;
	}
	
	void Get_mouse_position(int &x,int &y){
		if(mouse_x<68||mouse_x>580||mouse_y<19||mouse_y>531){
			x=y=-1;
			return;
		}
		x=(mouse_x-68)/64;
		y=(531-mouse_y)/64;
		if(!side)x=7-x,y=7-y;
		return;
	}
	
	Move Get_move(ChessBoard board){
		if(is_computer){
			if(board.turn)Render_text(Make_text("Iztimi is thinking ...",{255,255,255}),588,60,200,30);
			else Render_text(Make_text("Iztimi is thinking ...",{0,0,0}),588,60,200,30);
			
			SDL_RenderPresent(renderer);
			
			return Iztimi.Get_move(board);
		}
		
		bool is_choosed=false;
		int lift_x=-1,lift_y=-1;
		Move choosed_move;
		vector<Move> possible_moves,legal_moves;
		legal_moves=board.Get_all_legal_moves();
		while(running&&!is_choosed){
        	if(left_mouse_click){
        		for(Move move:possible_moves){
        			int x=move.x+move.dx,y=move.y+move.dy;
        			if(!side)x=7-x,y=7-y;
        			if(In_rect(mouse_x,mouse_y,position[x][y])){
        				is_choosed=true;
        				choosed_move=move;
        				break;
					}
				}
				
				if(!is_choosed){
	        		int x,y;
	        		Get_mouse_position(x,y);
	        		
					Piece piece;
					if(x!=-1&&y!=-1)piece=board.board[x][y];
					if(piece.Type()&&piece.color==board.turn){
						lift_x=x,lift_y=y;
						possible_moves.clear();
						for(Move move:legal_moves){
							if(move.x==lift_x&&move.y==lift_y)possible_moves.push_back(move);
						}
						need_flip=true;
					}
					else if(lift_x!=-1&&lift_y!=-1){
						lift_x=lift_y=-1;
						possible_moves.clear();
						need_flip=true;
					}
				}
			}
			
			if(lift_x!=-1&&lift_y!=-1){
				Draw_board(board);
				for(Move move:possible_moves){
					int x=move.x+move.dx,y=move.y+move.dy;
					Piece piece=board.board[x][y];
					if(!side)x=7-x,y=7-y;
					if(piece.Type())SDL_RenderCopy(renderer,texture["attack"],NULL,&position[x][y]);
					else SDL_RenderCopy(renderer,texture["reachable_point"],NULL,&position[x][y]);
				}
			}
			
			if(need_flip){
				need_flip=false;
				SDL_RenderPresent(renderer);
			}
			
        	SDL_Delay(1000/FPS);
		}
		
		if(choosed_move.promotion){
			is_choosed=false;
			
			Draw_board(board);
			if(board.turn){
				SDL_RenderCopy(renderer,texture["Q"],NULL,&promotion_position[0]);
				SDL_RenderCopy(renderer,texture["R"],NULL,&promotion_position[1]);
				SDL_RenderCopy(renderer,texture["B"],NULL,&promotion_position[2]);
				SDL_RenderCopy(renderer,texture["N"],NULL,&promotion_position[3]);
			}
			else{
				SDL_RenderCopy(renderer,texture["q"],NULL,&promotion_position[0]);
				SDL_RenderCopy(renderer,texture["r"],NULL,&promotion_position[1]);
				SDL_RenderCopy(renderer,texture["b"],NULL,&promotion_position[2]);
				SDL_RenderCopy(renderer,texture["n"],NULL,&promotion_position[3]);
			}
			SDL_RenderPresent(renderer);
			
			while(running&&!is_choosed){
				if(left_mouse_click){
					if(In_rect(mouse_x,mouse_y,promotion_position[0])){
						choosed_move.promotion=5;
						is_choosed=true;
					}
					
					if(In_rect(mouse_x,mouse_y,promotion_position[1])){
						choosed_move.promotion=4;
						is_choosed=true;
					}
					
					if(In_rect(mouse_x,mouse_y,promotion_position[2])){
						choosed_move.promotion=3;
						is_choosed=true;
					}
					
					if(In_rect(mouse_x,mouse_y,promotion_position[3])){
						choosed_move.promotion=2;
						is_choosed=true;
					}
				}
				
				SDL_Delay(1000/FPS);
			}
		}
		return choosed_move;
	}
};

struct ChessGame{
	ChessBoard board;
	
	Player white_player,black_player;
	
	ChessGame(string fen=INITIAL_FEN,bool white_is_computer=false,bool black_is_computer=false){
		board.Lead_in(fen);
		white_player=Player(white_is_computer);
		black_player=Player(black_is_computer);
	}
	
	void Move_animation(Move move,int tim=250){
		int x=move.x,y=move.y;
		double addx=1.0*move.dx*64*1000/(tim*FPS),addy=-1.0*move.dy*64*1000/(tim*FPS);
		
		SDL_Rect now_rect;
		now_rect.w=64;
		now_rect.h=64;
		
		if(!side){
			addx=-addx,addy=-addy;
			x=7-x,y=7-y;
		}
		
		x=68+x*64;
		y=467-y*64;
		
		for(int _=1;_<=tim*FPS/1000;_++){
			now_rect.x=x+addx*_;
			now_rect.y=y+addy*_;
			
			SDL_RenderClear(renderer);
			
			for(int i=0;i<8;i++){
				for(int j=0;j<8;j++){
					if((i+j)%2)SDL_RenderCopy(renderer,texture["black_square"],NULL,&position[i][j]);
					else SDL_RenderCopy(renderer,texture["white_square"],NULL,&position[i][j]);
				}
			}
			
			string moved_piece;
			
			for(int i=0;i<8;i++){
				for(int j=0;j<8;j++){
					if(board.board[i][j].piece_type){
						string piece;
						piece.push_back(board.board[i][j].Symbol());
						if(i==move.x&&j==move.y){
							moved_piece=piece;
							continue;
						}
						int x=i,y=j;
						if(move.castling!=-1&&board.board[i][j].Type()==6&&board.board[i][j].color==board.turn){
							if(move.castling==0||move.castling==2)x+=2;
							if(move.castling==1||move.castling==3)x-=2;
						}
						if(side)SDL_RenderCopy(renderer,texture[piece],NULL,&position[x][y]);
						else SDL_RenderCopy(renderer,texture[piece],NULL,&position[7-x][7-y]);
					}
				}
			}
			
			SDL_RenderCopy(renderer,texture[moved_piece],NULL,&now_rect);
			
			SDL_RenderPresent(renderer);
			
			SDL_Delay(1000/FPS);
		}
		return;
	}
	
	void Save(){
		fout.open(file_path);
		
		fout<<side<<endl;
		fout<<white_player.is_computer<<endl;
		fout<<black_player.is_computer<<endl;
		fout<<board.fen()<<endl;
		
		fout.close();
		
		cout<<"saved"<<endl;
		return;
	}
	
	void Execute(Move move){
		Move_animation(move);
		
		if(move.castling!=-1){
			if(move.castling==0||move.castling==2)move.x=7,move.dx=-2;
			if(move.castling==1||move.castling==3)move.x=0,move.dx=3;
			Move_animation(move);
		}
		
		board.execute(move);
		
		Save();
		return;
	}
	
	void Start_Game(){
		SDL_SetRenderDrawColor(renderer,100,100,100,255);
		
		while(running&&!board.Game_over()){
			Draw_board(board);
			SDL_RenderPresent(renderer);
			
			Move move;
			
			if(board.turn)move=white_player.Get_move(board);
			else move=black_player.Get_move(board);
			
			if(running)Execute(move);
		}
		return;
	}
}chess_game;

struct HistoryFile{
	vector<string> histories;
	
	HistoryFile(){
		string directoryPath="history\\*.txt";

	    WIN32_FIND_DATA findFileData;
	    HANDLE hFind=FindFirstFile(directoryPath.c_str(),&findFileData);
	    
        do{
            if(!(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)){
            	histories.push_back(findFileData.cFileName);
            	histories.back()=histories.back().substr(0,histories.back().size()-4);
            }
        }while(FindNextFile(hFind,&findFileData)!=0);
        FindClose(hFind);
	}

	void Choose_mode_interface(){
		cout<<"Choose mode"<<endl;
		
		SDL_Rect PVC_position,PVP_position,CVC_position;
	
		PVC_position={100,100,600,50};
		
		PVP_position={100,200,600,50};
		
		CVC_position={100,300,600,50};
		
		while(running){
			if(left_mouse_click){
				if(In_rect(mouse_x,mouse_y,PVC_position)){
					fout.open(file_path);
					
					fout<<side<<endl;
					if(side)fout<<0<<endl<<1<<endl;
					else fout<<1<<endl<<0<<endl;
					fout<<INITIAL_FEN<<endl;
					
					fout.close();
					break;
				}
				if(In_rect(mouse_x,mouse_y,PVP_position)){
					fout.open(file_path);
					
					fout<<side<<endl;
					fout<<0<<endl<<0<<endl;
					fout<<INITIAL_FEN<<endl;
					
					fout.close();
					break;
				}
				if(In_rect(mouse_x,mouse_y,CVC_position)){
					fout.open(file_path);
					
					fout<<side<<endl;
					fout<<1<<endl<<1<<endl;
					fout<<INITIAL_FEN<<endl;
					
					fout.close();
					break;
				}
			}
			
			SDL_RenderClear(renderer);
			
			if(In_rect(mouse_x,mouse_y,PVC_position))SDL_RenderCopy(renderer,texture["bar1_choosed"],NULL,&PVC_position);
			else SDL_RenderCopy(renderer,texture["bar1"],NULL,&PVC_position);
			Render_text(Make_text("player VS computer",{60,60,60}),250,100);
			
			if(In_rect(mouse_x,mouse_y,PVP_position))SDL_RenderCopy(renderer,texture["bar1_choosed"],NULL,&PVP_position);
			else SDL_RenderCopy(renderer,texture["bar1"],NULL,&PVP_position);
			Render_text(Make_text("player VS player",{60,60,60}),250,200);
			
			if(In_rect(mouse_x,mouse_y,CVC_position))SDL_RenderCopy(renderer,texture["bar1_choosed"],NULL,&CVC_position);
			else SDL_RenderCopy(renderer,texture["bar1"],NULL,&CVC_position);
			Render_text(Make_text("computer VS computer",{60,60,60}),190,300);
			
			SDL_RenderPresent(renderer);
			
			SDL_Delay(1000/FPS);
		}
		
		SDL_Delay(1000/FPS);
		return;
	}

	void Choose_side_interface(){
		cout<<"Choose side"<<endl;

		SDL_Rect choose_white_position,choose_black_position,choose_side_position;
	
		choose_white_position={120,195,160,160};
		
		choose_side_position={320,195,160,160};
		
		choose_black_position={520,195,160,160};
		
		while(running){
			if(left_mouse_click){
				if(In_rect(mouse_x,mouse_y,choose_white_position)){
					side=true;
					break;
				}
				if(In_rect(mouse_x,mouse_y,choose_side_position)){
					side=rand(0,1);
					break;
				}
				if(In_rect(mouse_x,mouse_y,choose_black_position)){
					side=false;
					break;
				}
			}
			
			SDL_RenderClear(renderer);
			
			if(In_rect(mouse_x,mouse_y,choose_white_position))SDL_SetTextureAlphaMod(texture["choose_white"],128);
			else SDL_SetTextureAlphaMod(texture["choose_white"],255);
			SDL_RenderCopy(renderer,texture["choose_white"],NULL,&choose_white_position);
			
			if(In_rect(mouse_x,mouse_y,choose_side_position))SDL_SetTextureAlphaMod(texture["choose_side"],128);
			else SDL_SetTextureAlphaMod(texture["choose_side"],255);
			SDL_RenderCopy(renderer,texture["choose_side"],NULL,&choose_side_position);
			
			if(In_rect(mouse_x,mouse_y,choose_black_position))SDL_SetTextureAlphaMod(texture["choose_black"],128);
			else SDL_SetTextureAlphaMod(texture["choose_black"],255);
			SDL_RenderCopy(renderer,texture["choose_black"],NULL,&choose_black_position);
			
			SDL_RenderPresent(renderer);
			
			SDL_Delay(1000/FPS);
		}
		
		SDL_Delay(1000/FPS);
		
		cout<<"side = "<<side<<endl;
		
		Choose_mode_interface();
		return;
	}
	
	void Create_new_interface(){
		cout<<"Create new"<<endl;
		
		SDL_Rect confirm_position={275,300,250,50};
		
		input_lock=false;
		
		input_text.clear();
		
		while(running){
			if(left_mouse_click){
				if(In_rect(mouse_x,mouse_y,confirm_position)){
					file_path="history/"+input_text+".txt";
					break;
				}
			}
			
			SDL_RenderClear(renderer);
			
			Render_text(Make_text("name: "+input_text,{255,255,255}),100,200);
			
			if(In_rect(mouse_x,mouse_y,confirm_position))SDL_RenderCopy(renderer,texture["bar2_choosed"],NULL,&confirm_position);
			else SDL_RenderCopy(renderer,texture["bar2"],NULL,&confirm_position);
			Render_text(Make_text("CONFIRM",{60,60,60}),310,300);
			
			SDL_RenderPresent(renderer);
			
			SDL_Delay(1000/FPS);
		}
		
		input_lock=true;
		
		cout<<"get history file path: "<<file_path<<endl;
		
		SDL_Delay(1000/FPS);
		
		Choose_side_interface();
		return;
	}
	
	void Lead_in(){
		cout<<"open history file : "<<file_path<<endl;
		
		bool white_is_computer,black_is_computer;
		
		string fen="";
		
		fin.open(file_path);
		
		fin>>side>>white_is_computer>>black_is_computer>>fen;
		
		for(int i=1;i<6;i++){
			string s;
			fin>>s;
			fen=fen+" "+s;
		}
		
		fin.close();
		
		chess_game=ChessGame(fen,white_is_computer,black_is_computer);
		
		cout<<"lead in :"<<endl;
		cout<<"side is : "<<side<<endl;
		cout<<"white is computer : "<<white_is_computer<<endl;
		cout<<"black is computer : "<<black_is_computer<<endl;
		cout<<"FEN : "<<fen<<endl;
		return;
	}
	
	void Choose_history_interface(){
		bool is_new=false;
		
		int page=0,choosed_history=-1;
		
		while(running){
			if(left_mouse_click){
				if(In_rect(mouse_x,mouse_y,comfirm_position)&&choosed_history!=-1){
					file_path="history/"+histories[choosed_history]+".txt";
					
					cout<<"get history file path: "<<file_path<<endl;
					break;
				}
				
				choosed_history=-1;
				for(int i=0;i<5;i++){
					int now=page*5+i;
					if(now>=histories.size())break;
					if(In_rect(mouse_x,mouse_y,history_position[i]))choosed_history=now;
				}
				
				if(In_rect(mouse_x,mouse_y,new_position)){
					is_new=true;
					break;
				}
				
				if(In_rect(mouse_x,mouse_y,page_up_position)&&page>0)page--;
				
				if(In_rect(mouse_x,mouse_y,page_down_position)&&page<histories.size()/5)page++;
			}
			
			SDL_RenderClear(renderer);
			
			SDL_RenderCopy(renderer,texture["title"],NULL,&title_rect);
			
			for(int i=0;i<5;i++){
				int now=page*5+i;
				if(now>=histories.size())break;
				if(In_rect(mouse_x,mouse_y,history_position[i])||choosed_history==now){
					SDL_RenderCopy(renderer,texture["bar1_choosed"],NULL,&history_position[i]);
				}
				else SDL_RenderCopy(renderer,texture["bar1"],NULL,&history_position[i]);
				Render_text(Make_text(histories[now],{60,60,60}),120,200+i*55);
			}
			
			if(In_rect(mouse_x,mouse_y,comfirm_position)&&choosed_history!=-1){
				SDL_RenderCopy(renderer,texture["bar2_choosed"],NULL,&comfirm_position);
			}
			else SDL_RenderCopy(renderer,texture["bar2"],NULL,&comfirm_position);
			Render_text(Make_text("COMFIRM",{60,60,60}),180,475);
			
			if(In_rect(mouse_x,mouse_y,new_position)){
				SDL_RenderCopy(renderer,texture["bar2_choosed"],NULL,&new_position);
			}
			else SDL_RenderCopy(renderer,texture["bar2"],NULL,&new_position);
			Render_text(Make_text("NEW",{60,60,60}),480,475);
			
			if(In_rect(mouse_x,mouse_y,page_up_position)&&page>0){
				SDL_RenderCopy(renderer,texture["page_up_choosed"],NULL,&page_up_position);
			}
			else SDL_RenderCopy(renderer,texture["page_up"],NULL,&page_up_position);
			
			if(In_rect(mouse_x,mouse_y,page_down_position)&&page<histories.size()/5){
				SDL_RenderCopy(renderer,texture["page_down_choosed"],NULL,&page_down_position);
			}
			else SDL_RenderCopy(renderer,texture["page_down"],NULL,&page_down_position);
			
			SDL_RenderPresent(renderer);
			
			SDL_Delay(1000/FPS);
		}
		
		if(is_new)Create_new_interface();
		
		Lead_in();
		
		SDL_Delay(1000/FPS);
		return;
	}
}history_file;

void Start_interface(){
	cout<<"Start"<<endl;
	
	history_file.Choose_history_interface();
	
	chess_game.Start_Game();
	return;
}

int main(int argc,char* argv[]){
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_VIDEO)){
        cerr<<"无法初始化 SDL: "<<SDL_GetError()<<endl;
        return 1;
    }
    
    Pretreatment();
    
    thread game_theard(Start_interface);

    SDL_Event event;
    while(running){
    	left_mouse_click=false;
        SDL_GetMouseState(&mouse_x,&mouse_y);
        
    	while(SDL_PollEvent(&event)){
			if(event.type==SDL_QUIT)running=false;
			if(event.type==SDL_MOUSEBUTTONDOWN){
                if(event.button.button==SDL_BUTTON_LEFT)left_mouse_down=true;
            }
            if(event.type==SDL_MOUSEBUTTONUP){
                if(event.button.button==SDL_BUTTON_LEFT&&left_mouse_down){
                    left_mouse_down=false;
                    left_mouse_click=true;
                }
            }
            if(!input_lock){
	            if(event.type==SDL_TEXTINPUT)input_text+=event.text.text;
	            if(event.type==SDL_KEYDOWN){
	                if(event.key.keysym.sym==SDLK_BACKSPACE&&!input_text.empty())input_text.pop_back();
	            }
			}
		}
		
		SDL_Delay(1000/FPS);
    }
    
    game_theard.join();
    
    TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
	IMG_Quit();
    SDL_Quit();
    return 0;
}

