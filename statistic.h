#pragma once

#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "board.h"
#include "action.h"
#include "agent.h"
#include "episode.h"

class statistic {
public:
	/**
	 * the total episodes to run
	 * the block size of statistic
	 *
	 * note that total >= block
	 */
	statistic(unsigned int total, unsigned int block = 0 )
		: total(total),
		  block(block ? block : total),
		  count(0) {}

public:
	void show () const {
		int blk = block;
		// size_t play_eat = 0, play_move = 0, env_eat = 0, env_move = 0;
		size_t sop = 0, pop = 0, eop = 0;
		time_t sdu = 0, pdu = 0, edu = 0;
		auto it = data.end();
		static int total = 0;
		static int player_win = 0;
		total += blk;
		for ( int i = 0; i < blk; ++i ) {
			auto& ep = *(--it);

			player_win += (ep.who_win == "play") ? 1: 0;

			sop += ep.step();
			// pop += (ep.step() / 2 ) + ((ep.step() % 2) ? 1 : 0);
			// eop += ep.step() / 2;
			pop += ep.step('p');
			eop += ep.step('e');
			
			// play_eat += ep.step(1);
			// play_move += ( ep.step() / 2 )+ ( ep.step() % 2 );
			// env_eat += ep.step(3);
			// env_move += ep.step() / 2;
			
			sdu += ep.time();
			pdu += ep.time('p');
			edu += ep.time('e');
		}
		int env_win = total - player_win;
		//print win rate
		cout << "In " << count << " games:\n\n";
		cout << "player win: " << player_win << " games\n";
 		cout << "env win: " << env_win << " games\n\n";
		cout << "player win rate: " << static_cast<double>(player_win) / total * 100.0 << "%\n";
		cout << "env win rate: " << double(env_win) / total * 100.0 << "%\n";
		cout << "ops: " << (sop * 1000.0) / sdu << '(' << (pop * 1000.0) / pdu << " | " << 
		(eop * 1000.0) / edu << ')'<< "(player_op | env_op)" << '\n';


		cout << "-------------------------------------\n\n";

		// cout << "Steps:" << sop/blk << '\n';
		// cout << "Player eats:" << play_eat/blk << '\n';
		// cout << "Player moves:" << play_move/blk << '\n';
		// cout << "Env eats:" << env_eat/blk << '\n';
		// cout << "Env moves:" << env_move/blk << '\n';
		// cout << "Time:" << sdu/blk << '\n';
	}

  bool is_finished() { return count >= total; }

  void open_episode(const string& flag = "") {
		count++;
		data.emplace_back();
		data.back().open_episode(flag);
	}

  void close_episode(const std::string& flag = "") {
		data.back().close_episode(flag);
		if (count % block == 0) show();
	}

  episode& back() { return data.back(); }
private:
	unsigned int total;
	unsigned int block;
	unsigned int count;
	list<episode> data;

};