#pragma once

#include<array>
#include<iostream>
#include<iomanip>
#include<unistd.h>
#include<algorithm>
#include<random>
#include<vector>	
using namespace std;
typedef pair<int, int> Pair;

#define BLACK 0
#define WHITE 1
/*
surakarta game board
any change in board will write here
*/

class board {
public:
	typedef array<int, 6> row;
	typedef array<row, 6> grid;
	int step;//if odd, white's turn if even, black's turn

	board() :tile() {
		step = 0;
		for (unsigned int i = 0; i < 6; i++) {
			for (unsigned int j = 0; j < 6; j++) {
				if (i <= 1) tile[i][j] = 0;
				if (i <= 3 && i > 1) tile[i][j] = 9;
				else if (i <= 5 && i > 3) tile[i][j] = 1;
			}
		}
		// tile[2][0] = 0;
		// tile[0][2] = 1;
	}

	int& operator()(int i) { return tile[ i / 6 ][ i % 6 ]; }
	// int operator()(int i) const { return tile[ i / 6 ][ i % 6 ]; }
	board& operator =(const board &b) = default;

	double judge() {
		int b = 0, w = 0;
		for (int i=0; i<36; i++) {
			if ( (*this)(i) == 0)
				b++;
			else if ( ((*this)(i) == 1) )
				w++;
		}
		if (b==w)
			return 0.5;
		return (b>w)?1:0;
	}
	int take_turn() {
		if(step%2)
			return WHITE;
		else
			return BLACK;
	}
	/* 
	piece:
	0:no piece 
	1:black 
	2:white
	*/
	int search_up(int &pos, bool pass, const int piece, int &count_step) {
		count_step++;
		
		// has run a circle
		if (count_step >= 25) {
			count_step = 0;
			return -1;
		}

		if ( (*this)(pos) == piece) return -1;
		else if( (*this)(pos) == (!piece)) {
			if(pass)
				return 1;
			else
				return -1;
		}
		pos -= 6;

		if (pos < 0) {
			switch(pos) {
				case -5: pos = 6; pass = true; return search_right(pos, pass, piece, count_step); 
				case -4: pos = 12; pass = true; return search_right(pos, pass, piece, count_step);
				case -3: pos = 17; pass = true; return search_left(pos, pass, piece, count_step);
				case -2: pos = 11; pass = true; return search_left(pos, pass, piece, count_step);
				default: pos = -1;
			}
		}
		if (pos != -1)
			return search_up(pos, pass, piece, count_step);
		else 
			return -1;
	}

	int search_down(int &pos, bool pass, const int piece, int &count_step) {
		reflect_vertical(pos);
		int ans = search_up(pos, pass, piece, count_step);
		reflect_vertical(pos);
		return ans;
	}

	int search_right(int &pos, bool pass, const int piece, int &count_step) {
		rotate_left(pos);
		int ans = search_up(pos, pass, piece, count_step);
		rotate_right(pos);
		return ans;
	}

	int search_left(int &pos, bool pass, const int piece, int &count_step) {
		rotate_right(pos);
		int ans = search_up(pos, pass, piece, count_step);
		rotate_left(pos);
		return ans;
	}

public:

	int check_eat(int pos, const int piece) {
		if (pos == 0 || pos == 5 || pos == 30 || pos == 35)
			return -1;
		//search four way and return position when have eatable siece
		(*this)(pos) = 9;

		int prev_pos = pos;
		int ans = -1;
		vector<int> p;

		int count_step = 0;
		if ( search_up(pos, false, piece, count_step) != -1) p.push_back(pos);	  
		pos = prev_pos; count_step = 0;
		if ( search_down(pos, false, piece, count_step) != -1) p.push_back(pos); 
		pos = prev_pos; count_step = 0;
		if ( search_right(pos, false, piece, count_step) != -1) p.push_back(pos); 
		pos = prev_pos; count_step = 0;
		if ( search_left(pos, false, piece, count_step) != -1) p.push_back(pos);  
		pos = prev_pos; count_step = 0;

		(*this)(prev_pos) = piece;

		if (p.empty())
			return ans;
		else
			return p[0];//return ramdomly
	}
//check whether the moves are available
	vector<int> check_move(int pos, const int piece) {
		vector<int> ans;
		ans.reserve(100);
		//8 directions
		int dir[8] = {-7, -6, -5, -1, 1, 5, 6, 7};
		if (pos <= 5 && pos >= 0) 
			dir[0] = 0, dir[1] = 0, dir[2] = 0;
		
		if ( (pos % 6) == 0) 
			dir[0] = 0, dir[3] = 0, dir[5] = 0;
		
		if ( ( (pos+1) % 6) == 0) 
			dir[2] = 0, dir[4] = 0, dir[7] = 0;
		
		if (pos <= 35 && pos >= 30) 
			dir[5] = 0, dir[6] = 0, dir[7] = 0;
		
		for (auto &d : dir) {
			if (d == 0) continue;
			if ( (*this)(pos + d) == 9 ){
				ans.push_back(pos + d);
			}
		}
		return ans;
	}

	vector<Pair> eat_piece(const int piece) {
		vector<Pair> ans;

		for (int i=0; i<36; i++) {
			int tile = (*this)(i);
			if (tile == piece) {
				int eat_pos = check_eat(i, piece);
				
				if (eat_pos != -1) {
					// return make_pair(i, eat_pos);
					
					ans.push_back(make_pair(i, eat_pos));
				}
				
			}
		}
		// return make_pair(-1, -1);
		return ans;
	}

	vector<Pair> move_piece(const int piece) {
		vector< Pair > ans; //store available moves pairs(now and next)
		vector< int > move_pos;//store available next move 

		for (int now_pos = 0; now_pos < 36; now_pos++) {
			int tile = (*this)(now_pos);
			if (tile == piece) {		
				move_pos = check_move(now_pos, piece);
				if ( !move_pos.empty() ) {
					for (auto &next_pos : move_pos)
						ans.push_back( make_pair(now_pos, next_pos) );
				}
			}
		}
		//random select a move
		random_device rd;
		default_random_engine engine(rd());

		// shuffle(ans.begin(), ans.end(), engine);
		return ans;
		// return ans[0];
	}
	vector<Pair> get_available_move(const int piece) {
		vector<Pair> mv = move_piece(piece);
		vector<Pair> ea = eat_piece(piece);
		ea.insert(ea.end(), mv.begin(), mv.end());
		
		return ea;
	}		

	int check_Piece_onBoard (const int piece)  {
		bool find = 0;
		for (int i=0; i<36; i++) {
			if ( (*this)(i) == piece){
				find = 1; break;
			}
		}
		if(find) return 1;
		else return 0;
	}


public:
	//move or eat a piece
	int move(int prev_pos, int place_pos, int piece) {
		if (place_pos > 35 || place_pos < 0) return -1;
		//if ((*this)(place_pos) != 0) return -1;

		// cout << piece << "'s chance.\n";
		// cout << "Move from (" << prev_pos / 6 << ", " << prev_pos % 6 << ") to (" 
		// << place_pos / 6 << ", " << place_pos % 6 << ")\n\n";

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = 9;
		step++;
		// cout << (*this) << '\n';

		// operator()(place_pos) = piece;
		return 0;
	}

	int eat(int prev_pos, int place_pos, int piece) {
		if (place_pos > 35 || place_pos < 0) return -1;
		if ((*this)(place_pos) != (!piece) ) return -1;
		
		// cout << piece << "'s chance.\n";
		// cout << "Eat from (" << prev_pos / 6 << ", " << prev_pos % 6 << ") to (" 
		// << place_pos / 6 << ", " << place_pos % 6<< ")\n\n";

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = 9;
		step++;
		// cout << (*this) << '\n';
		// operator()(place_pos) = piece;
		return 0;
	}

	void reflect_vertical(int &pos) {
		int x = pos / 6; int y = pos % 6;
		pos = (5-x) * 6 + y;
		for (int c = 0; c < 6; c++) {
			swap(tile[0][c], tile[5][c]);
			swap(tile[1][c], tile[4][c]);
			swap(tile[2][c], tile[3][c]);
		}
	}

	void reflect_horizontal(int &pos) {
		int x = pos / 6; int y = pos % 6;
		pos = x * 6 + (5-y);
		for (int r = 0; r < 6; r++) {
			swap(tile[r][0], tile[r][5]);
			swap(tile[r][1], tile[r][4]);
			swap(tile[r][2], tile[r][3]);

		}
	}

	void transpose(int &pos) {
		int x = pos / 6; int y = pos % 6;
		pos = y * 6 + x;
		for (int r = 0; r < 6; r++) {
			for (int c = (r + 1); c < 6; c++) {
				swap(tile[r][c], tile[c][r]);
			}
		}
	}

	void rotate_right(int &pos) {transpose(pos); reflect_horizontal(pos);}

	void rotate_left(int &pos) {transpose(pos); reflect_vertical(pos);}

public:
			/*
			       0 1 2 3 4 5
			"  ┏━━━━━━━┓ ┏━━━━━━━┓  
			   ┃ ┏━━━┓ ┃ ┃ ┏━━━┓ ┃
			  0┃ ┃ 1 1 1 1 1 1 ┃ ┃ 
			  1┃ ┗ 1 1 1 1 1 1 ┛ ┃
			  2┗━━ 0 0 0 0 0 0 ━━┛
			  3┏━━ 0 0 0 0 0 0 ━━┓
			  4┃ ┏ 2 2 2 2 2 2 ┓ ┃
		      5┃ ┃ 2 2 2 2 2 2 ┃ ┃
		       ┃ ┗━━━┛ ┃ ┃ ┗━━━┛ ┃
		       ┗━━━━━━━┛ ┗━━━━━━━┛
			"
		*/
	friend ostream& operator<<(ostream& out, const board& b) {
		out << "+--------------------+" << '\n';
		out << "      0 1 2 3 4 5    " << '\n';
		out << "  ┏━━━━━━━┓ ┏━━━━━━━┓" << '\n';
		out << "  ┃ ┏━━━┓ ┃ ┃ ┏━━━┓ ┃" << '\n';
		
		int idx = 0;
		for (auto &row : b.tile) {
			switch(idx) {
				case 0: out << "0 ┃ ┃"; break;
				case 1: out << "1 ┃ ┗"; break;
				case 2: out << "2 ┗━━"; break;
				case 3: out << "3 ┏━━"; break;
				case 4: out << "4 ┃ ┏"; break;
				case 5: out << "5 ┃ ┃"; break;
			}

			for (auto t : row) {
				if (t==9)
					out << setw(2) << 'x';
				else
					out << setw(2) << t;
			}

			switch(idx) {
				case 0: out << " ┃ ┃"; break;
				case 1: out << " ┛ ┃"; break;
				case 2: out << " ━━┛"; break;
				case 3: out << " ━━┓"; break;
				case 4: out << " ┓ ┃"; break;
				case 5: out << " ┃ ┃"; break;
			}
			out << '\n';
			idx++;
		}
		out << "  ┃ ┗━━━┛ ┃ ┃ ┗━━━┛ ┃" << '\n';
		out << "  ┗━━━━━━━┛ ┗━━━━━━━┛" << '\n';
		out << "+--------------------+" << '\n';
		return out;
	}

private:
	grid tile;
};