#pragma once 

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <numeric>
#include "board.h"
#include "action.h"
#include "agent.h"

using namespace std;

class statistic;

class episode {
friend class statistic;

public:
	episode() : ep_state(initial_state()), ep_time(0) { ep_moves.reserve(10000); }

public:
	board& state() { return ep_state; }

	const board& state() const { return ep_state; }

  void open_episode(const string& tag) {
		ep_open = { tag, millisec() };
	}

	void close_episode(const string& tag) {
		ep_close = { tag, millisec() };
	}

	//save every action in episode
  bool apply_action(action move) {
  		// if ( move.apply(state()) == -1) return false;

		ep_moves.emplace_back(move, 0, millisec() - ep_time);
		return true;
	}

	//decide whose turn
  agent& take_turns(agent& play, agent& env) {
		ep_time = millisec();
		return ((step() + 1) % 2) ? play : env;
	}

	//see which player moves last -> winner
	agent& last_turns(agent& play, agent& env) {
		agent &tmp = take_turns(play, env);
		who_win =  ( tmp.get_piece() == 1) ? "play":"env" ;
		return tmp;
	}


public:
	//count moves of each player (cnt and type)
	unsigned step (char who = 'n') const {
		// unsigned size = 0;
		int siz = ep_moves.size();
		switch (who) {
			case 'p': return (siz / 2 ) + ((siz % 2) ? 1 : 0);
			case 'e': return siz / 2;
		default :
			return siz; // 'int' is important for handling 0
		}
	}

	//count time cost in an episode
	time_t time (char who = 'n') const {
		time_t time = 0;
		size_t i = 0;
		switch(who) {
			case 'p':
				i = 0;
				while( i < ep_moves.size() ) time += ep_moves[i].time, i += 2;
				break;
			case 'e':
				i = 1;
				while( i < ep_moves.size() ) time += ep_moves[i].time, i += 2;
				break;

			default: 
				time = ep_close.when - ep_open.when;
				break;
		}
		return time;
	}

protected:  
struct move {
	action code;
	int reward;
	time_t time;

	move(action code = {}, int reward = 0, time_t time = 0) : code(code), reward(reward), time(time) {}
	
	operator action() const { return code; }

};

struct meta {
		string tag;
		time_t when;
		meta(const string& tag = "N/A", time_t when = 0) : tag(tag), when(when) {}

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
		auto now = chrono::system_clock::now().time_since_epoch();
		return chrono::duration_cast<chrono::milliseconds>(now).count();
	}

public:
	board ep_state;
private:
	vector<move> ep_moves;
	string who_win;
	time_t ep_time;
	meta ep_open;
	meta ep_close;
};