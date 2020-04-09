#pragma once 

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <numeric>
#include "agent.h"


class statistic;

class episode {
friend class statistic;

public:
	episode() : ep_state(initial_state()), ep_time(0) { ep_moves.reserve(1000); }

public:
	board& state() { return ep_state; }

	const board& state() const { return ep_state; }

    void open_episode(const std::string& tag) {
		ep_open = { tag, millisec() };
	}

	void close_episode(const std::string &tag, const agent &winner, const board &b) {
		who_win = winner.name();
		win_piece = b.count_piece(winner.get_piece());
		result = (winner.get_piece()==BLACK) ? 0:1;
		ep_close = { tag, millisec() };
	}

	//save every action in episode
    void record_action(const Pair &move, const board &b, const PIECE &piece) {
		ep_moves.emplace_back( move, 0, millisec() - ep_time );
		ep_boards.emplace_back(b);
		ep_pieces.emplace_back(piece);
	}


	agent& winner(agent& play, agent& env) {
		return (take_turns(play, env) == play) ? env : play;
	}

	//decide whose turn
    agent& take_turns(agent& play, agent& env) {
		ep_time = millisec();
		
		if (env.get_piece() == BLACK)
			return ((step() + 1) % 2) ? env : play;
		else
			return ((step() + 1) % 2) ? play : env;
	}

public:
	//count moves of each player (cnt and type)
	std::size_t step (const char who = 'n') const {
		// std::size_t total_step = ep_moves.size();
		
		switch (std::size_t total_step = ep_moves.size(); who) {
			case 'p': 
				return (total_step / 2 ) + ((total_step % 2) ? 1 : 0);
			case 'e': 
				return total_step / 2;
		default :
			return total_step; // 'int' is important for handling 0
		}
	}

	//count time cost in an episode
	std::time_t time (const char who = 'n') const {
		std::time_t time_cost = 0;
		
		switch(who) {
			case 'p':
				for ( std::size_t i{0}; i < ep_moves.size(); i+=2 ) 
					time_cost += ep_moves[i].time;
				break;
			case 'e':
				for ( std::size_t i{1}; i < ep_moves.size(); i+=2 ) 
					time_cost += ep_moves[i].time;
				break;

			default: 
				time_cost = ep_close.when - ep_open.when;
				break;
		}
		return time_cost;
	}

protected:  
struct move {
	Pair code;

	int reward;
	std::time_t time;

	move(Pair code = {}, int reward = 0, time_t time = 0) : 
	code(code), reward(reward), time(time) {}
	
};

struct meta {
		std::string tag;
		time_t when;
		meta(const std::string& tag = "N/A", time_t when = 0) : tag(tag), when(when) {}

		// friend ostream& operator <<(ostream& out, const meta& m) {
		// 	return out << m.tag << "@" << dec << m.when;
		// }
		// friend istream& operator >>(istream& in, meta& m) {
		// 	return getline(in, m.tag, '@') >> dec >> m.when;
		// }
	};

	static board initial_state() {
		return board();
	}

	static time_t millisec() {
		auto now = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	}

public:
	board ep_state;
	std::vector<move> ep_moves;
	std::vector<board> ep_boards;
	std::vector<PIECE> ep_pieces;
	int result;
private:
	std::string who_win;
	unsigned win_piece;
	time_t ep_time;
	meta ep_open;
	meta ep_close;
};
