#pragma once

#include <unordered_map>
#include "board.h"
//record every moves


class action {
	public:
		action (bool piece = 0, char last_pos = -1, char new_pos = -1, char act = 'n') : 
		piece(piece), last_pos(last_pos), new_pos(new_pos), act(act) {}
		//default destructor
		virtual ~action() {};


		action(const action& a) : piece(a.piece), last_pos(a.last_pos), new_pos(a.new_pos), act(a.act){}

		class move;
		class eat;

	public:
		virtual EXEC_STATE apply(board &b){ 
			//find what action it act
			auto proto = entries().find(act);
			//reinterpret action to new action
			if (proto != entries().end() ) return proto->second->reinterpret(this).apply(b);
			return FAIL;
		}

	public:
		Pair get_positions() const{ return Pair(last_pos, new_pos); }

		unsigned get_piece() const { return piece; }

		char get_act() const { return act; }

	private:
		bool piece;
		char last_pos;
		char new_pos;
		char act;

	protected:
		typedef std::unordered_map<char, action*> prototype; //map that store two types of actin
		static prototype& entries() { static prototype m; return m; }
		virtual action& reinterpret(const action* a) const { return *new (const_cast<action*>(a)) action(*a); }

};

class action::move : public action {
public:	
	move(bool piece, char last_pos, char new_pos) : action(piece, last_pos, new_pos, 'm') {}
	move(const action &a = {}) : action(a) {}

public:
	EXEC_STATE apply(board &b) {
		//if failed return -1
		EXEC_STATE apply_state = b.move(last_pos, new_pos, piece);

		return apply_state;
	}

protected:
	action& reinterpret(const action* a) const { return *new (const_cast<action*>(a)) move(*a); }
	static __attribute__((constructor)) void init() { entries()['m'] = new move; }

};

class action::eat : public action {
public:
	eat(bool piece, char last_pos, char new_pos) : action(piece, last_pos, new_pos, 'e') {}
	eat(const action &a = {}) : action(a) {}

public:
	EXEC_STATE apply(board &b) {
		//if failed return -1
		EXEC_STATE apply_state = b.eat(last_pos, new_pos, piece);
		return apply_state;
	}
	
protected:

	action& reinterpret(const action* a) const { return *new (const_cast<action*>(a)) eat(*a); }
	static __attribute__((constructor)) void init() { entries()['e'] = new eat; }

};
