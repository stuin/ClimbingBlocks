#include <cstdlib>

#include "Skyrmion/tiling/SquareTiles.h"
#include "Skyrmion/tiling/RandomNoise.hpp"
#include "Skyrmion/util/AnimatedNode.hpp"

#include "GridManager.h"
#include "indexes.h"
#include "Player.hpp"
#include "Button.hpp"
#include "Menu.hpp"

void initialize() {
	std::srand(7802);

	//Load settings file
	Settings::loadSettings("res/settings.json");

	//Load backgrounds
	Node background1(BACKGROUND, Vector2i(4096, 4096));
	Node background2(BACKGROUND, Vector2i(4096, 4096));
	Node background3(BACKGROUND, Vector2i(4096, 4096));
	Node background4(BACKGROUND, Vector2i(4096, 4096));
	background1.setTexture(backgroundTexture1);
	background2.setTexture(backgroundTexture2);
	background3.setTexture(backgroundTexture3);
	background4.setTexture(backgroundTexture4);
	UpdateList::addNode(&background4);
	UpdateList::addNode(&background3);
	UpdateList::addNode(&background2);
	UpdateList::addNode(&background1);

	//Load base tile maps
	GridManager worldGrid("res/simple_world.json", SECTION, Vector2i(64, 64));
	MapIndexer display(worldGrid.grid, displayIndex, -1);
	//RandomIndexer display(new MapIndexer(worldGrid.grid, displayIndex, -1), randomizerIndex, -1);
	LargeTileMap world(worldTexture, 32, 32, &display, MAP);
	world.setScale(Vector2f(2, 2));
	UpdateList::addNodes(worldGrid.getNodes());
	UpdateList::addNodes(world.getNodes());

	//Secondary maps
	MapIndexer *collisionMapOff = new MapIndexer(worldGrid.grid, collisionIndexOff, EMPTY, 64, 64);
	MapIndexer *collisionMapOn = new MapIndexer(worldGrid.grid, collisionIndexOn, EMPTY, 64, 64);
	MapIndexer *frictionMap = new MapIndexer(worldGrid.grid, frictionIndex, 100, 64, 64);
	LargeTileMap tempMap(worldTexture, 32, 32, new MapIndexer(worldGrid.grid, tempDisplayIndex, -1), TEMPMAP);
	tempMap.setScale(Vector2f(2, 2));
	UpdateList::addNodes(tempMap.getNodes());

	//Generate tree+decor maps
	GridMaker treeGrid(worldGrid.width, worldGrid.height);
	GridMaker decorGrid(worldGrid.width, worldGrid.height);
	MapIndexer growthMap(worldGrid.grid, treeGrowIndex, NONE);

	//std::cout << EMPTY << FULL << SLOPE_UPLEFT << SLOPE_UPRIGHT << ONEWAY_UP << "\n";

	int x = std::rand() / ((RAND_MAX + 1u) / 3);
	while(x < worldGrid.width) {
		int y = 0;
		bool wide = true;
		while(y < worldGrid.height && growthMap.getTile(Vector2f(x,y)) == TREEEMPTY) {
			if(growthMap.getTile(Vector2f(x+1,y)) != TREEEMPTY)
				wide = false;
			y += 1;
		}

		int r = std::rand() / ((RAND_MAX + 1u) / 8);
		int t = growthMap.getTile(Vector2f(x,y));
		int o = (t == SNOWTREE) ? 1 : 0;
		if(t == ROCK || t == SNOWROCK)
			decorGrid.setTile(Vector2f(x, y-1), 'r'+(t == SNOWROCK) ? SNOW_OFFSET : 0);
		else if((t == TREE || t == SNOWTREE) && r < 4) {
			switch(r) {
			case 0:
				decorGrid.setTile(Vector2f(x, y-1), 'f'+o*SNOW_OFFSET);
				break;
			case 1:
				decorGrid.setTile(Vector2f(x, y-1), o?'p'+SNOW_OFFSET : 'f');
				break;
			case 2:
				if(wide && growthMap.getTile(Vector2f(x+1,y)) == t) {
					o *= 10;
					treeGrid.setTile(Vector2f(x,   y-1), 30+o);
					treeGrid.setTile(Vector2f(x+1, y-1), 31+o);
					treeGrid.setTile(Vector2f(x,   y-2), 32+o);
					treeGrid.setTile(Vector2f(x+1, y-2), 33+o);
				} else
					decorGrid.setTile(Vector2f(x, y-1), 'r'+o*SNOW_OFFSET);
				break;
			case 3:
				decorGrid.setTile(Vector2f(x, y-1), 'b'+o*SNOW_OFFSET);
				break;
			}
		} else if(wide && (t == TREE || t == SNOWTREE) && growthMap.getTile(Vector2f(x+1,y)) == t) {
			o = (r > 6) ? 10 : 0;
			treeGrid.setTile(Vector2f(x,   y-1), 10+o);
			treeGrid.setTile(Vector2f(x+1, y-1), 11+o);
			treeGrid.setTile(Vector2f(x,   y-2), 12+o);
			treeGrid.setTile(Vector2f(x+1, y-2), 13+o);
			treeGrid.setTile(Vector2f(x,   y-3), 14+o);
			treeGrid.setTile(Vector2f(x+1, y-3), 15+o);
			treeGrid.setTile(Vector2f(x,   y-4), 16+o);
			treeGrid.setTile(Vector2f(x+1, y-4), 17+o);
		}

		x += 2 + std::rand() / ((RAND_MAX + 1u) / 3);
	}

	//Render decor map
	RandomIndexer decor(new MapIndexer(&decorGrid, decorDisplayIndex, -1), decorRandomIndex, -1);
	LargeTileMap decorMap(decorTexture, 32, 32, &decor, TREEMAP);
	decorMap.setScale(Vector2f(2, 2));
	UpdateList::addNodes(decorMap.getNodes());

	//Render tree map
	LargeTileMap treeMap(treeTexture, 64, 64, new MapIndexer(&treeGrid, treeDisplayIndex, -1), TREEMAP);
	UpdateList::addNodes(treeMap.getNodes());

	//Player
	Player player(collisionMapOn, collisionMapOff, frictionMap);
	player.setTexture(playerTexture);
	player.setPosition(Vector2f(0,0));
	player.background1 = &background1;
	player.background2 = &background2;
	player.background3 = &background3;
	player.background4 = &background4;
	UpdateList::addNode(&player);

	//Place player and boxes
	Indexer scaleMap(worldGrid.grid, ' ', Vector2i(64, 64));
	scaleMap.mapGrid([&player, &collisionMapOn, &collisionMapOff, &frictionMap, &treeGrid](uint c, Vector2f pos) {
		uint s = c - SNOW_OFFSET;
		if(c == 'P' && player.getPosition() == Vector2f(0,0))
			player.setPosition(pos + Vector2f(32, 16));
		else if(c == 'w' || s == 'w' || c == 'g' || s == 'g' || c == 'i' || s == 'i' || c == 'm' || s == 'm')
			new MovableBox(collisionMapOn, collisionMapOff, frictionMap, c, pos + Vector2f(32, 16), blocksTexture);
		else if(c == '_' || s == '_')
			new Button(pos + Vector2f(32, 60), false);
		else if(c == ',' || s == ',')
			new Button(pos + Vector2f(32, 60), true);
		else if(c == 'f' || s == 'f') {
			AnimatedNode *flag = new AnimatedNode(flagTexture, 6, 0.1, SIGN, Vector2i(30, 45));
			flag->setPosition(pos + Vector2f(32, 18));
			flag->setScale(Vector2f(2, 2));
			UpdateList::addNode(flag);
		} else if(c == 'E' || s == 'E') {
			AnimatedNode *yeti = new AnimatedNode(yetiTexture, 14, 0.1, ANIMATED, Vector2i(40, 40));
			yeti->setPosition(pos + Vector2f(16, 16));
			yeti->setScale(Vector2f(2, 2));
			UpdateList::addNode(yeti);
		} else if(c == 't' || s == 't') {
			Node *tent = new Node(ANIMATED, Vector2i(63, 37));
			tent->setPosition(pos + Vector2f(16, 7));
			tent->setScale(Vector2f(3, 3));
			tent->setTexture(tentTexture);
			tent->setTextureIntRect(IntRect(0, (s == 't') ? 37 : 0, 63, 37));
			UpdateList::addNode(tent);
		} else if(c == '>' || s == '>' || c == '<' || s == '<' || c == 'f' || s == 'f') {
			Node *sign = new Node(SIGN, Vector2i(64, 64));
			sign->setPosition(pos + Vector2f(32, 32));
			UpdateList::addNode(sign);
		}
	});

	Menu menu(menuButtonsTexture, &player);

	//Finish engine setup
	UpdateList::globalLayer(BACKGROUND);
	UpdateList::globalLayer(PLAYER);
	UpdateList::globalLayer(INPUT);
	UpdateList::globalLayer(TEXT);
	UpdateList::globalLayer(MENU);
	UpdateList::globalLayer(MENUBUTTON);

	//Start music
	UpdateList::musicStream("res/snow_game_jam.mp3", Settings::getInt("/music_volume", 100));

	UpdateList::setFont("res/goudy_mediaeval_regular.ttf");

	UpdateList::startEngine();
}

std::string TITLE = "Climbing Blocks";
std::string *windowTitle() {
	return &TITLE;
}

skColor backgroundColor() {
	return skColor(0,0,0,200);
}

std::vector<std::string> &textureFiles() {
	return TEXTURE_FILES;
}
std::vector<std::string> &layerNames() {
	return LAYER_NAMES;
}

void recieveNetworkString(std::string data, int code) {
	std::cout << "NETWORK: Received string " << data << "\n";
}