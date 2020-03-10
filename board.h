/*
Explanation:

Surakarta Game board, implement functions related to board.
*/

#pragma once

#include <array>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <functional>

class Pair {
public:
	char prev;
	char next;
	Pair() : prev(-1), next(-1) {}
	Pair (char x, char y) : prev(x), next(y) {}

	bool operator==(const Pair &p) {
		return ( p.prev==prev && p.next==next);
	}
	bool operator!=(const Pair &p) {
		return ( ! ((*this)==p) );
	}
};


enum Piece {
	BLACK,
	WHITE,
	SPACE = 9,
	UNKNOWN = -1
};
enum WIN_STATE {
	BLACK_WIN,
	WHITE_WIN,
	DRAW
};
enum EXEC_STATE {
	SUCCESS = 1,
	FAIL = -1
};


class board {

public:
	typedef std::array<char, 6> row;
	typedef std::array<row, 6> grid;
private:
	grid tile;
	
public:
	int step;//if odd, white's turn if even, black's turn
	
	static constexpr int SIZE { 36 };
	static constexpr int COL { 6 };
	
	// std::map<int, std::pair<int, std::function<board::EXEC_STATE(char&, bool , const bool&, char&)> > >mapping_circle;
	std::map<char, std::pair<char, EXEC_STATE(board::*)(char&, bool , const bool&, char&)> > mapping_circle;
	void init_map() {
		mapping_circle = {

		{ 1,  { 6, &board::search_right} }, { 2, {12, &board::search_right} }, {3 , {17, &board::search_left } }, {4 , {11, &board::search_left} },
		{ 6,  { 1, &board::search_down } }, {12, { 2, &board::search_down } }, {18, {32, &board::search_up   } }, {24, {31, &board::search_up  } },
		{31,  {24, &board::search_right} }, {32, {18, &board::search_right} }, {33, {23, &board::search_left } }, {34, {29, &board::search_left} },
		{29,  {34, &board::search_up   } }, {23, {33, &board::search_up   } }, {17,  {3, &board::search_down } }, {11, { 4, &board::search_down} }
		};
	}

	board() : tile(), step(0) {
		init_map();	
		for (int i{0}; i < COL; ++i) {
			for (int j = 0; j < COL; ++j) {
				if (i <= 1) tile[i][j] = BLACK;
				if (i <= 3 && i > 1) tile[i][j] = SPACE;
				else if (i <= 5 && i > 3) tile[i][j] = WHITE;
			}
		}
	}
	char& operator()(char i)  { return tile[ i / 6 ][ i % 6 ]; }
	char operator()(char i) const {return tile[ i / 6 ][ i % 6 ]; }

	char& operator()(char x, char y)  { return tile[x][y]; }
	char operator()(char x, char y) const { return tile[x][y]; }	
	board& operator =(const board &b) = default;

	inline WIN_STATE compare_piece() const {
		int b = 0, w = 0;
		for (int i{0}; i < SIZE; ++i) {
			if ( (*this)(i) == BLACK)
				b++;
			else if ( ((*this)(i) == WHITE) )
				w++;
		}
		if (b==w)
			return DRAW;
		return (b>w) ? BLACK_WIN : WHITE_WIN;
	}

	inline bool take_turn() const {
		if (step % 2){
			return WHITE;
		}
		else {
			return BLACK;
		}
	}

	inline unsigned count_piece(const bool &piece) const {
		unsigned total = 0;
		
		for(auto &r : tile) {
			for(auto &c : r) {
				if (piece == c)
					++total;
			}
		}
		return total;
	}
private:
	EXEC_STATE search_up (char &pos, bool pass, const bool &piece, char &count_step) {

		do {
			count_step++;
			
			// search has passed through circle and can't find eatable piece 
			// or collided with same color piece
			if (count_step >= 25 || (*this)(pos) == piece) {
				count_step = 0;
				return FAIL;
			}
		
			if( (*this)(pos) == (!piece)) {
				// pass: search route has passed a ring or not
				if(pass)
					return SUCCESS;
				else {
					count_step = 0;
					return FAIL;
				}
			}
			pos -= COL;
		} while(pos >= 0);
		
		// return to previous position
		pos += COL;
		
		// pass through circles
		if ( mapping_circle.find(pos) != mapping_circle.end() ){
			pass = true;
			char old_pos = pos;
			pos = mapping_circle[pos].first;
			return (this->*mapping_circle[old_pos].second)( pos, pass, piece, count_step); 
		}
		else {
			count_step = 0;
			return FAIL;
		}
	}

	EXEC_STATE search_down (char &pos, bool pass, const bool &piece, char &count_step)  {
		do{
			count_step++;
			
			// search has passed through circle and can't find eatable piece 
			// or collided with same color piece
			if (count_step >= 25 || (*this)(pos) == piece) {
				count_step = 0;
				return FAIL;
			}
		
			if( (*this)(pos) == (!piece)) {
				// pass: search route has passed a ring or not
				if(pass)
					return SUCCESS;
				else {
					count_step = 0;
					return FAIL;
				}
			}
			pos += COL;

		} while( pos < 36 );
		
		// return to previous position
		pos -= COL;
		
		// pass through circles
		if ( mapping_circle.find(pos) != mapping_circle.end() ){
			pass = true;
			char old_pos = pos;
			pos = mapping_circle[pos].first;
			return (this->*mapping_circle[old_pos].second)( pos, pass, piece, count_step); 
		}
		else {
			count_step = 0;
			return FAIL;
		}		
	}
	
	EXEC_STATE search_right (char &pos, bool pass, const bool &piece, char &count_step) {
		do {
			count_step++;
			
			// search has passed through circle and can't find eatable piece 
			// or collided with same color piece
			if (count_step >= 25 || (*this)(pos) == piece) {
				count_step = 0;
				return FAIL;
			}
		
			if( (*this)(pos) == (!piece)) {
				// pass: search route has passed a ring or not
				if(pass)
					return SUCCESS;
				else {
					count_step = 0;
					return FAIL;
				}
			}
			pos++;

		} while ( (pos%6) != 0 );
		
		// return to previous position
		pos--; 

		// pass through circles
		if ( mapping_circle.find(pos) != mapping_circle.end() ){
			pass = true;
			char old_pos = pos;
			pos = mapping_circle[pos].first;
			return (this->*mapping_circle[old_pos].second)( pos, pass, piece, count_step); 
		}
		else {
			count_step = 0;
			return FAIL;
		}

	}

	EXEC_STATE search_left (char &pos, bool pass, const bool &piece, char &count_step) {
		do {
			count_step++;
			
			// search has passed through circle and can't find eatable piece 
			// or collided with same color piece
			if (count_step >= 25 || (*this)(pos) == piece) {
				count_step = 0;
				return FAIL;
			}
		
			if( (*this)(pos) == (!piece)) {
				// pass: search route has passed a ring or not
				if(pass)
					return SUCCESS;
				else {
					count_step = 0;
					return FAIL;
				}
			}
			pos--;
		} while ( (pos%6) != 5);
		
		// return to previous position
		pos++;

		// pass through circles
		if ( mapping_circle.find(pos) != mapping_circle.end() ){
			pass = true;
			char old_pos = pos;
			pos = mapping_circle[old_pos].first;
			return (this->*mapping_circle[old_pos].second)( pos, pass, piece, count_step); 
		}
		else {
			count_step = 0;
			return FAIL;
		}
	}
	

	EXEC_STATE check_eat (char &origin_pos, const bool &piece) {
		char pos = origin_pos;
		const char prev_pos = pos;
		std::vector<char> p;	
		
		// corner pos is not on any track, can't eat any piece
		if (pos == 0 || pos == 5 || pos == 30 || pos == 35)
			return FAIL;
		
		// search four different ways
		(*this)(pos) = SPACE;

		char count_step = 0;
		if ( search_up(pos, false, piece, count_step) != FAIL) p.push_back(pos);	  
		pos = prev_pos; count_step = 0;
		if ( search_down(pos, false, piece, count_step) != FAIL) p.push_back(pos); 
		pos = prev_pos; count_step = 0;
		if ( search_right(pos, false, piece, count_step) != FAIL) p.push_back(pos); 
		pos = prev_pos; count_step = 0;
		if ( search_left(pos, false, piece, count_step) != FAIL) p.push_back(pos);  
		pos = prev_pos; count_step = 0;
		
		(*this)(prev_pos) = piece;

		if (p.empty())
			return FAIL;
		else{
			origin_pos = p[0];
			return SUCCESS;  // return ramdomly
		}
	}
	// check whether the moves are available
	std::vector<char> check_move (const char &pos, const bool &piece) const {
		std::vector<char> movable;
		
		// 8 directions
		char dir[8] {-7, -6, -5, -1, 1, 5, 6, 7};
		const char no_move {0};

		// pos in top row
		if (pos <= 5 && pos >= 0) 
			dir[0] = dir[1] = dir[2] = no_move;
		
		// pos in left column
		if ( (pos % 6) == 0) 
			dir[0] = dir[3] = dir[5] = no_move;
		
		// pos in right column
		if ( ( (pos+1) % 6) == 0) 
			dir[2] = dir[4] = dir[7] = no_move;
		
		// pos in bottom row
		if (pos <= 35 && pos >= 30) 
			dir[5] = dir[6] = dir[7] = no_move;
		
		for (auto &d : dir) {
			if (d == no_move) continue;
			if ( (*this)(pos + d) == SPACE ){
				movable.push_back(pos + d);
			}
		}
		return movable;
	}
public:
	std::vector<Pair> eat_piece (const bool &piece) {
		std::vector<Pair> eatable;

		for (char i {0}; i < SIZE; ++i) {
			char next_pos = i;
			char tile = (*this)(next_pos);
			
			if (tile == piece) {
				char now_pos = next_pos;
				EXEC_STATE eat_pos = check_eat(next_pos, piece);
				
				if (eat_pos != FAIL) {
					eatable.push_back( {now_pos, next_pos} );
				}
				
			}
		}
		return eatable;
	}

	std::vector<Pair> move_piece (const bool &piece) {
		std::vector<Pair> movable; // store available moves pairs(now and next)

		for (char now_pos {0}; now_pos < SIZE; ++now_pos) {
			char tile = (*this)(now_pos);
			if (tile == piece) {		
				std::vector<char> move_pos { check_move(now_pos, piece) };
				
				if ( !move_pos.empty() ) {
					for (auto &next_pos : move_pos)
						movable.push_back( {now_pos, next_pos} );
				}
			}
		}
		return movable;
	}
public:
	std::vector<Pair> get_available_move(const bool &piece) {
		std::vector<Pair> mv = move_piece(piece);
		std::vector<Pair> ea = eat_piece(piece);
		ea.insert(ea.end(), mv.begin(), mv.end());
		
		return ea;
	}

	EXEC_STATE check_Piece_onBoard (const bool &piece) const {
		bool find = false;

		for (int i = 0; i < SIZE; i++) {
			if ( (*this)(i) == piece){
				find = true; break;
			}
		}
		if(find) return SUCCESS;
		else return FAIL;
	}

	// move or eat a piece
	EXEC_STATE move(const char prev_pos, const char place_pos, const bool &piece) {
		if (place_pos >= SIZE || place_pos < 0) return FAIL;
		//if ((*this)(place_pos) != 0) return -1;

		// std::cout << piece << "'s chance.\n";
		// std::cout << "Move from (" << prev_pos / 6 << ", " << prev_pos % 6 << ") to (" 
		// << place_pos / 6 << ", " << place_pos % 6 << ")\n\n";

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = SPACE;
		step++;
		// cout << (*this) << '\n';

		// operator()(place_pos) = piece;
		return SUCCESS;
	}

	/*
	EXEC_STATE eat(const char prev_pos, const char place_pos, const bool &piece) {
		if (place_pos >= SIZE || place_pos < 0) return FAIL;
		//if ((*this)(place_pos) != (!piece) ) return FAIL;
		
		// cout << piece << "'s chance.\n";
		// cout << "Eat from (" << prev_pos / 6 << ", " << prev_pos % 6 << ") to (" 
		// << place_pos / 6 << ", " << place_pos % 6<< ")\n\n";

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = SPACE;
		step++;
		// cout << (*this) << '\n';
		// operator()(place_pos) = piece;
		return SUCCESS;
	}
*/
private:
	inline void reflect_vertical(char &pos) {
		// change current pos after reflection
		char x = pos / COL; char y = pos % COL;
		pos = (5-x) * COL + y;
		
		for (int c = 0; c < COL; c++) {
			std::swap(tile[0][c], tile[5][c]);
			std::swap(tile[1][c], tile[4][c]);
			std::swap(tile[2][c], tile[3][c]);
		}
	}

	inline void reflect_horizontal(char &pos) {
		// change current pos after reflection
		char x = pos / COL; char y = pos % COL;
		pos = x * COL + (5-y);
		
		for (int r = 0; r < COL; r++) {
			std::swap(tile[r][0], tile[r][5]);
			std::swap(tile[r][1], tile[r][4]);
			std::swap(tile[r][2], tile[r][3]);

		}
	}

	inline void transpose (char &pos) {
		// change current pos after transpose
		char x = pos / COL; char y = pos % COL;
		pos = y * COL + x;
		
		for (int r = 0; r < COL; r++) {
			for (int c = (r + 1); c < COL; c++) {
				std::swap(tile[r][c], tile[c][r]);
			}
		}
	}

	inline void rotate_right (char &pos) { transpose(pos); reflect_horizontal(pos); }

	inline void rotate_left (char &pos) { transpose(pos); reflect_vertical(pos); }

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
	friend std::ostream& operator<<(std::ostream& out, const board& b) {
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

			for (auto &t : row) {
				if (t==SPACE)
					out << std::setw(2) << 'x';
				else
					out << std::setw(2) << static_cast<int>(t);
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

};
