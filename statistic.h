#pragma once

#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "board.h"
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
	//show winrate and operates per second (time not completed)
	void show () const {
		int blk = block;
		
		std::cout << "#Game     #Winner     #left_pieces     #Moves     #player_time     #env_time\n";

		int game_id = 1;
		std::cout << std::setiosflags(std::ios::right);
		for ( auto &it : data ) {
			std::cout << std::setw(5) << game_id++ << std::setw(12) <<  it.who_win << std::setw(16) << it.win_piece << std::setw(10) << it.ep_moves.size() <<
			std::setw(14) << it.time('p') << "(ms)"  << std::setw(10) << it.time('e') << "(ms)" << '\n';
		}

		std::size_t sop = 0, pop = 0, eop = 0;
		std::time_t sdu = 0, pdu = 0, edu = 0;
		auto it = data.end();
		static int total = 0;
		static int player_win = 0;
		static int none_win = 0;
		total += blk;
		for ( int i = 0; i < blk; ++i ) {
			auto& ep = *(--it);
			
			if ( ep.who_win == "player" )
				++player_win;
			else if ( ep.who_win != "envir" )
			 	++none_win;
			

			sop += ep.step();
			pop += ep.step('p');
			eop += ep.step('e');
			
			sdu += ep.time();
			pdu += ep.time('p');
			edu += ep.time('e');
		}
		int env_win = total - player_win - none_win;
		static int total_nodraw = player_win + env_win;
		//print win rate
		std::cout << "In " << count << " games:\n\n";
		std::cout << "player win: " << player_win << " games\n";
 		std::cout << "env win: " << env_win << " games\n\n";
		std::cout << "player win rate: " << static_cast<double>(player_win) / total_nodraw * 100.0 << "%\n";
		std::cout << "env win rate: " << double(env_win) / total_nodraw * 100.0 << "%\n";
		//cout << sdu / 1000.0 << ' ' << pdu / 1000.0 << ' ' << edu / 1000.0 << '\n';
		std::cout << "ops: " << (sop * 1000.0) / sdu << '(' << (pop * 1000.0) / pdu << " | " << 
		(eop * 1000.0) / edu << ')'<< "(player_op | env_op)" << '\n';


		std::cout << "-------------------------------------\n\n";
	}

	//check if games cnt == total (finished) 
  bool is_finished() { return count >= total; }

	//append an new episode
  void open_episode(const std::string& flag = "") {
		count++;
		data.emplace_back();
		data.back().open_episode(flag);
	}

	//episode end and show stats
  void close_episode(const std::string& flag, const agent *winner, const board &b) {
		data.back().close_episode(flag, winner, b);
		if (count % block == 0) show();
	}

	//access the last episode
  episode& back() { return data.back(); }
private:
	unsigned int total;
	unsigned int block;
	unsigned int count;
	std::list<episode> data;

};
