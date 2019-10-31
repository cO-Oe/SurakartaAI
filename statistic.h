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
		size_t sop = 0, play_eat = 0, play_move = 0, env_eat = 0, env_move = 0;
		time_t sdu = 0;
		auto it = data.end();

		for ( size_t i = 0; i < blk; ++i ) {
			auto& ep = *(--it);
			sop += ep.step();
			play_eat += ep.step(1);
			play_move += ep.step(2);
			env_eat += ep.step(3);
			env_move += ep.step(4);
			sdu += ep.time();
		}

		cout << "Steps:" << sop/blk << '\n';
		cout << "Player eats:" << play_eat/blk << '\n';
		cout << "Player moves:" << play_move/blk << '\n';
		cout << "Env eats:" << env_eat/blk << '\n';
		cout << "Env moves:" << env_move/blk << '\n';
		cout << "Time:" << sdu/blk << '\n';
	}

  bool is_finished() { return count >= total; }

  void open_episode(const string& flag = "") {
		count++;
		data.emplace_back();
		data.back().open_episode(flag);
	}

  void close_episode(const std::string& flag = "") {
		data.back().close_episode(flag);
		if (count % block == 0) cout << "Episode end.\n";
	}

  episode& back() { return data.back(); }
private:
	unsigned int total;
	unsigned int block;
	unsigned int count;
	list<episode> data;

};