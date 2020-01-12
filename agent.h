#pragma once

#include<iostream>
#include<random>
#include<vector>
#include<algorithm>
#include "board.h"
#include "action.h"
#include "Node.h"
#include "MonteCarlo.h"

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
			this->piece = 0;
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

public:
//greedy method that always find eat step first
	action take_action(board &before) {

		if ( !before.check_Piece_onBoard(piece) ) return action();//lose
		// if (!check_Piece_onBoard(before)) return action();
		//find whether can eat piece or not
		
		MonteCarloTree tree;
		tree.reset(before);

		int simulationtime = 2000;
		int count_sim = 0;
		while (count_sim < simulationtime) {
			tree.tree_policy();
			count_sim++;
		}

		int offset = tree.root->best_child();
		// cout << tree.root_board << '\n';

		Node* tmp = tree.root->child;
		Pair best_move = (tmp + offset)->move;
		tree.root->showchild();

		cout << "piece: " << piece << '\n';
		cout << "move from " << best_move.first/6 << ' ' << best_move.first%6 << " to " << best_move.second/6 << ' ' << best_move.second%6 << '\n';
		// before.move(best_move.first, best_move.second, b.take_turn());
		tree.clear();
		return action::move(piece, best_move.first, best_move.second);
		


		//  --------Greedy Policy-------- //
		/*
		vector<Pair> pos = before.eat_piece(piece);
		// Pair pos = eat_piece(before);
		
		if (!pos.empty()) {
			//take eat_action
			count_not_eat = 0;
			return action::eat( piece, pos[0].last_pos, pos[0].new_pos );
		}
		else {
			//take move_action
			pos = before.move_piece(piece);
			// pos = move_piece(before);
			if ( !pos.empty() ) {
				count_not_eat++;
				random_device rv;
				default_random_engine engine(rv());

				shuffle(pos.begin(), pos.end(), engine);
				// cout << "move: " << pos[];

				return action::move( piece, pos[0].last_pos, pos[0].new_pos );
			}
		}
		*/

		//GameOver
		return action();
	}

private:
	int count_step;
	int count_not_eat;
};


class envir : public agent {

public:
	envir(char c) : count_step(0), count_not_eat(0) {
		if (c=='B') 
			this->piece = 0;
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

public:
//greedy method that always find eat step first
	action take_action(board &before) {

		if ( !before.check_Piece_onBoard(piece) ) return action();//lose
		// if (!check_Piece_onBoard(before)) return action();
		//find whether can eat piece or not
		

		//  --------Greedy Policy-------- //
		
		vector<Pair> pos = before.eat_piece(piece);
		// Pair pos = eat_piece(before);
		
		if (!pos.empty()) {
			//take eat_action
			count_not_eat = 0;
			cout << "piece: " << piece << '\n';
			cout << "eat from " << pos[0].last_pos/6 << ' ' << pos[0].last_pos%6 << " to " << pos[0].new_pos/6 << ' ' << pos[0].new_pos%6 << '\n';
			
			return action::eat( piece, pos[0].last_pos, pos[0].new_pos );
		}
		else {
			//take move_action
			pos = before.move_piece(piece);
			// pos = move_piece(before);
			if ( !pos.empty() ) {
				count_not_eat++;
				random_device rv;
				default_random_engine engine(rv());

				shuffle(pos.begin(), pos.end(), engine);
				// cout << "move: " << pos[];
				cout << "piece: " << piece << '\n';
				cout << "move from " << pos[0].last_pos/6 << ' ' << pos[0].last_pos%6 << " to " << pos[0].new_pos/6 << ' ' << pos[0].new_pos%6 << '\n';
				return action::move( piece, pos[0].last_pos, pos[0].new_pos );
			}
		}

		//GameOver
		return action();
	}

private:
	int count_step;
	int count_not_eat;
};