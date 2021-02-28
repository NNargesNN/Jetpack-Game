#include "stdafx.h"
#include <iostream>
#include "SBDL.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include<fstream>
#include<ctime>

using namespace std;

fstream infile;

struct Point
{
	int x, y;
};

struct Coin
{
	Point p;
	bool eaten = false;
	SDL_Rect coinrect;
};

enum gamestatus {
	play, pause, options, firstpage, setting_from_first, setting_from_pause, lose
};

gamestatus GameStatus = firstpage;

enum Type
{
	zappers_h, zappers_v, zappers_d, laser_nonactive, laser_active, caution1, caution2, missile
};

enum Appearance
{
	normal, gravity, alienapp, speedtokenapp_normal, speedtokenapp_alien
};

struct obstract
{
	Point p;
	bool eaten = false;
	Type type;
	SDL_Rect obs_rect;
};
obstract  obs;

//vectors:
vector <Coin> coins;
vector <obstract>obstracts;
vector<vector<int> > pattern;
vector<int> eachline;

//Textures:
//coin textures:
Texture coin[6];
//laser textures:
Texture nonactivelaser, activelaser1;
//barry textures:
Texture barrywalk[2], barryup, barrydown, barryspeed, alienwalk[2], alienup, aliendown, gravitydownwalk[2], gravityupwalk[2], gravityup, gravitydown;
//gravity:
Texture gravity_token_key;
//missle textures:
Texture warning, warning2, missle[6];
//missle smoke textures:
Texture smoke[6];
//speed textures:
Texture speedtoken[4];
//zappers textures:
Texture diagonal[4], vertical[4], horizontal[4];
//backgrounds textures:
Texture background[15];

Music* music;
Sound* coinsound;
Sound* zappersound;
Sound* nonactivelasersound;
Sound* warningsound;
Sound* keysound;
Sound* gravityupsound;
Sound* gravitydownsound;

Font* font;
Font*  fontb;
//MENU:
Texture firstBackground, pausekey, resumekey, quitkey, retrykey, barrybackground, highscoreskey, settingkey, playkey, sound_on, sound_off, back, change_barry, revive;

//function prototypes:
void loadassets();
void initialize_cordinates();
void initialize_obstract();
void initialize_coin_vector();
void mane();
void show_firstpage();
void show_pause();
void show_setting();
void show_backgrounds();
void show_barry();
void show_obstracts();
void show_coins();
void show_lose();
void show_gravity_key();
void show_speed_key();
void show_score();
void go_up_acceleration();
void go_down_acceleration();

//global variables:
bool goup = false, godown = false;
bool mutebkgmusic = false;
int gamespeed = 5;
const double delta_time = 0.03;
std::clock_t laser_start, missile_start, zapper_start, speed_start,coin_start;
int  window_width = 1500, window_height = 460;
double duration_laser, duration_missile, duration_zapper, duration_speed,duration_coin;
int barry_y = 350;
float barry_velocity = 0, barry_accleration = 0, delta_y = 0;
const float  const_accleration = 200;
int score = 0, scorecoin = 0;
int  xg = (rand() % 10) * 50 + 2*window_width;
int xs = (rand() % 10) * 70 + 3*window_width;
int ys = 100 + 100 * sin((double)xs / 50);
int lasertimer = 0;
bool showspeedtokenkey = false;
const int barrywalk_y = 350, barrywalkup_y = 30;
int coinfile_num = 1;
int scoremeter = 0, metercounter = 0;
SDL_Rect obsrect;
SDL_Rect backgroundrect = { 0,0,window_width, window_height };
Appearance appearance = normal,prev_appearance=normal;
SDL_Rect barry_rect;
SDL_Rect gravity_rect = { xg,70,80,80 } , speed_rect={ xs,ys,80,80 };
SDL_Rect pause_rect = { 1430,20,40,40 };
SDL_Rect retry_rect = { 620,350,250,100 };
SDL_Rect quit_rect = { 320,350,250,100 };
SDL_Rect resume_rect = { 920,350,250,100 };
SDL_Rect revive_rect = { 1000,300,150,150 };
SDL_Rect exit_rect = { 520,350,200,100 };
SDL_Rect play_rect = { 220,350,200,100 };
SDL_Rect setting_rect = { 820,350,200,100 };
SDL_Rect setting_rect2 = { 620,200,250,100 };
SDL_Rect highscores_rect = { 1120,350,200,100 };
SDL_Rect sound_rect = { 1120,350,200,100 };
SDL_Rect back_rect = { 220,350,200,100 };
SDL_Rect change_barry_rect = { 620,350,300,100 };
SDL_Rect barrybackground_rect = { 300,0,window_width - 600,window_height - 200 };
SDL_Rect currentrect, commingrect;
bool havelaser = false, havezapper = false,havemissile=false;
int xcurrentback = 0, xcommingback = window_width;
int  walkcounter = 0, coincounter = 0, bkgcounter = 0, smokecounter = 0, zappercounter = 0,token_counter=0;
bool play_zapper_sound = false;
bool play_laser_sound = false;
bool play_missile_sound = false;
bool play_warning_sound = false;
bool grav_up_sound = false;
bool grav_down_sound = false;
const int velocity_initial=7;
const int gamespeedfast=30;
const int meterunitfast = 5;
bool normalspeed = true;
const int meter_unit = 40;
float obj_distance;
int main(int argc, const char* argv[])
{
	SBDL::InitEngine("Jetpack Joyride", window_width, window_height);
	loadassets();
	initialize_cordinates();
	SBDL::playMusic(music, -1);

	while (SBDL::isRunning())
	{
		SBDL::updateEvents();
		SBDL::clearRenderScreen();

		if (GameStatus == firstpage)
		{
			show_firstpage();
		}

		else if (GameStatus == setting_from_first || GameStatus == setting_from_pause)
		{
			show_setting();
		}

		else if (GameStatus == play)
		{
			//count meters:
			//sorate mamoolie bazi vaghti ke speedtoken nabashad tabeE bar asase masafate barry ast
			if (normalspeed)
				gamespeed = velocity_initial + scoremeter / 10;
			//be ezaye har meter_unit=40 bari ke dar while miad 1 meter be masafat ezafe mishe.
			metercounter++;
			if (metercounter >= meter_unit)
			{
				scoremeter += metercounter / meter_unit;
				metercounter = 0;
			}
			show_backgrounds();
			show_barry();
			initialize_coin_vector();
			initialize_obstract();
			show_gravity_key();
			show_speed_key();
			show_obstracts();
			show_coins();
			show_score();

			SBDL::showTexture(pausekey, pause_rect);
			if (SBDL::mouseInRect(pause_rect) && SBDL::Mouse.clicked())
				GameStatus = pause;
		}
		else if (GameStatus == pause)
		{
			show_pause();
		}

		else if (GameStatus == lose)
		{
			show_lose();
		}

		SBDL::updateRenderScreen();
		SBDL::delay(30);

	}
}


void loadassets()
{
	for (int i = 0; i < 6; i++)
	{
		coin[i] = SBDL::loadTexture("assets/pic/coin/" + to_string(i + 1) + ".png");
		missle[i] = SBDL::loadTexture("assets/pic/missle/missle (" + to_string(i + 1) + ").png");
		smoke[i] = SBDL::loadTexture("assets/pic/smoke/smoke " + to_string(i + 1) + ".png");
	}
	nonactivelaser = SBDL::loadTexture("assets/pic/lazer/laser_noneactive.png");
	activelaser1 = SBDL::loadTexture("assets/pic/lazer/laser_active1.png");
	warning = SBDL::loadTexture("assets/pic/missle/1m.png");
	warning2 = SBDL::loadTexture("assets/pic/missle/2m.png");
	speedtoken[0] = SBDL::loadTexture("assets/pic/speedToken/speed token2.png");
	speedtoken[1] = SBDL::loadTexture("assets/pic/speedToken/speed token.png");
	speedtoken[2] = SBDL::loadTexture("assets/pic/speedToken/speed token3.png");
	speedtoken[3] = SBDL::loadTexture("assets/pic/speedToken/speed token4.png");

	for (int i = 0; i < 4; i++)
	{
		diagonal[i] = SBDL::loadTexture("assets/pic/zappers/d" + to_string(i + 1) + ".png");
		vertical[i] = SBDL::loadTexture("assets/pic/zappers/v" + to_string(i + 1) + ".png");
		horizontal[i] = SBDL::loadTexture("assets/pic/zappers/h" + to_string(i + 1) + ".png");
	}
	//backgrounds:
	background[0] = SBDL::loadTexture("assets/pic/back/Lab Start.png");
	background[3] = SBDL::loadTexture("assets/pic/back/Sector Start.png");
	background[6] = SBDL::loadTexture("assets/pic/back/Volcano Start.png");
	background[9] = SBDL::loadTexture("assets/pic/back/warehouse Start.png");
	background[10] = SBDL::loadTexture("assets/pic/back/warehouse.png");
	background[1] = SBDL::loadTexture("assets/pic/back/Lab1.png");
	background[2] = SBDL::loadTexture("assets/pic/back/Lab2.png");
	background[4] = SBDL::loadTexture("assets/pic/back/Sector1.png");
	background[5] = SBDL::loadTexture("assets/pic/back/Sector2.png");
	background[7] = SBDL::loadTexture("assets/pic/back/Volcano1.png");
	background[8] = SBDL::loadTexture("assets/pic/back/Volcano2.png");
	background[11]= SBDL::loadTexture("assets/pic/parallex/Second P.png");
	background[13] = SBDL::loadTexture("assets/pic/parallex/First P.png");
	background[12] = SBDL::loadTexture("assets/pic/parallex/Second P2.png");
	background[14] = SBDL::loadTexture("assets/pic/parallex/First P2.png");

	barrywalk[0] = SBDL::loadTexture("assets/pic/barry/barry.png");
	barrywalk[1] = SBDL::loadTexture("assets/pic/barry/barry2.png");
	barryup = SBDL::loadTexture("assets/pic/barry/barry3.png");
	barrydown = SBDL::loadTexture("assets/pic/barry/barry4.png");
	barryspeed = SBDL::loadTexture("assets/pic/barry/barryst.png");
	gravity_token_key = SBDL::loadTexture("assets/pic/menu/gravity_token.png");
	gravitydownwalk[0] = SBDL::loadTexture("assets/pic/barry/gg1.png");
	gravitydownwalk[1] = SBDL::loadTexture("assets/pic/barry/gg2.png");
	gravityupwalk[0] = SBDL::loadTexture("assets/pic/barry/gg3.png");
	gravityupwalk[1] = SBDL::loadTexture("assets/pic/barry/gg4.png");
	gravityup = SBDL::loadTexture("assets/pic/barry/ggup.png");
	gravitydown = SBDL::loadTexture("assets/pic/barry/ggdown.png");
	alienwalk[0] = SBDL::loadTexture("assets/pic/barry/alien.png");
	alienwalk[1] = SBDL::loadTexture("assets/pic/barry/alien2.png");
	alienup = SBDL::loadTexture("assets/pic/barry/alienup.png");
	aliendown = SBDL::loadTexture("assets/pic/barry/aliendown.png");

	font = SBDL::loadFont("assets/Font/Jetpackia.ttf", 20);
	fontb = SBDL::loadFont("assets/Font/Jetpackia.ttf", 50);
	firstBackground = SBDL::loadTexture("assets/pic/menu/Background.png");
	pausekey = SBDL::loadTexture("assets/added/Pause.png");
	retrykey = SBDL::loadTexture("assets/pic/menu/retry.png");
	resumekey = SBDL::loadTexture("assets/pic/menu/resume.png");
	quitkey = SBDL::loadTexture("assets/pic/menu/quit.png");
	barrybackground = SBDL::loadTexture("assets/pic/menu/pauseb.png");
	settingkey = SBDL::loadTexture("assets/pic/menu/setting.png");
	highscoreskey = SBDL::loadTexture("assets/pic/menu/highscores.png");
	playkey = SBDL::loadTexture("assets/pic/menu/play.png");
	sound_on = SBDL::loadTexture("assets/pic/menu/sound on.png");
	sound_off = SBDL::loadTexture("assets/pic/menu/sound off.png");
	back = SBDL::loadTexture("assets/pic/menu/back.png");
	change_barry = SBDL::loadTexture("assets/pic/menu/change keys.png");
	revive = SBDL::loadTexture("assets/pic/menu/resume 500.png");
	//sounds:
	music = SBDL::loadMusic("assets/sound/music.mp3");
	coinsound = SBDL::loadSound("assets/sound/coin.wav");
	nonactivelasersound = SBDL::loadSound("assets/sound/laser_warning.wav");
	warningsound = SBDL::loadSound("assets/sound/missile_warning.wav");
	gravitydownsound = SBDL::loadSound("assets/sound/grav_change_2.wav");
	gravityupsound = SBDL::loadSound("assets/sound/grav_change_1.wav");
	zappersound = SBDL::loadSound("assets/sound/zapper.wav");
	keysound = SBDL::loadSound("assets/sound/changebarry.wav");
	
}


void initialize_cordinates()
{
	barry_y = barrywalk_y;
	barry_velocity = 0, barry_accleration = 0;
	score = 0;
	scorecoin = 0;
	xg = (rand() % 10) * 50 +2* window_width;
	gravity_rect = { xg,70,80,80 };
	xs = (rand() % 10) * 70 +3* window_width;
	ys = 100 + 100 * sin((double)xs / 50);
	speed_rect = { xs,ys,80,80 };
	lasertimer = 0;
	coinfile_num = 1;
	scoremeter = 0;
	metercounter = 0;
	appearance = normal;
	obstracts.clear();
	coins.clear();
}


void initialize_coin_vector()
{
	if (coins.size() == 0)
	{
		ifstream myfile("assets/coinPatterns/coinPattern" + to_string(coinfile_num) + ".txt");
		coinfile_num++;
		if (coinfile_num > 27)
			coinfile_num = 1;
		char ch;
		if (myfile.is_open()) {

			while (myfile >> ch)
			{
				if (ch == ' ')
					ch = '0';
				eachline.push_back(ch - '0');
				if ((myfile.peek() == '\n') || (myfile.peek() == EOF)) 
				{
					pattern.push_back(eachline);
					eachline.clear();
				}
			}
			myfile.close();
		}
		int start_x = window_width + 50;
		int start_y = (rand() % 3 + 1) * 50;
		int print_x = 0, print_y = 0;
		for (int i = 0; i < pattern.size(); i++)
		{
			for (int j = 0; j < pattern[i].size(); j++)
			{
				if (pattern[i][j] == 1)
				{
					print_x = start_x + j * 20;
					print_y = start_y + i * 20;
					Point p = { print_x,print_y };
					SDL_Rect rect = { p.x,p.y,20,20 };
					Coin newcoin = { p,false,rect };
					coins.push_back(newcoin);
				}
			}
			pattern[i].clear();
		}
		pattern.clear();
		coin_start = std::clock();
	}
	for (int k = 0; k<coins.size(); k++)
		if (coins.size()>0 && coins[k].p.x <0)
			coins.erase(coins.begin() + k);
}


void initialize_obstract()
{
	for (int k = 0; k < obstracts.size(); k++)
	{
		if (obstracts[k].p.x < 0)
			obstracts.erase(obstracts.begin() + k);
	}
	//obj_distance neshan midahad agar x mane e ghabli az meghdarash kamtar shod mane e jadid ijad shavad 
	//va in vabaste be sorate bazi ast ke be moroor maneha bishtar shavand.
	obj_distance = (velocity_initial + scoremeter / 10)*window_width / 30;
	if (obj_distance > (2 / 3)*window_width)
		obj_distance = (2 / 3)*window_width;
	if (obstracts.size() ==0 || (obstracts[obstracts.size() - 1].p.x < obj_distance))
	{	
			mane();
	}
	
}

void mane()
{
	int x, randomy;
	Type type;
	Point p;
	srand(time(NULL));
	int choice = rand() % 5;//noe mane ra random tolid mikonad
	//az beyne chand mokhtasat be soorate random yeki ra baray shorooe jaye mane entekhab mikonad ta maneha dar makanhaye monaseb bashand
	int zappers_y_position[] = { 180,260,340 };
	int zappers_h_y_position[] = { 180,300,410 };
	int laser_y_position[] = { 150,200,400 };
	havezapper = false;
	havemissile = false;

	for (int k = 0; k < obstracts.size(); k++)
	{
		if (obstracts[k].type == zappers_d || obstracts[k].type == zappers_v || obstracts[k].type == zappers_h)
			havezapper = true;

		if (obstracts[k].type == missile || obstracts[k].type == caution1 || obstracts[k].type == caution2)
			havemissile = true;
	}
	switch (choice)
	{
	case(0)://mane zappere ofoghi:
		randomy = zappers_h_y_position[rand() % 3];
		x = window_width + 50;
		p = { x,randomy };
		type = zappers_h;
		obsrect = { p.x,p.y,100,40 };
		obs = { p,false,type,obsrect };
		duration_laser = (std::clock() - laser_start) / (double)CLOCKS_PER_SEC;
		//in baraye in ast ke vaghti laser miayad zapper nayad.chon duratione laser 9 ast bayad bad az an biayad
		if (havelaser && duration_laser>9)
		{
			obstracts.push_back(obs);
			zapper_start = std::clock();
			havezapper = true;
		}
		//agar ham ke laser nadashtim emkane tolide zapper hast.
		else if (!havelaser)
		{
			obstracts.push_back(obs);
			zapper_start = std::clock();
			havezapper = true;
		}
		break;

	case(1)://mane zappere amoodi:
		randomy = zappers_y_position[rand() % 3];
		x = window_width + 50;
		p = { x,randomy };
		type = zappers_v;
		obsrect = { p.x,p.y,50,100 };
		obs = { p,false,type,obsrect };
		duration_laser = (std::clock() - laser_start) / (double)CLOCKS_PER_SEC;
		//in baraye in ast ke vaghti laser miayad zapper nayad.chon duratione laser 9 ast bayad bad az an biayad
		if (havelaser && duration_laser>9)
		{
			obstracts.push_back(obs);
			zapper_start = std::clock();
			havezapper = true;
		}
		//agar ham ke laser nadashtim emkane tolide zapper hast.
		else if (!havelaser)
		{
			obstracts.push_back(obs);
			zapper_start = std::clock();
			havezapper = true;
		}
		break;

	case(2)://mane zappere mayel:
		randomy = zappers_y_position[rand() % 3];
		x = window_width + 50;
		p = { x,randomy };
		type = zappers_d;
		obsrect = { p.x,p.y,100,100 };
		obs = { p,false,type,obsrect };
		duration_laser = (std::clock() - laser_start) / (double)CLOCKS_PER_SEC;
		//in baraye in ast ke vaghti laser miayad zapper nayad.chon duratione laser 9 ast bayad bad az an biayad
		if (havelaser && duration_laser > 9)
		{
			obstracts.push_back(obs);
			zapper_start = std::clock();
			havezapper = true;
		}
		//agar ham ke laser nadashtim emkane tolide zapper hast.
		else if(!havelaser)
		{
			obstracts.push_back(obs);
			zapper_start = std::clock();
			havezapper = true;
		}
		break;

	case(3)://laser

		randomy = laser_y_position[rand() % 3];
		x = 10;
		p = { x,randomy };
		type = laser_nonactive;
		obsrect = { p.x,p.y,window_width,50 };
		obs = { p,false,type,obsrect };
		duration_zapper = (std::clock() - zapper_start) / (double)CLOCKS_PER_SEC;
		duration_missile = (std::clock() - missile_start) / (double)CLOCKS_PER_SEC;
		//in khat baray adame tadakhol beyne laser ba zapper va missile ast.
		// window_width/gamespeed zamane rad shodane mane ast.
		if (((havezapper && duration_zapper > window_width  / gamespeed)||!havezapper) &&(( havemissile && duration_missile > window_width / gamespeed)|| !havemissile))
		{
			obstracts.push_back(obs);
			laser_start = std::clock();
			havelaser = true;
		}
		break;

	case(4)://missile
		randomy = barry_y;
		x = 1400;
		p = { x,randomy };
		type = caution1;
		obsrect = { p.x,p.y,60,50 };
		obs = { p,false,type,obsrect };
		duration_laser = (std::clock() - laser_start) / (double)CLOCKS_PER_SEC;
		//baraye inke missile ba laser nayayad;
		if (havelaser && duration_laser > 9)
		{
			obstracts.push_back(obs);
			missile_start = std::clock();
			havemissile = true;
		}
		//agar ham ke laser nabood emkane tolide missile hast.
		else if (!havelaser)
		{
			obstracts.push_back(obs);
			missile_start = std::clock();
			havemissile = true;
		}
		break;

	default:
		break;
	}
}

void show_firstpage() {
	SBDL::showTexture(background[0], backgroundrect);
	SBDL::showTexture(barrybackground, barrybackground_rect);
	SBDL::showTexture(quitkey, exit_rect);
	SBDL::showTexture(playkey, play_rect);
	SBDL::showTexture(highscoreskey, highscores_rect);
	SBDL::showTexture(settingkey, setting_rect);

	if (SBDL::mouseInRect(play_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		GameStatus = play;
	}
	else if (SBDL::mouseInRect(exit_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		exit(0);
	}
	else if (SBDL::mouseInRect(setting_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		GameStatus = setting_from_first;
	}
}

void show_setting() {
	SBDL::showTexture(background[9], backgroundrect);
	SBDL::showTexture(barrybackground, barrybackground_rect);
	SBDL::showTexture(back, back_rect);
	SBDL::showTexture(change_barry, change_barry_rect);
	if (SBDL::mouseInRect(change_barry_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		if (appearance == normal)
		{
			appearance = alienapp;
			prev_appearance = alienapp;
		}
		else
		{
			appearance = normal;
			prev_appearance = normal;
		}
	}
	if (GameStatus == setting_from_pause &&SBDL::mouseInRect(back_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		GameStatus = pause;
		
	}

	else if (GameStatus == setting_from_first && SBDL::mouseInRect(back_rect) && SBDL::Mouse.clicked())
	{
		GameStatus = firstpage;
		SBDL::playSound(keysound, 1);
	}


	if (mutebkgmusic == true)
	{
		SBDL::showTexture(sound_on, sound_rect);
		if (SBDL::mouseInRect(sound_rect) && SBDL::Mouse.clicked())
		{
			SBDL::playSound(keysound, 1);
			mutebkgmusic = false;

			SBDL::playMusic(music, -1);
			//how to play music again?
		}
	}
	else if (mutebkgmusic == false)
	{
		SBDL::showTexture(sound_off, sound_rect);
		if (SBDL::mouseInRect(sound_rect) && SBDL::Mouse.clicked())
		{
			SBDL::playSound(keysound, 1);
			SBDL::stopMusic();
			mutebkgmusic = true;
		}

	}

}
void show_backgrounds() {
	currentrect = { xcurrentback, 0 ,window_width,window_height };
	commingrect = { xcommingback, 0 ,window_width,window_height };
	if (SBDL::hasIntersectionRect(currentrect, backgroundrect))
	{
		
		bkgcounter = bkgcounter % 11;
		
			SBDL::showTexture(background[bkgcounter], currentrect);
			SBDL::showTexture(background[(bkgcounter + 1) % 11], commingrect);
			if (bkgcounter == 8)
			{
				SBDL::showTexture(background[11], commingrect);
				SBDL::showTexture(background[13], commingrect);
				
			}
			if (bkgcounter == 9)
			{
				SBDL::showTexture(background[11], currentrect);
				SBDL::showTexture(background[13], currentrect);
				SBDL::showTexture(background[12], commingrect);
				SBDL::showTexture(background[14], commingrect);
			}

			else if (bkgcounter == 10)
			{
				SBDL::showTexture(background[12], currentrect);
				SBDL::showTexture(background[14], currentrect);
			}

			xcurrentback -= gamespeed;
			xcommingback -= gamespeed;
		
	}
	else
	{
		if (bkgcounter == 10)
			bkgcounter = 0;
		else
			bkgcounter++;
		xcurrentback = 0;
		xcommingback = window_width;
	}
}

void show_barry() {

	barry_rect = { 100,barry_y,50,70 };
	if (appearance == speedtokenapp_normal || appearance == speedtokenapp_alien)
	{
		gamespeed = gamespeedfast;
		normalspeed = false;
		metercounter += meterunitfast;
		barry_y = window_height / 2;
		if (appearance == speedtokenapp_alien)
			SBDL::showTexture(alienwalk[1], 45, barry_rect);
		else
			SBDL::showTexture(barryspeed, barry_rect);

		duration_speed = (std::clock() - speed_start) / (double)CLOCKS_PER_SEC;

		if (duration_speed > 3)
		{
			if (appearance == speedtokenapp_alien)
				appearance = alienapp;
			else
				appearance = normal;

			normalspeed = true;
			barry_y = barrywalk_y;
		}

	}
	if (appearance == normal || appearance == alienapp)
	{
		if ( SBDL::keyHeld(SDL_SCANCODE_SPACE))
		{	
			go_up_acceleration();
		
			if (appearance == alienapp)
				SBDL::showTexture(alienup, barry_rect);
			else
				SBDL::showTexture(barryup, barry_rect);

			if (barry_y > barrywalkup_y)
				barry_y -= delta_y;
		}
		else if (barry_y == barrywalk_y)
		{
			if (walkcounter < 4)
			{
				if (appearance == normal)
					SBDL::showTexture(barrywalk[0], barry_rect);
				else
					SBDL::showTexture(alienwalk[0], barry_rect);
			}
			else
			{
				if (appearance == normal)
					SBDL::showTexture(barrywalk[1], barry_rect);
				else
					SBDL::showTexture(alienwalk[1], barry_rect);
			}
			walkcounter++;
			if (walkcounter > 7)
				walkcounter = 0;
		}
		else //biad paeen
		{
			go_down_acceleration();
			
			if (appearance == normal)
				SBDL::showTexture(barrydown, barry_rect);
			else
				SBDL::showTexture(aliendown, barry_rect);
			if (barry_y >= barrywalk_y)
				barry_y = barrywalk_y;
			else
				barry_y += delta_y;

		}
	}
	if (appearance == gravity)
	{
		if (SBDL::Mouse.clicked())
		{
			grav_up_sound = false;
			grav_down_sound = false;
			if ((goup == false || godown == true) && barry_y > barrywalkup_y)
			{
				goup = true;
				godown = false;
			}
			else if ((godown == false || goup == true) && barry_y < barrywalk_y)
			{
				godown = true;
				goup = false;
			}
		}

		if (goup)
		{
			if (barry_y >= barrywalkup_y && barry_y <= barrywalk_y)
			{
				if (!grav_up_sound)
				{
					SBDL::playSound(gravityupsound, 1);
					grav_up_sound = true;
				}
				SBDL::showTexture(gravityup, barry_rect);
				go_up_acceleration();
				barry_y -= delta_y;
			}
			if (barry_y <= barrywalkup_y)
			{
				barry_y = barrywalkup_y;
				goup = false;
			}

		}
		if (godown)
		{
			if (barry_y >= barrywalkup_y && barry_y <= barrywalk_y)
			{
				if (!grav_down_sound)
				{
					SBDL::playSound(gravitydownsound, 1);
					grav_down_sound = true;
				}
				SBDL::showTexture(gravitydown, barry_rect);
				go_down_acceleration();
				barry_y += delta_y;
			}

			if (barry_y >= barrywalk_y)
			{
				godown = false;
				barry_y = barrywalk_y;
			}
		}
		if (barry_y == barrywalkup_y)
		{

			if (walkcounter < 4)
				SBDL::showTexture(gravityupwalk[0], barry_rect);
			else
				SBDL::showTexture(gravityupwalk[1], barry_rect);
			walkcounter++;
			if (walkcounter > 7)
				walkcounter = 0;
		}
		if (barry_y == barrywalk_y)
		{
			barry_y = barrywalk_y;
			if (walkcounter < 4)
				SBDL::showTexture(gravitydownwalk[0], barry_rect);
			else
				SBDL::showTexture(gravitydownwalk[1], barry_rect);
			walkcounter++;
			if (walkcounter > 7)
				walkcounter = 0;
		}

	}
}
void show_gravity_key() {
	if (SBDL::hasIntersectionRect(barry_rect, gravity_rect))
	{
		prev_appearance = appearance;
		appearance = gravity;
		xg = -window_width;
		gravity_rect.x = -window_width;
		barry_y = barrywalk_y;
	}
	if ( !SBDL::hasIntersectionRect(barry_rect, gravity_rect))//baraye inke gravity_key ba samte chap harekat kone
	{
		xg -= gamespeed;
		gravity_rect = { xg,70,80,80 };
		SBDL::showTexture(gravity_token_key, gravity_rect);
	}
	if (xg <= -10 && (appearance == normal || appearance == alienapp))//baraye inke az safhe ke biroon raft meghdare jadid begirad.
	{
		srand(time(NULL));
		xg = (rand() % 10) * 80 + 2 * window_width;
		gravity_rect = { xg,70,80,80 };
	}
}
void show_speed_key()
{
	if (SBDL::hasIntersectionRect(barry_rect, speed_rect))
	{
		xs =- window_width;
		speed_rect.x = -window_width;
		SBDL::playSound(keysound, 1);
		if (appearance == alienapp || prev_appearance == alienapp)//ghabli ra ham check mikonim momkene gravity shode bashad.
			appearance = speedtokenapp_alien;
		else
			appearance = speedtokenapp_normal;
		speed_start = std::clock();//baraye inke ta zamane moshakhasi ba sorate bala bashad
	}

	 if(xs>-10 && !SBDL::hasIntersectionRect(barry_rect, speed_rect))
	{
		
		xs -= gamespeed / 2;
		ys = 100 + 100 * sin((double)xs / 50);
		speed_rect = { xs,ys,80,80 };
		SBDL::showTexture(speedtoken[token_counter/5], speed_rect);
		token_counter++;
		if (token_counter > 20)
			token_counter = 0;
	}

	if (xs <= -10 && (appearance == normal || appearance == alienapp))
	{
		srand(time(NULL));
		xs = (rand() % 10) * 70 + 3 * window_width;
		ys = 100 + 100 * sin((double)xs / 50);
		speed_rect = { xs,ys,80,80 };
	}
}


void show_obstracts()
{
	for (int k = 0; k < obstracts.size(); k++)
	{
		if (obstracts[k].eaten == false)
		{
			if (obstracts[k].type == zappers_h)
			{
				SBDL::showTexture(horizontal[zappercounter], obstracts[k].obs_rect);
				zappercounter++;
				if (zappercounter > 3)
					zappercounter = 0;
				obstracts[k].p.x -= gamespeed;
				obstracts[k].obs_rect.x -= gamespeed;

			}
			else if (obstracts[k].type == zappers_v)
			{

				SBDL::showTexture(vertical[zappercounter], obstracts[k].obs_rect);
				zappercounter++;
				if (zappercounter > 3)
					zappercounter = 0;
				obstracts[k].p.x -= gamespeed;
				obstracts[k].obs_rect.x -= gamespeed;

			}
			else if (obstracts[k].type == zappers_d)
			{

				SBDL::showTexture(diagonal[zappercounter], obstracts[k].obs_rect);
				zappercounter++;
				if (zappercounter > 3)
					zappercounter = 0;
				obstracts[k].p.x -= gamespeed;
				obstracts[k].obs_rect.x -= gamespeed;
				

			}
			else if (obstracts[k].type == laser_nonactive)
			{
				if (!play_laser_sound)
				{
					SBDL::playSound(nonactivelasersound, 1);
					play_laser_sound = true;
				}
				duration_laser = (std::clock() - laser_start) / (double)CLOCKS_PER_SEC;
				if (duration_laser < 4.5)
					SBDL::showTexture(nonactivelaser, obstracts[k].obs_rect);
				else
				{
					obstracts[k].type = laser_active;
					play_laser_sound = false;
				}

			}
			else if (obstracts[k].type == laser_active)
			{
				duration_laser = (std::clock() - laser_start) / (double)CLOCKS_PER_SEC;
				if (duration_laser <9)
					SBDL::showTexture(activelaser1, obstracts[k].obs_rect);
				else
				{
					obstracts[k].p.x = -window_width;
					obstracts[k].obs_rect.x = -window_width;
					havelaser = false;
				}
			}
			else if (obstracts[k].type == caution1)
			{
				if (!play_warning_sound)
				{
					SBDL::playSound(warningsound, 1);
					play_warning_sound = true;
				}

				obstracts[k].obs_rect.y = barry_y;
				obstracts[k].p.y = barry_y;
				duration_missile = (std::clock() - missile_start) / (double)CLOCKS_PER_SEC;
				if (duration_missile < 0.5)
					SBDL::showTexture(warning, obstracts[k].obs_rect);
				else
					obstracts[k].type = caution2;
			}
			else if (obstracts[k].type == caution2)
			{
				play_warning_sound = false;
				obstracts[k].obs_rect.y = barry_y;
				obstracts[k].p.y = barry_y;
				duration_missile = (std::clock() - missile_start) / (double)CLOCKS_PER_SEC;
				if (duration_missile < 1)
					SBDL::showTexture(warning2, obstracts[k].obs_rect);
				else
				{
					obstracts[k].type = missile;
					
				}

			}
			else if (obstracts[k].type == missile)
			{
				SBDL::showTexture(smoke[smokecounter], obstracts[k].obs_rect.x + obstracts[k].obs_rect.w, obstracts[k].obs_rect.y);
				SBDL::showTexture(missle[smokecounter], obstracts[k].obs_rect);
				smokecounter++;
				if (smokecounter > 5)
					smokecounter = 0;
				obstracts[k].p.x -= 4 * gamespeed;
				obstracts[k].obs_rect.x -= 4 * gamespeed;
				
			}

		}
		if (SBDL::hasIntersectionRect(barry_rect, obstracts[k].obs_rect) && (appearance == normal || appearance == alienapp) && obstracts[k].type != laser_nonactive&& obstracts[k].type != zappers_d)
		{
			if ((obstracts[k].type == zappers_h || obstracts[k].type == zappers_v) && obstracts[k].eaten == false)
			{

				SBDL::playSound(zappersound, 1);

			}
			if (!obstracts[k].eaten)
				GameStatus = lose;
			obstracts[k].eaten = true;

		}
		else if (SBDL::hasIntersectionRect(barry_rect, obstracts[k].obs_rect) && obstracts[k].type == zappers_d && (appearance == normal || appearance == alienapp))
		{
			if (obstracts[k].obs_rect.x + 100 - barry_rect.x < 50)
			{
				if (obstracts[k].eaten == false)
				{

					SBDL::playSound(zappersound, 1);

				}
				if (!obstracts[k].eaten)
					GameStatus = lose;
				obstracts[k].eaten = true;
			}
		}
		else if (SBDL::hasIntersectionRect(barry_rect, obstracts[k].obs_rect) && appearance == gravity  && obstracts[k].type != laser_nonactive && obstracts[k].type != zappers_d)
		{
			
			if ((obstracts[k].type == zappers_h || obstracts[k].type == zappers_v) && obstracts[k].eaten == false)
			{

				SBDL::playSound(zappersound, 1);

			}
			obstracts[k].eaten = true;
			appearance = prev_appearance;
			obstracts[k].obs_rect.x = -window_width;
			obstracts[k].p.x = -window_width;
			barry_y = barrywalk_y;
		}

		else if (SBDL::hasIntersectionRect(barry_rect, obstracts[k].obs_rect) && obstracts[k].type == zappers_d && appearance == gravity)
		{
			if (obstracts[k].obs_rect.x + 100 - barry_rect.x < 50)
			{
				if (obstracts[k].eaten == false)
				{

					SBDL::playSound(zappersound, 1);

				}
				obstracts[k].eaten = true;
				obstracts[k].obs_rect.x = -window_width;
				obstracts[k].p.x = -window_width;
				appearance = prev_appearance;
				barry_y = barrywalk_y;
				obstracts.erase(obstracts.begin() + k);
			}

		}
		duration_speed = (std::clock() - speed_start) / (double)CLOCKS_PER_SEC;

		if ((duration_speed < 7) && obstracts[k].eaten == true) //ta chand sanye bad az taghire halat be halate addi az gravity nabaze
		{
			obstracts[k].eaten = false;
			obstracts[k].obs_rect.x = -window_width;
			obstracts[k].p.x = -window_width;
			GameStatus = play;
		}

	}
}


void show_coins() {
	for (int k = 0; k < coins.size(); k++)
	{
		if (coins[k].eaten == false)
			SBDL::showTexture(coin[coincounter / 7], coins[k].coinrect);
		coincounter++;
		if (coincounter > 42)
			coincounter = 0;
		coins[k].p.x -= gamespeed;
		coins[k].coinrect.x -= gamespeed;

		if (SBDL::hasIntersectionRect(barry_rect, coins[k].coinrect))
		{
			if (coins[k].eaten == false)
				scorecoin++;
			SBDL::playSound(coinsound, 1);
			coins[k].eaten = true;

		}

	}
}

void show_score() 
{
	Texture textcoin = SBDL::createFontTexture(font, "coins : " + to_string(scorecoin), 255, 255, 0);
	SBDL::showTexture(textcoin, 10, 30);
	SBDL::freeTexture(textcoin);
	Texture textmeter = SBDL::createFontTexture(font, "" + to_string(scoremeter) + " M", 0, 0, 0);
	SBDL::showTexture(textmeter, 10, 10);
	SBDL::freeTexture(textmeter);
}
void show_pause() 
{
	SBDL::stopAllSounds();
	SBDL::showTexture(background[0], backgroundrect);
	SBDL::showTexture(barrybackground, barrybackground_rect);
	SBDL::showTexture(settingkey, setting_rect2);
	SBDL::showTexture(retrykey, retry_rect);
	SBDL::showTexture(resumekey, resume_rect);
	SBDL::showTexture(quitkey, quit_rect);

	if (SBDL::mouseInRect(setting_rect2) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		GameStatus = setting_from_pause;
	}
	if (SBDL::mouseInRect(resume_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		GameStatus = play;
	}
	if (SBDL::mouseInRect(quit_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		xcurrentback = 0; xcommingback = window_width;
		bkgcounter = 0; walkcounter = 0; coincounter = 0; zappercounter = 0;
		initialize_cordinates();
		GameStatus = firstpage;
	}
	if (SBDL::mouseInRect(retry_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		xcurrentback = 0; xcommingback = window_width;
		bkgcounter = 0; walkcounter = 0; coincounter = 0; zappercounter = 0;
		initialize_cordinates();
		GameStatus = play;
	}
}
void show_lose() 
{
	SBDL::showTexture(background[10], backgroundrect);
	Texture textcoin = SBDL::createFontTexture(fontb, "coins : " + to_string(scorecoin), 255, 255, 0);
	SBDL::showTexture(textcoin, 650, 200);
	SBDL::freeTexture(textcoin);

	Texture	collect = SBDL::createFontTexture(font, "  AND COLLECTED  ", 255, 204, 255);
	SBDL::showTexture(collect, 650, 170);
	SBDL::freeTexture(collect);

	Texture flew = SBDL::createFontTexture(font, "  YOU FLEW  ", 255, 204, 255);
	SBDL::showTexture(flew, 650, 70);
	SBDL::freeTexture(flew);

	Texture textmeter = SBDL::createFontTexture(fontb, "" + to_string(scoremeter) + " M", 255, 255, 0);
	SBDL::showTexture(textmeter, 650, 100);
	SBDL::freeTexture(textmeter);
	if (scorecoin < 500)
	{
		Texture cantrevive = SBDL::createFontTexture(font, "NOT ENOUGH COINS TO REVIVE !   : ( ", 0, 204, 0);
		SBDL::showTexture(cantrevive, 950, 200);
		SBDL::freeTexture(cantrevive);
	}
	SBDL::showTexture(revive, revive_rect);
	SBDL::showTexture(retrykey, retry_rect);
	SBDL::showTexture(playkey, play_rect);
	if (SBDL::mouseInRect(retry_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		xcurrentback = 0; xcommingback = window_width;
		bkgcounter = 0; walkcounter = 0; coincounter = 0; zappercounter = 0;
		initialize_cordinates();
		GameStatus = play;
	}
	if (SBDL::mouseInRect(revive_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		if (scorecoin >= 500)
		{
			scorecoin -= 500;
			GameStatus = play;
			barry_y = barrywalk_y;
			xg = (rand() % 10) * 50 + window_width;
			gravity_rect = { xg,70,80,80 };
			lasertimer = 0;
			obstracts.clear();
			coins.clear();
		}
	}
	if (SBDL::mouseInRect(play_rect) && SBDL::Mouse.clicked())
	{
		SBDL::playSound(keysound, 1);
		xcurrentback = 0; xcommingback = window_width;
		bkgcounter = 0; walkcounter = 0; coincounter = 0; zappercounter = 0;
		initialize_cordinates();
		GameStatus = firstpage;
	}
}

void go_up_acceleration() 
{
	barry_accleration = -const_accleration;
	barry_velocity = sqrt(2 * const_accleration*barry_y);
	delta_y = fabs(0.5*barry_accleration*delta_time*delta_time + barry_velocity*delta_time);
}

void go_down_acceleration() {
	barry_accleration = -const_accleration;
	barry_velocity = -sqrt(2 * const_accleration*(barry_y));
	delta_y = fabs(0.5*barry_accleration*delta_time*delta_time + barry_velocity*delta_time);
}