#include<iostream>
#include<vector>
#include "board.h"
#include "action.h"

using namepsace std;
class episode {
	public:
		episode() : ep_state(initial_state()) {ep.moves.reserve(1000);}
	public:
		board &state() {return ep_state; }
		void open_episode(){

		}
		void close_episode(){

		}
		bool apply_action(action move) {
			int stat = move.apply(state());
			if (stat == -1) return false;
			ep_moves.emplace_back(move);
			
			return true;
		}
		agent& take_turns() {
			
		}
	private:
		board ep_state;
		vector<action> ep_moves;
}