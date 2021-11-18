#pragma once
#include <string>
#include <cstdarg>
#include <algorithm>
#ifdef __linux__ 
#include <libfmt>
#elif _WIN32
#include <format>
#endif
#include "./RandomEngine.h"

class Player {
public:
	Player(const std::string& statline, RandomEngine* rng);
	inline void clear_mods();
	int roll_srv();
	int roll_rec(short mod, bool is_wipe);
	int roll_set(short mod, bool allow_devious);
	int roll_spk(short mod);
	int roll_blk(short mod);
	int roll_pss(short mod, double hd);
	short statSum();
	std::string to_string();
	short buff;
	short srv, rec, set, spk, blk;
	short mSrv, mRec, mSet, mSpk, mBlk;
	double height;
	std::string name;
	bool powerful = false,
		clever = false,
		complacent = false,
		resilient = false,
		resourceful = false,
		chaotic = false,
		devious = false,
		efficient = false,
		vigilant = false,
		unsuspecting = false,
		indecisive = false,
		arrogant = false,
		moody = false;
	RandomEngine* rng;
};

