#pragma once

#include<array>
#include<iostream>
#include<iomanip>
#include<unistd.h>
using namespace std;

/*
surakarta game board
any change in board will write here
*/

class board {
public:
	typedef array<int, 6> row;
	typedef array<row, 6> grid;

	board() :tile() {
		for (unsigned int i = 0; i < 6; i++) {
			for (unsigned int j = 0; j < 6; j++) {
				if (i <= 1) tile[i][j] = 1;
				if (i <= 3 && i > 1) tile[i][j] = 0;
				else if (i <= 5 && i > 3) tile[i][j] = 2;
			}
		}
	}

	int& operator()(int i) { return tile[ i / 6 ][ i % 6 ]; }

	board& operator =(const board &b) = default;

	/* 
	piece:
	0:no piece 
	1:black 
	2:white
	*/

	//move or eat a piece
	int move(int prev_pos, int place_pos, int piece) {
		if (place_pos > 35 || place_pos < 0) return -1;
		if ((*this)(place_pos) != 0) return -1;

		cout << piece << "'s chance.\n";
		cout << "Move from (" << prev_pos / 6 << ", " << prev_pos % 6 << ") to (" 
		<< place_pos / 6 << ", " << place_pos % 6 << ")\n\n";

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = 0;
		// operator()(place_pos) = piece;
		return 0;
	}

	int eat(int prev_pos, int place_pos, int piece) {
		if (place_pos > 35 || place_pos < 0) return -1;
		if ((*this)(place_pos) != (piece ^ 3) ) return -1;
		
		cout << piece << "'s chance.\n";
		cout << "Eat from (" << prev_pos / 6 << ", " << prev_pos % 6 << ") to (" 
		<< place_pos / 6 << ", " << place_pos % 6<< ")\n\n";

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = 0;
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
			" ┏━━━━━━━┓ ┏━━━━━━━┓  
			  ┃ ┏━━━┓ ┃ ┃ ┏━━━┓ ┃
			  ┃ ┃ 1 1 1 1 1 1 ┃ ┃ 
			  ┃ ┗ 1 1 1 1 1 1 ┛ ┃
			  ┗━━ 0 0 0 0 0 0 ━━┛
			  ┏━━ 0 0 0 0 0 0 ━━┓
			  ┃ ┏ 2 2 2 2 2 2 ┓ ┃
        ┃ ┃ 2 2 2 2 2 2 ┃ ┃
        ┃ ┗━━━┛ ┃ ┃ ┗━━━┛ ┃
        ┗━━━━━━━┛ ┗━━━━━━━┛
			"
		*/
	friend ostream& operator<<(ostream& out, const board& b) {
		out << "+--------------------+" << '\n';

		out << "┏━━━━━━━┓ ┏━━━━━━━┓" << '\n';
		out << "┃ ┏━━━┓ ┃ ┃ ┏━━━┓ ┃" << '\n';
		
		int idx = 0;
		for (auto &row : b.tile) {
			switch(idx) {
				case 0: out << "┃ ┃"; break;
				case 1: out << "┃ ┗"; break;
				case 2: out << "┗━━"; break;
				case 3: out << "┏━━"; break;
				case 4: out << "┃ ┏"; break;
				case 5: out << "┃ ┃"; break;
			}

			for (auto t : row) out << setw(2) << t;

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
		out << "┃ ┗━━━┛ ┃ ┃ ┗━━━┛ ┃" << '\n';
		out << "┗━━━━━━━┛ ┗━━━━━━━┛" << '\n';
		out << "+--------------------+" << '\n';
		return out;
	}

private:
	grid tile;
};