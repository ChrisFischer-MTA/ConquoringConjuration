#pragma once
class events
{
public:
	events();
	~events();
	void setTestingTrue();
	int getRandNum(int lowbound, int highbound);
	void mouse_move_double(int x, int y);
	void event_score_continue();
	void event_play_continue();
	void event_game_end();
	void event_anti_afk();
	void event_move_x();
	void event_click_conj();
	void event_game_start();
	void event_move_right();
private:
	bool testing;
};

