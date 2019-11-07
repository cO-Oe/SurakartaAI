#pragma once

#include<iostream>
#include<random>
#include<vector>
#include<algorithm>
#include "board.h"
#include "action.h"

#define last_pos first
#define new_pos second
using namespace std;

/*

any rule for a player will write here


  0   1   2   3   4   5
  6   7   8   9  10  11
 12  13  14  15  16  17
 18  19  20  21  22  23 
 24  25  26  27  28  29
 30  31  32  33  34  35

*/

class agent {
public:
	agent(int piece = 0) : piece(piece) {}

public:
	virtual void open_episode(const string& flag = "") {}
	virtual void close_episode(const string& flag = "") {}
	virtual action take_action(board &b){return {}; }
	int get_piece(){return piece;}

protected:
	int piece;
};



class player : public agent {
public:
	player(char c) : count_step(0), count_not_eat(0) {
		if (c=='B') 
			this->piece = 2;
		if (c=='W')
			this->piece = 1;
	}

public:
	int idle_step() { return count_not_eat; }

	int count_piece(board &b) {
		int cnt = 0;
		for (int i = 0; i < 36; i++) {
			if(b(i) == piece) cnt++;
		}
		return cnt;
	}

public:
	int search_up(board &before, int &pos, bool pass) {
		this->count_step++;
		
		// has run a circle
		if (this->count_step >= 25) {
			this->count_step = 0;
			return -1;
		}

		if (before(pos) == piece) return -1;
		else if(before(pos) == (piece ^ 3)) {
			if(pass)
				return 1;
			else
				return -1;
		}
		pos -= 6;

		if (pos < 0) {
			switch(pos) {
				case -5: pos = 6; pass = true; return search_right(before, pos, pass); 
				case -4: pos = 12; pass = true; return search_right(before, pos, pass);
				case -3: pos = 17; pass = true; return search_left(before, pos, pass);
				case -2: pos = 11; pass = true; return search_left(before, pos, pass);
				default: pos = -1;
			}
		}
		if (pos != -1)
			return search_up(before, pos, pass);
		else 
			return -1;
	}

	int search_down(board &before, int &pos, bool pass) {
		before.reflect_vertical(pos);
		int ans = search_up(before, pos, pass);
		before.reflect_vertical(pos);
		return ans;
	}

	int search_right(board &before, int &pos, bool pass) {
		before.rotate_left(pos);
		int ans = search_up(before, pos, pass);
		before.rotate_right(pos);
		return ans;
	}

	int search_left(board &before, int &pos, bool pass) {
		before.rotate_right(pos);
		int ans = search_up(before, pos, pass);
		before.rotate_left(pos);
		return ans;
	}

public:

	int check_eat(board &before, int pos) {
		if (pos == 0 || pos == 5 || pos == 30 || pos == 35)
			return -1;
		//search four way and return position when have eatable siece
		before(pos) = 0;

		int prev_pos = pos;
		int ans = -1;
		vector<int> p;
		if ( search_up(before, pos, false) != -1) p.push_back(pos);	  
		pos = prev_pos; this->count_step = 0;
		if ( search_down(before, pos, false) != -1) p.push_back(pos); 
		pos = prev_pos; this->count_step = 0;
		if ( search_right(before, pos, false) != -1) p.push_back(pos); 
		pos = prev_pos; this->count_step = 0;
		if ( search_left(before, pos, false) != -1) p.push_back(pos);  
		pos = prev_pos; this->count_step = 0;

		before(prev_pos) = this->piece;

		if (p.empty())
			return ans;
		else
			return p[0];//return ramdomly
	}
//check whether the moves are available
	vector<int> check_move(board &before, int pos) {
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
			if ( before(pos + d) == 0 ){
				ans.push_back(pos + d);
			}
		}
		return ans;
	}

	Pair eat_piece(board &before) {
		for (int i=0; i<36; i++) {
			int tile = before(i);
			if (tile == piece) {
				int eat_pos = check_eat(before, i);
				
				if (eat_pos != -1)
					return make_pair(i, eat_pos);
				
			}
		}
		return make_pair(-1, -1);
	}

	Pair move_piece(board &before) {
		vector< Pair > ans; //store available moves pairs(now and next)
		vector< int > move_pos;//store available next move 

		for (int now_pos = 0; now_pos < 36; now_pos++) {
			int tile = before(now_pos);
			if (tile == piece) {		
				move_pos = check_move(before, now_pos);
				if ( !move_pos.empty() ) {
					for (auto &next_pos : move_pos)
						ans.push_back( make_pair(now_pos, next_pos) );
				}
			}
		}
		//random select a move
		shuffle(ans.begin(), ans.end(), engine);
		return ans[0];
	}
	
	//check whether there are still pieces on board
	int check_Piece_onBoard (board &before) const {
		bool find = 0;
		for (int i=0; i<36; i++) {
			if ( before(i) == piece){
				find = 1; break;
			}
		}
		if(find) return 1;
		else return 0;
	}

public:
//greedy method that always find eat step first
	action take_action(board &before) {

		if ( !check_Piece_onBoard(before) ) return action();//lose

		//find whether can eat piece or not
		Pair pos = eat_piece(before);
		if (pos.last_pos != -1) {
			//take eat_action
			count_not_eat = 0;
			return action::eat( piece, pos.last_pos, pos.new_pos );
		}
		else {
			//take move_action
			pos = move_piece(before);

			if (pos.last_pos != -1) {
				count_not_eat++;
				return action::move( piece, pos.last_pos, pos.new_pos );
			}
		}
		//GameOver
		return action();
	}

private:
	int count_step;
	int count_not_eat;
	default_random_engine engine;
};
