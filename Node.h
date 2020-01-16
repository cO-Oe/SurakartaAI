#pragma once

#include "board.h"


class Node {
public:
	Node* child;
	int color;
	// char place;
	Pair move;
	int c_size;
	double count;
	double win;
	//double means;

public:		
	Node() {}
	~Node() {
		if(child != NULL) {
			delete [] child;child = NULL;
		}
	}
	void init_Node(Pair m, int piece) {
		color = piece;
		move = m;
		count = 0;
	    win = 0;
		//means = 0.5;
		c_size = 0;
		child = NULL;
	}
	void addresult(double result) {
		if (result == 0.5)
			count++;
		else if ( (result > 0 && color==BLACK) || (result == 0 && color==WHITE) ) {
			count++;
			win++;
			//means = (means*count+1.00)/(count+1);
		}
		else
			count++;
	//	else
			//means = (means*count)/(count+1);

	}
	void expand(board &b) {
		// cout << "expand node!\n\n";
		// int c = !b.take_turn();
		int c = b.take_turn();
		c_size = 0;

		// for (int i=0; i<BoardSize; i++) {
			// if (b.check(i, c))
				// c_size++;
		// }

		vector<Pair> mv = b.get_available_move(c);

		c_size = mv.size();

		if (c_size == 0)
			return;

		child = new Node[c_size];
		int idx = 0;
		for (auto &p : mv) {
			child[idx].init_Node(p, c);
			idx++;
		}
		// int idx = 0;
		// for (int i=0; i<BoardSize; i++) {
		// 	if (b.check(i, c)) {
		// 		child[idx].init_Node(i, c);

		// 		idx++;
		// 	}
		// }
	}

	int best_child() {
		int ind = -1;
		double maxx = -1, tmp;

		for (int i=0 ;i < c_size; i++) {
			tmp = child[i].count;
			// cout << "piece: " << child[i].color;
			// cout << "move: " << child[i].move.first << ' ' << child[i].move.second << '\n';
			if (tmp > maxx) {
				maxx = tmp;
				ind = i;
			}
		}

		return ind;
	}
	void showchild() {
		cerr << setw(6) <<"from:";
		cerr << setw(10) << "to:";
		cerr << "  win rate     count\n";
		for(int i=0; i < c_size; i++) {
			if (child[i].count >= 0) {
				cerr << "(" << child[i].move.first/6 << ", "<< child[i].move.first%6  << ") to (" << child[i].move.second/6 << ", " <<child[i].move.second%6 << ")  " 
				<< child[i].win/child[i].count << ' ' << child[i].count << ' ';
				cerr << "\n\n";
			}
		}
	}


};