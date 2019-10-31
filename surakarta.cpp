/**
The main program of Surakarta Game
-------------------------
This is the Project of Department of Computer Science in NCTU 
Construced by Clive Wu and Mark Chang
Instructed by Professor I-Chen Wu
**/
#include <iostream>
#include <iterator>
#include <unistd.h>
#include <typeinfo> 
#include <fstream>
#include "board.h"
#include "action.h"
#include "agent.h"

using namespace std;
int main(int argc, char* argv[]) {
	cout << "Surakarta Demo: ";
	// copy(argv, argv + argc, ostream_iterator<char *>(cout, " "));
	cout << "\n\n";

	size_t total = 1, block = 0;
	for (int i = 1; i < argc; i++) {
		string para(argv[i]);
		if (para.find("--total=") == 0) {
			total = stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--block=") == 0) {
			block = stoull(para.substr(para.find("=") + 1));
		}
	}


	board b;
	player env('B');//2
	player play('W');//1
	unsigned int i = 0; 
	while (i++ < total) {
		int idx = 0;
		agent win;

		while(true) {

			if((idx % 2) ) {
				action &a = env.take_action(b);//2

				if( a.apply(b) == -1) {
					cout << b << '\n';//last board
					win = play; break;
				}
				delete &a;

			}
			else {
				action &a = play.take_action(b);//1

				if ( a.apply(b) == -1) {
					cout << b << '\n';//last board
					win = env; break;
				}
			delete &a;

			}
			
			//check draw
			if (env.idle_step() >= 25 && play.idle_step() >= 25) {
				if (env.count_piece(b) > play.count_piece(b) ) {
					win = env; break;
				}
				else if (play.count_piece(b) > env.count_piece(b)) {
					win = play; break;
				}
			}

			cout << b << '\n';
			// sleep(4);
			idx++;
		}
		if ( win.get_piece() != 0)
			cout << "winner: " <<  win.get_piece() << '\n';
		else
			cout << "no winner, it's a draw.\n";
	}
	return 0;
}