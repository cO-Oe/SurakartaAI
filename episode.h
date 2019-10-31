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

  void apply_action(action move) {
		ep_moves.emplace_back(move, 0, millisec() - ep_time);
	}

  agent& take_turns(agent& play, agent& env) {
		ep_time = millisec();
		return ((step() + 1) % 2) ? play : env;
	}

	agent& last_turns(agent& play, agent& env) {
		return take_turns(play, env);
	}


public:
	unsigned step (unsigned who = 0) const {
		unsigned size = 0;
		switch (who) {
		case 1:
			for ( size_t i = 0; i < ep_moves.size(); i+=2 ) 
				if ( ep_moves[i].code.get_act() == 'e' )
					size++;
			break;
		case 2:
			for ( size_t i = 0; i < ep_moves.size(); i+=2 ) 
				if ( ep_moves[i].code.get_act() == 'm' )
					size++;
			break;
		case 3:
			for ( size_t i = 1; i < ep_moves.size(); i+=2 ) 
				if ( ep_moves[i].code.get_act() == 'e' )
					size++;
			break;
		case 4:
			for ( size_t i = 1; i < ep_moves.size(); i+=2 ) 
				if ( ep_moves[i].code.get_act() == 'm' )
					size++;
			break;
		default :
			size = ep_moves.size(); // 'int' is important for handling 0
		}
		return size;
	}

	time_t time () const {
		return ep_close.when - ep_open.when;
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
		return {};
	}

	static time_t millisec() {
		auto now = chrono::system_clock::now().time_since_epoch();
		return chrono::duration_cast<chrono::milliseconds>(now).count();
	}

public:
	board ep_state;
private:
	vector<move> ep_moves;
	time_t ep_time;
	meta ep_open;
	meta ep_close;
};