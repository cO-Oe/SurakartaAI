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
		count = 1;
	    win = 0;
		//means = 0.5;
		c_size = 0;
		child = NULL;
	}
	void addresult(double result) {
		if ( (result > 0 && color==BLACK) || (result == 0 && color==WHITE) ) {
			win++;
			//means = (means*count+1.00)/(count+1);
		}
	//	else
			//means = (means*count)/(count+1);

		count++;
	}
	void expand(board &b) {

		// int c = !b.take_turn();
		int c = b.take_turn();
		c_size = 0;

		// for (int i=0; i<BoardSize; i++) {
			// if (b.check(i, c))
				// c_size++;
		// }
		int tmp;
		if (c==0)
			tmp = 2;
		else
			tmp = 1;
		vector<Pair> mv = b.get_available_move(tmp);

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
		for(int i=0; i < c_size; i++) {
			if (child[i].count > 5) {
				//cerr << transform_vertex( (int)child[i].place) << ' ' << child[i].win/child[i].count << ' ' << child[i].count << ' ';
				cerr << "\n\n";
			}
		}
	}


};