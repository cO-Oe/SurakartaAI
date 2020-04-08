#pragma once

#include "MonteCarlo.h"
#include "policy.h"
#include <functional>
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
	agent(PIECE piece) : piece(piece), count_step(0), count_not_eat(0) {}

public:
	
	virtual void open_episode (const std::string& flag = "") {}
	virtual void close_episode (const std::string& flag = "") {}
	virtual Pair take_action (board &before) { return {}; }
	
	virtual std::string name() const { return "agent"; }
	

	PIECE get_piece() const { return piece; }
	
	// count opponent piece?
	int count_piece (const board &b) const {
		int cnt = 0;
		for ( int i = 0; i < board::SIZE; i++ ) {
			if ( b(i) == (this->get_piece() ^ 1) ) 
				cnt++;
		}
		return cnt;
	}
	bool operator==(agent &a) {
		return (piece==a.piece);
	}

	inline int idle_step() const { return count_not_eat; }

protected:
	const PIECE piece;
	int count_step;
	int count_not_eat;
	
	void print_pos (const Pair &pos) const{
		std::cout << "piece: " << piece << '\n';
		std::cout << "move from " << pos.prev / 6 << ' ' << pos.prev % 6 << " to " << pos.next / 6 << ' ' << pos.next % 6 << '\n';
	}

};


class player : public agent {
public:
	player(PIECE p) : agent(p){

	}
	
	virtual Pair take_action (board &before) override{	
		
		// Pair mv = Policy::MCTS(before, piece, 5000);
		Pair mv = Policy::NN(before, piece);
		EXEC_STATE S = before.move(mv.prev, mv.next, piece);
		if (S==FAIL) {
			return Pair{};
		}
		else
			return mv;
	}
	virtual std::string name() const override {return "player"; }

};


class envir : public agent {

public:
	envir(PIECE p) : agent(p){
		
	}

	virtual Pair take_action (board &before) override {
		
		Pair mv = Policy::Greedy(before, piece);
		EXEC_STATE S = before.move(mv.prev, mv.next, piece);
		if (S==FAIL) {
			return Pair{};
		}
		else
			return mv;
	}
	virtual std::string name()  const override {return "envir";}

};

