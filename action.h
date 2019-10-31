#pragma once

#include<iostream>
#include<unordered_map>
#include "board.h"
//record every moves
typedef pair<int, int> Pair;


class action {
	public:
		action(int piece = -1, int last_pos = -1, int new_pos = -1, char act = 'n') : 
		piece(piece), last_pos(last_pos), new_pos(new_pos), act(act) {}

		action(const action& a) : piece(a.piece), last_pos(a.last_pos), new_pos(a.new_pos), act(a.act){}

		class move;
		class eat;

	public:
		virtual int apply(board &b){ 
			auto proto = entries().find(act);
			if (proto != entries().end() ) return proto->second->reinterpret(this).apply(b);
			return -1;
		}

	public:
		Pair get_positions() { return Pair(last_pos, new_pos); }

		unsigned get_piece() { return piece; }

		char get_act() const { return act; }

	private:
		int piece;
		unsigned last_pos;
		unsigned new_pos;
		char act;

	protected:
		typedef std::unordered_map<char, action*> prototype;
		static prototype& entries() { static prototype m; return m; }
		virtual action& reinterpret(const action* a) const { return *new (const_cast<action*>(a)) action(*a); }

};

class action::move : public action {
public:	
	move(int piece, int last_pos, int new_pos) : action(piece, last_pos, new_pos, 'm') {}
	move(const action &a = {}) : action(a) {}

public:
	int apply(board &b) {
		int apply_state = b.move(last_pos, new_pos, piece);

		return apply_state;
	}

protected:
	action& reinterpret(const action* a) const { return *new (const_cast<action*>(a)) move(*a); }
	static __attribute__((constructor)) void init() { entries()['m'] = new move; }

};

class action::eat : public action {
public:
	eat(int piece, int last_pos, int new_pos) : action(piece, last_pos, new_pos, 'e') {}
	eat(const action &a = {}) : action(a) {}

public:
	int apply(board &b) {
		int apply_state = b.eat(last_pos, new_pos, piece);
		return apply_state;
	}
	
protected:
	action& reinterpret(const action* a) const { return *new (const_cast<action*>(a)) eat(*a); }
	static __attribute__((constructor)) void init() { entries()['e'] = new eat; }
};