#pragma once

#include "Skyrmion/input/MovementEnums.h"

enum CollisionLayer {
	BACKGROUND,
	TREEMAP,
	MAP,
	TEMPMAP,
	ANIMATED,
	SIGN,
	BUTTON,
	BOX,
	PLAYER,
	SECTION,
	INPUT,
	TEXT,
	FOREGROUND,
	MENU,
	MENUBUTTON
};
static std::vector<std::string> LAYER_NAMES = {
	"BACKGROUND", "TREEMAP", "MAP", "TEMPMAP", "ANIMATED", "SIGN", "BUTTON",
	"BOX", "PLAYER", "SECTION", "INPUT", "TEXT", "FOREGROUND", "MENU", "MENUBUTTON"
};

enum Textures {
	invalidTexture,
	solidTexture,
	worldTexture,
	decorTexture,
	treeTexture,
	playerTexture,
	blocksTexture,
	flagTexture,
	yetiTexture,
	tentTexture,
	menuButtonsTexture,
	backgroundTexture1,
	backgroundTexture2,
	backgroundTexture3,
	backgroundTexture4,
};
static std::vector<std::string> TEXTURE_FILES = {
	"#INVALID", "#SOLIDCOLOR",
	"res/textures/world_tiles.png", "res/textures/decor_tiles.png", "res/textures/trees.png",
	"res/textures/character.png", "res/textures/blocks.png","res/textures/flag.png",
	"res/textures/sleeping_yeti.png","res/textures/tent.png", "res/textures/menu_buttons.png",
	"res/textures/background1.png", "res/textures/background2.png", "res/textures/background3.png", "res/textures/background4.png",
};


enum Signals {
	RESET_SECTION,
	RESET_GAME,
	TOGGLE_MENU,
	SAVE_GAME
};

enum GrowthType {
	TREEEMPTY,
	NONE,
	TREE,
	SNOWTREE,
	ROCK,
	SNOWROCK
};

#define SNOW_OFFSET 100

static const std::map<int, int> displayIndex = {
	{' ', -1},
	{'/', 8},
	{'-', 0},
	{'\\', 8+144},
	{'#', 10},
	{'\'', 6},
	{'y', 10},
	{'Y', 10},
	{'[', 4},
	{']', 4+144},
	{'{', 6+12},
	{'=', 7+12},
	{'}', 6+12+144},
	{'(', 8+12},
	{'u', 9+12},
	{')', 8+12+144},
	{'"', 0+24},
	{'T', 1+24},
	{'N', 7+24},
	{'H', 6+24},
	{'_', 2+24},
	{',', 8+24},
	{'b', 11+24},
	{'>', 4+24},
	{'<', 10+24},
	{'^', 5+24},
	{' '+SNOW_OFFSET, -1},
	{'/'+SNOW_OFFSET, 10+12},
	{'-'+SNOW_OFFSET, 0+12},
	{'\\'+SNOW_OFFSET,10+12+144},
	{'#'+SNOW_OFFSET, 5+12},
	{'\''+SNOW_OFFSET,7+12},
	{'y'+SNOW_OFFSET, 11+12},
	{'Y'+SNOW_OFFSET, 11+12+144},
	{'['+SNOW_OFFSET, 3+12},
	{']'+SNOW_OFFSET, 3+12+144},
	{'{'+SNOW_OFFSET, 6+12},
	{'='+SNOW_OFFSET, 7+12},
	{'}'+SNOW_OFFSET, 6+12+144},
	{'('+SNOW_OFFSET, 8+12},
	{'u'+SNOW_OFFSET, 9+12},
	{')'+SNOW_OFFSET, 8+12+144},
	{'"'+SNOW_OFFSET, 0+24},
	{'T'+SNOW_OFFSET, 1+24},
	{'N'+SNOW_OFFSET, 7+24},
	{'H'+SNOW_OFFSET, 6+24},
	{'_'+SNOW_OFFSET, 2+24},
	{','+SNOW_OFFSET, 8+24},
	{'b'+SNOW_OFFSET, 11+24},
	{'>'+SNOW_OFFSET, 4+24},
	{'<'+SNOW_OFFSET, 10+24},
	{'^'+SNOW_OFFSET, 5+24},
};

static const std::map<int, int> randomizerIndex = {
	{'/', 2},
	{'-', 4},
	{'\\', 2},
	{'#', 2},
	{'\'', 2},
	{'[', 2},
	{']', 2},
	{'/'+SNOW_OFFSET, 1},
	{'-'+SNOW_OFFSET, 3},
	{'\\'+SNOW_OFFSET, 1},
	{'#'+SNOW_OFFSET, 1},
	{'\''+SNOW_OFFSET, 1},
	{'['+SNOW_OFFSET, 2},
	{']'+SNOW_OFFSET, 2}
};

static const std::map<int, int> collisionIndexOff = {
	{' ', EMPTY},
	{'/', SLOPE_UPRIGHT},
	{'-', FULL},
	{'\\', SLOPE_UPLEFT},
	{'#',  FULL},
	{'\'', ONEWAY_UP},
	{'y',  FULL},
	{'Y',  FULL},
	{'[', FULL},
	{']', FULL},
	{'[', FULL},
	{']', FULL},
	{'{', FULL},
	{'=', FULL},
	{'}', FULL},
	{'(', ONEWAY_UP},
	{'u', ONEWAY_UP},
	{')', ONEWAY_UP},
	{'"', ONEWAY_UP},
	{'t', FULL},
	{'?', FULL},
	{'T', ONEWAY_UP},
	{'_', EMPTY},
	{',', EMPTY},
	{'H', ONEWAY_UP},
	{' '+SNOW_OFFSET, EMPTY},
	{'/'+SNOW_OFFSET, SLOPE_UPRIGHT},
	{'-'+SNOW_OFFSET, FULL},
	{'\\'+SNOW_OFFSET, SLOPE_UPLEFT},
	{'#'+SNOW_OFFSET, FULL},
	{'\''+SNOW_OFFSET, ONEWAY_UP},
	{'y'+SNOW_OFFSET, FULL},
	{'Y'+SNOW_OFFSET, FULL},
	{'['+SNOW_OFFSET, FULL},
	{']'+SNOW_OFFSET, FULL},
	{'{'+SNOW_OFFSET, FULL},
	{'='+SNOW_OFFSET, FULL},
	{'}'+SNOW_OFFSET, FULL},
	{'('+SNOW_OFFSET, ONEWAY_UP},
	{'u'+SNOW_OFFSET, ONEWAY_UP},
	{')'+SNOW_OFFSET, ONEWAY_UP},
	{'"'+SNOW_OFFSET, ONEWAY_UP},
	{'t'+SNOW_OFFSET, FULL},
	{'?'+SNOW_OFFSET, FULL},
	{'T'+SNOW_OFFSET, ONEWAY_UP},
	{'_'+SNOW_OFFSET, EMPTY},
	{','+SNOW_OFFSET, EMPTY},
	{'H'+SNOW_OFFSET, ONEWAY_UP}
};

static const std::map<int, int> collisionIndexOnAdd = {
	{'~', ONEWAY_UP},
	{'%', FULL},
	{'~'+SNOW_OFFSET, ONEWAY_UP},
	{'%'+SNOW_OFFSET, FULL}
};
static std::map<int, int> collisionIndexOn = collisionIndexOff + collisionIndexOnAdd;

static const std::map<int, int> frictionIndex = {
	{' ', 100},
	{'{', 15},
	{'=', 15},
	{'{', 15},
	{'(', 15},
	{'u', 15},
	{')', 15},
	{' '+SNOW_OFFSET, 20},
	{'/'+SNOW_OFFSET, 20},
	{'-'+SNOW_OFFSET, 20},
	{'\\'+SNOW_OFFSET, 20},
	{'#'+SNOW_OFFSET, 20},
	{'\''+SNOW_OFFSET, 20},
	{'['+SNOW_OFFSET, 20},
	{']'+SNOW_OFFSET, 20},
	{'{'+SNOW_OFFSET, 15},
	{'='+SNOW_OFFSET, 15},
	{'}'+SNOW_OFFSET, 15},
	{'('+SNOW_OFFSET, 15},
	{'u'+SNOW_OFFSET, 15},
	{')'+SNOW_OFFSET, 15}
};

static const std::map<int, int> tempDisplayIndex = {
	{'~', 0+24},
	{'%', 9+24},
	{'~'+SNOW_OFFSET, 0+24},
	{'%'+SNOW_OFFSET, 9+24}
};

static const std::map<int, int> treeGrowIndex = {
	{' ', TREEEMPTY},
	{'-', TREE},
	{'[', TREE},
	{']', TREE},
	{'#', ROCK},
	{'-'+SNOW_OFFSET, SNOWTREE},
	{'['+SNOW_OFFSET, SNOWTREE},
	{']'+SNOW_OFFSET, SNOWTREE},
	{'#'+SNOW_OFFSET, SNOWROCK}
};

static const std::map<int, int> treeDisplayIndex = {
	{' ', -1},
	{10, 14},
	{11, 15},
	{12, 10},
	{13, 11},
	{14, 6},
	{15, 7},
	{16, 2},
	{17, 3},
	{20, 12},
	{21, 13},
	{22, 8},
	{23, 9},
	{24, 4},
	{25, 5},
	{26, 0},
	{27, 1},
	{30, 20},
	{31, 21},
	{32, 16},
	{33, 17},
	{40, 20},
	{41, 21},
	{42, 16},
	{43, 17}
};

static const std::map<int, int> decorDisplayIndex = {
	{' ', -1},
	{'f', 0},
	{'r', 4},
	{'p', -1},
	{'b', 12},
	{'f'+SNOW_OFFSET, 2},
	{'r'+SNOW_OFFSET, 6},
	{'p'+SNOW_OFFSET, 8},
	{'b'+SNOW_OFFSET, 14}
};

static const std::map<int, int> decorRandomIndex = {
	{'f', 2},
	{'r', 2},
	{'p', 1},
	{'b', 1},
	{'f'+SNOW_OFFSET, 2},
	{'r'+SNOW_OFFSET, 2},
	{'p'+SNOW_OFFSET, 3},
	{'b'+SNOW_OFFSET, 1}
};

static const std::map<int, int> foregroundDisplayIndex = {
	{' ', -1},
	{1, 0},
	{2, 7},
	{31, 4},
	{32, 5},
	{41, 13},
	{42, 6}
};

static const std::map<int, int> foregroundRandomIndex = {
	{1, 4},
	{2, 5}
};

