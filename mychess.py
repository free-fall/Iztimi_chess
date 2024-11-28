from random import randint,uniform,choice
from functools import wraps
from time import sleep
import threading
import pygame
import chess
import math
import sys

FPS=20
canvas_x=0
canvas_y=0
need_flip=False

pygame.init()
screen_width=800
screen_height=550
screen=pygame.display.set_mode((screen_width,screen_height))
screen.fill((100,100,100))
pygame.display.set_caption("Mychess")
clock=pygame.time.Clock()

class newThread(threading.Thread):
	def __init__(self,func,*args,**kwargs)->None:
		threading.Thread.__init__(self)
		self.func=func
		self.p=args
		self.q=kwargs
	def run(self)->None:
		self.func(*self.p,**self.q)

def newthread(func):
	@wraps(func)
	def wrapper(*args,**kwargs):
		p=newThread(func,*args,**kwargs)
		p.start()
	return wrapper

def Pretreatment()->None:
	global image

	image={}
	image['P']=pygame.transform.scale(pygame.image.load('images/white_pawn.png'),(64,64))
	image['N']=pygame.transform.scale(pygame.image.load('images/white_knight.png'),(64,64))
	image['B']=pygame.transform.scale(pygame.image.load('images/white_bishop.png'),(64,64))
	image['R']=pygame.transform.scale(pygame.image.load('images/white_rook.png'),(64,64))
	image['Q']=pygame.transform.scale(pygame.image.load('images/white_queen.png'),(64,64))
	image['K']=pygame.transform.scale(pygame.image.load('images/white_king.png'),(64,64))
	image['p']=pygame.transform.scale(pygame.image.load('images/black_pawn.png'),(64,64))
	image['n']=pygame.transform.scale(pygame.image.load('images/black_knight.png'),(64,64))
	image['b']=pygame.transform.scale(pygame.image.load('images/black_bishop.png'),(64,64))
	image['r']=pygame.transform.scale(pygame.image.load('images/black_rook.png'),(64,64))
	image['q']=pygame.transform.scale(pygame.image.load('images/black_queen.png'),(64,64))
	image['k']=pygame.transform.scale(pygame.image.load('images/black_king.png'),(64,64))
	image['choose_side']=pygame.image.load('images/choose_side.png')
	image['choose_white']=pygame.image.load('images/choose_white.png')
	image['choose_black']=pygame.image.load('images/choose_black.png')
	image['white_square']=pygame.transform.scale(pygame.image.load('images/white_square.png'),(64,64))
	image['black_square']=pygame.transform.scale(pygame.image.load('images/black_square.png'),(64,64))
	image["reachable_point"]=pygame.transform.scale(pygame.image.load('images/reachable_point.png'),(64,64))
	image['attack']=pygame.transform.scale(pygame.image.load('images/attack.png'),(64,64))
	image['chessboard']=pygame.Surface((800,550),pygame.SRCALPHA|pygame.HWSURFACE)
	image['below_pieces']=pygame.Surface((800,550),pygame.SRCALPHA|pygame.HWSURFACE)
	image['pieces']=pygame.Surface((800,550),pygame.SRCALPHA|pygame.HWSURFACE)
	image['above_pieces']=pygame.Surface((800,550),pygame.SRCALPHA|pygame.HWSURFACE)
	return

def Place_image(s,x=0,y=0,alpha=1,canvas=screen)->None:
	global image

	now=image[s].copy()
	now.set_alpha(255*alpha)
	canvas.blit(now,(x+canvas_x,y+canvas_y))
	return

class Chessboard:
	def __init__(self,side=True)->None:
		self.board=chess.Board()
		self.side=side
		return
	
	def Change(self,now)->tuple:
		x=now%8
		y=now//8
		if self.side:y=7-y
		else:x=7-x
		return x,y

	def Show(self)->None:
		screen.fill((100,100,100))
		screen.blit(image['chessboard'],(canvas_x,canvas_y))
		screen.blit(image['below_pieces'],(canvas_x,canvas_y))
		screen.blit(image['pieces'],(canvas_x,canvas_y))
		screen.blit(image['above_pieces'],(canvas_x,canvas_y))
		pygame.display.flip()
		return
	
	def Get_mouse_position(self)->int:
		mouse_x,mouse_y=pygame.mouse.get_pos()
		mouse_x=max(mouse_x,0)
		mouse_x=min(mouse_x,screen_width)
		mouse_y=max(mouse_y,0)
		mouse_y=min(mouse_y,screen_height)
		x=(mouse_x-144)//64
		y=(mouse_y-19)//64
		if self.side:return (7-y)*8+x
		else:return y*8+(7-x)
		return -1

	def Get_Move(self)->chess.Move:
		global need_flip

		lift=None
		choosed_move=None
		possible_moves=[]
		need_flip=False
		while choosed_move is None:
			now=self.Get_mouse_position()
			if pygame.mouse.get_pressed()[0]:
				for move in possible_moves:
					if move.to_square==now:
						choosed_move=move
				if choosed_move is None:
					piece=self.board.piece_at(now)
					if piece and piece.color==self.board.turn:
						lift=now
						possible_moves=[]
						for move in self.board.legal_moves:
							if move.from_square==lift:
								possible_moves.append(move)
						need_flip=True
					elif lift is not None:
						lift=None
						possible_moves=[]
						need_flip=True
			if lift is not None:
				image['above_pieces'].fill((0,0,0,0))
				for move in possible_moves:
					x,y=self.Change(move.to_square)
					x=x*64+144
					y=y*64+19
					piece=self.board.piece_at(move.to_square)
					if piece is None:
						Place_image('reachable_point',x,y,0.3,canvas=image['above_pieces'])
					else:
						Place_image('attack',x,y,canvas=image['above_pieces'])
			if need_flip:
				need_flip=False
				self.Show()
			clock.tick(FPS)
		if choosed_move.promotion:
			is_choosed=False
			if self.board.turn==True:
				Place_image('Q',40,19,canvas=image['pieces'])
				Place_image('R',40,83,canvas=image['pieces'])
				Place_image('N',40,147,canvas=image['pieces'])
				Place_image('B',40,211,canvas=image['pieces'])
			else:
				Place_image('q',40,19,canvas=image['pieces'])
				Place_image('r',40,83,canvas=image['pieces'])
				Place_image('n',40,147,canvas=image['pieces'])
				Place_image('b',40,211,canvas=image['pieces'])
			self.Show()
			while not is_choosed:
				mouse_x,mouse_y=pygame.mouse.get_pos()
				mouse_x=max(mouse_x,0)
				mouse_x=min(mouse_x,screen_width)
				mouse_y=max(mouse_y,0)
				mouse_y=min(mouse_y,screen_height)
				if pygame.mouse.get_pressed()[0]:
					for i in [19,83,147,211]:
						if 40<=mouse_x<=104:
							if i<=mouse_y<=i+64:
								if i==19:choosed_move.promotion=chess.QUEEN
								if i==83:choosed_move.promotion=chess.ROOK
								if i==147:choosed_move.promotion=chess.KNIGHT
								if i==211:choosed_move.promotion=chess.BISHOP
								is_choosed=True
				clock.tick(FPS)
		image['above_pieces'].fill((0,0,0,0))
		return choosed_move

	def Move_animation(self,st,ed,move=None)->None:
		tim=0.1
		cnt=20
		piece=self.board.piece_at(st)
		x,y=self.Change(st)
		x=x*64+144
		y=y*64+19
		xx,yy=self.Change(ed)
		xx=xx*64+144
		yy=yy*64+19
		addx=(xx-x)/cnt
		addy=(yy-y)/cnt
		for _ in range(cnt):
			image['pieces'].fill((0,0,0,0))
			for i in range(0,8):
				for j in range(0,8):
					if self.side:now=(7-i)*8+j
					else:now=i*8+(7-j)
					_piece=self.board.piece_at(now)
					if move and now==move.to_square:
						Place_image(self.board.piece_at(move.from_square).symbol(),144+j*64,19+i*64,canvas=image['pieces'])
					elif _piece and now!=st:
						if move==None or now!=move.from_square:
							Place_image(_piece.symbol(),144+j*64,19+i*64,canvas=image['pieces'])
			x+=addx
			y+=addy
			Place_image(piece.symbol(),x,y,canvas=image['pieces'])
			self.Show()
			sleep(tim/cnt)
		return

	def Move(self,move)->None:
		piece=self.board.piece_at(move.from_square)
		self.Move_animation(move.from_square,move.to_square)
		if piece.piece_type==chess.KING and move.to_square//8==move.from_square//8 and abs(move.to_square-move.from_square)==2:
			if move.to_square%8==2:
				self.Move_animation(move.to_square-2,move.to_square+1,move)
			if move.to_square%8==6:
				self.Move_animation(move.to_square+1,move.to_square-1,move)
		self.board.push(move)
		return

	def Start_game(self)->None:
		records=[]
		while not self.board.is_game_over():
			image['below_pieces'].fill((0,0,0,0))
			image['pieces'].fill((0,0,0,0))
			image['above_pieces'].fill((0,0,0,0))
			for i in range(0,8):
				for j in range(0,8):
					if self.side:piece=self.board.piece_at((7-i)*8+j)
					else:piece=self.board.piece_at(i*8+(7-j))
					if piece:
						Place_image(piece.symbol(),144+j*64,19+i*64,canvas=image['pieces'])
			self.Show()
			records.append(self.board.copy())
			if self.board.turn==self.side:
				move=self.Get_Move()
				self.Move(move)
			else:
				move=self.Get_Move()
				self.Move(move)
		return

def Shake_screen(shake_distance=5,shake_speed=0.05,shake_times=5)->None:
	global canvas_x,canvas_y

	for i in range(shake_times):
		canvas_x=randint(-shake_distance,shake_distance)
		canvas_y=randint(-shake_distance,shake_distance)
		chess_board.Show()
		sleep(shake_speed)
	canvas_x,canvas_y=0,0
	return

@newthread
def Choose_side()->None:
	global screen,chess_board,image

	side=-1

	while side==-1:
		mouse_x,mouse_y=pygame.mouse.get_pos()
		mouse_x=max(mouse_x,0)
		mouse_x=min(mouse_x,screen_width)
		mouse_y=max(mouse_y,0)
		mouse_y=min(mouse_y,screen_height)

		screen.fill((100,100,100))
		if 120<=mouse_x<=280 and 195<=mouse_y<=355:
			Place_image('choose_white',120,195,0.8)
			if pygame.mouse.get_pressed()[0]:
				side=1
		else:
			Place_image('choose_white',120,195)
		if 320<=mouse_x<=480 and 195<=mouse_y<=355:
			Place_image('choose_side',320,195,0.8)
			if pygame.mouse.get_pressed()[0]:
				side=randint(0,1)
		else:
			Place_image('choose_side',320,195)
		if 520<=mouse_x<=680 and 195<=mouse_y<=355:
			Place_image('choose_black',520,195,0.8)
			if pygame.mouse.get_pressed()[0]:
				side=0
		else:
			Place_image('choose_black',520,195)
		pygame.display.flip()
		clock.tick(FPS)

	chess_board=Chessboard(side)
	image['chessboard'].fill((0,0,0,0))
	for i in range(0,8):
		for j in range(0,8):
			if ((i+j)%2)^side:
				Place_image('white_square',144+j*64,19+i*64,canvas=image['chessboard'])
			else:
				Place_image('black_square',144+j*64,19+i*64,canvas=image['chessboard'])
	image['pieces'].fill((0,0,0,0))
	for i in range(0,8):
		for j in range(0,8):
			if chess_board.side:
				piece=chess_board.board.piece_at((7-i)*8+j)
			else:
				piece=chess_board.board.piece_at(i*8+(7-j))
			if piece:
				Place_image(piece.symbol(),144+j*64,19+i*64,canvas=image['pieces'])
	chess_board.Start_game()
	return

if __name__=='__main__':
	Pretreatment()
	Choose_side()
	running=True
	while running:
		for event in pygame.event.get():
			if event.type==pygame.QUIT:
				running=False
		clock.tick(FPS)
	pygame.quit()
	sys.exit()