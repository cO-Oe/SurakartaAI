#pragma once

class bitboard {

public:
	unsigned long long b_board;

	bitboard() : b_board(0) {}
	
	int count() const {
		unsigned long long v;

		v = (b_board & 0x5555555555555555LL) + ( (b_board>>1) & 0x5555555555555555LL);
		v = (v & 0x3333333333333333LL) + ((v >> 2) & 0x3333333333333333LL);
		v = (v & 0x0f0f0f0f0f0f0f0fLL) + ( (v >> 4) & 0x0f0f0f0f0f0f0f0fLL);
		return  ( (v * 0x0101010101010101LL) >> 56);
	}
	void add (int pos) {
		b_board |= (1LL << (pos & 63));
	}
	void remove(int pos) {
		b_board &= ~(1LL << (pos & 63));
	}
	void move(int prev, int next) {
		// remove prev piece
		remove(prev);
		// add next piece
		add(next);
	}
	void swap(int p1, int p2) {
		bitboard tmp = (*this);
		
		if ( tmp.check(p1) ) {
			(*this).add(p2);
		}
		else {
			(*this).remove(p2);
		}

		if ( tmp.check(p2) ) {
			(*this).add(p1);
		}
		else {
			(*this).remove(p1);
		}
		
	}

	bool check(int pos) const {
		return ( b_board & ( 1LL << (pos & 63) ) ) != 0; 
	}

	void clear() { b_board = 0LL; }


};

