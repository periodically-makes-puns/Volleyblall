#include "Player.h"

inline short bound(short v) {
	#ifdef _WIN32
	return (0i16 < v) ? ((v < 10i16) ? v : 10i16) : 0i16;
	#elif __linux__
	return (0 < v) ? ((v < 10) ? v : 10) : 0;
	#endif
}

Player::Player(const std::string& statline, RandomEngine* engine) {
	char trait1[20], trait2[20], name[120];

	this->rng = engine;
	#ifdef _WIN32
	sscanf_s(statline.c_str(), 
	#elif __linux__
	sscanf(statline.c_str(),
	#endif
		"%119[^|]| %*[^|]| %lf | %hd %hd %hd %hd %hd | %19s %19s", 
		#ifdef _WIN32
		name, 119, 
		#elif __linux__
		name,
		#endif
		&this->height, &this->srv, &this->rec, &this->set, 
		&this->spk, &this->blk, 
		#ifdef _WIN32
		trait1, 19, trait2, 19);
		#elif __linux__
		trait1, trait2);
		#endif
	this->clear_mods();
	this->buff = 0;
	name[119] = trait1[19] = trait2[19] = '\0';
	this->name = std::string(name);
	if (strlen(trait1) > 0) {
		std::string trait(trait1);
		this->powerful = trait == "powerful";
		this->clever = trait == "clever";
		this->complacent = trait == "complacent";
		this->resilient = trait == "resilient";
		this->resourceful = trait == "resourceful";
		this->chaotic = trait == "chaotic";
		this->devious = trait == "devious";
		this->efficient = trait == "efficient";
		this->vigilant = trait == "vigilant";
		this->unsuspecting = trait == "unsuspecting";
		this->indecisive = trait == "indecisive";
		this->arrogant = trait == "arrogant";
		this->moody = trait == "moody";

	}
	if (strlen(trait2) > 0) {
		std::string trait(trait2);
		if (trait == "powerful") this->powerful = true;
		if (trait == "clever") this->clever = true;
		if (trait == "complacent") this->complacent = true;
		if (trait == "resilient") this->resilient = true;
		if (trait == "resourceful") this->resourceful = true;
		if (trait == "chaotic") this->chaotic = true;
		if (trait == "devious") this->devious = true;
		if (trait == "efficient") this->efficient = true;
		if (trait == "vigilant") this->vigilant = true;
		if (trait == "unsuspecting") this->unsuspecting = true;
		if (trait == "indecisive") this->indecisive = true;
		if (trait == "arrogant") this->arrogant = true;
		if (trait == "moody") this->moody = true;
	}
}

void Player::clear_mods() {
	this->mSrv = this->mRec = this->mSet = this->mSpk = this->mBlk = 0;
}

int Player::roll_srv() {
	short skill = bound(this->srv + this->buff + mSrv);
	mSrv = 0;
	short res = rng->roll(RandomEngine::Action::SRV, skill, 0);
	if ((this->chaotic || this->powerful) && rng->getReal() < 0.03) {
		res = 0;
	}
	if (res == 3) res = 2;
	if (res >= 2) {
		if (this->chaotic && rng->getReal() < 0.2) {
			res = (short)(rng->getReal() * 7) + 9; // chaotic special case
		} else if (this->powerful && rng->getReal() < 0.2) {
			res = 3;
		}
	}
	return res;
}

int Player::roll_rec(short mod, bool is_wipe) {
	if (is_wipe) {
		if (this->vigilant && rng->getReal() < 0.3)
			mod += 3;
		if (this->unsuspecting)
			mod -= 3;
	}
	short skill = bound(this->rec + this->buff + mod + mRec);
	mRec = 0;
	short res = rng->roll(RandomEngine::Action::REC, skill, 0);
	if (res == 3) res = 5;
	return res;
}

int Player::roll_set(short mod, bool allow_devious) {
	short skill = bound(this->set + this->buff + mod + mSet);
	mSet = 0;
	short extra = (allow_devious && this->devious) ? 6 : 0;
	return rng->roll(RandomEngine::Action::SET, skill, extra);
}

int Player::roll_spk(short mod) {
	short skill = bound(this->spk + this->buff + mod + mSpk);
	mSpk = 0;
	short res = rng->roll(RandomEngine::Action::SPK, skill, 0);
	if (res == 1) res = -4;
	return res;
}

int Player::roll_blk(short mod) {
	short skill = bound(this->blk + this->buff + mod + mBlk);
	mBlk = 0;
	short extra = (this->efficient) ? 4 : 0;
	return rng->roll(RandomEngine::Action::BLK, skill, extra);
}

int Player::roll_pss(short mod, double hd) {
	if (hd < 0) hd = 0;
	short skill = bound(mod);
	int weights[5] = { 
		10 - skill, 
		skill, 
		(int) ((hd * hd) / 100.0),
		(this->powerful) ? 3 : 0,
		(this->clever) ? 3 : 0 
	};
	int SW[6];
	SW[0] = 0;
	for (int i = 0; i < 5; i++) {
		SW[i + 1] = SW[i] + weights[i];
	}
	return std::lower_bound(SW, SW + 6, rng->getReal() * SW[5]) - SW - 1;
}

short Player::statSum() {
	return this->srv + this->rec + this->set + this->spk + this->blk;
}

std::string Player::to_string() {
#ifdef __linux__
	return fmt::format("{} | {} | {} {} {} {} {}", name, height, srv, rec, set, spk, blk);
#elif _WIN32
	return std::format("{} | {} | {} {} {} {} {}", name, height, srv, rec, set, spk, blk);
#endif
}
