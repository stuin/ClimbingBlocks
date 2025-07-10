#include "Skyrmion/core/UpdateList.h"
#include "Skyrmion/util/AnimatedNode.hpp"

#include "MovableBox.hpp"
#include "Menu.hpp"

std::vector<std::string> miscLayout = {
	"/controls/zoom_in", "/controls/zoom_out", "/controls/reset_room", "/controls/menu"
};

class Player : public Node, public PhysicsObject {
	Indexer *collisionOn;
	Indexer *collisionOff;
	DirectionHandler moveInput;
	InputHandler miscInput;

	Indexer *frictionMap;
	float frictionValue = 1;
	PersonalPhysicsStats *physics = new PersonalPhysicsStats();
	GlobalPhysicsStats *globalPhysics = new GlobalPhysicsStats();
	std::vector<PersonalPhysicsStats *> colliding;

	const float scaleFactor = 3;
	Vector2f enterPoint = Vector2f(0,0);
	Vector2f startPoint = Vector2f(0,0);

	//sf::RectangleShape textShape;
	//sf::Text text;
	Node *textBackground;
	Node *textNode;
	bool textVisible = false;

	WorldSection *section = NULL;
	bool lastTrigger = false;
	bool triggerOverride = false;

	Node *camera = NULL;
	float zoomLevel = 3.0;
	float zoomTarget = 3.0;

	Vector2f lowCorner = Vector2f(0,0);
	Vector2f highCorner = Vector2f(0,0);
	Vector2f center = Vector2f(0,0);

	int frameWidth = 0;
	int frameHeight = 0;
	int animationSet = 0;
    FrameTimer timer;

public:
	Node background1 = Node(BACKGROUND, Vector2i(4096, 4096));
	Node background2 = Node(BACKGROUND, Vector2i(4096, 4096));
	Node background3 = Node(BACKGROUND, Vector2i(4096, 4096));
	Node background4 = Node(BACKGROUND, Vector2i(4096, 4096));

	Player(Indexer *_collisionOn, Indexer *_collisionOff, Indexer *_friction) :
	Node(PLAYER, Vector2i(22, 29)), collisionOn(_collisionOn), collisionOff(_collisionOff),
		moveInput("/movement", INPUT, this), miscInput(miscLayout, INPUT, this), frictionMap(_friction), timer(8, 0.1) {

		//Setup text box
		textBackground = new Node(TEXT, Vector2i(16, 16), true, this);
		textBackground->setColor(skColor(0,0,0,200));
		textBackground->setTexture(solidTexture);
		textBackground->setScale(Vector2i(1,1), false);
		textNode = new Node(TEXT, Vector2i(16, 16), false, textBackground);
		textNode->setPosition(Vector2f(14, 6));
		textNode->setSize(Vector2i(25, 25));
		UpdateList::addNode(textBackground);
		UpdateList::addNode(textNode);

		//Load backgrounds
		background1.setTexture(backgroundTexture1);
		background2.setTexture(backgroundTexture2);
		background3.setTexture(backgroundTexture3);
		background4.setTexture(backgroundTexture4);
		UpdateList::addNode(&background4);
		UpdateList::addNode(&background3);
		UpdateList::addNode(&background2);
		UpdateList::addNode(&background1);

		//Background position math
		lowCorner = Vector2f(0, _collisionOff->getSize().y * _collisionOff->getScale().y);
		//lowCorner.y = std::max(lowCorner.y, 4096.0f);
		highCorner = Vector2f(_collisionOff->getSize().x * _collisionOff->getScale().x * 1.5, 0);
		center = Vector2f(highCorner.x / 2, lowCorner.y / 2);
		//std::cout << _collisionOff->getSize().x * _collisionOff->getScale().x << "," << _collisionOff->getSize().y * _collisionOff->getScale().y << "\n";

		//Camera position
		camera = new Node(INPUT, Vector2i(450, 250), true, this);
		camera->setScale(camera->getScale(), false);
		UpdateList::setCamera(camera, Vector2f(450, 250) * zoomLevel);

		//Non movement keyboard input
		Player *_player = this;
		miscInput.pressedFunc = [_player](int i) {
			if(i < 2) {
				_player->zoomTarget = std::clamp(2.0, _player->zoomTarget + 0.2 * (i ? 1 : -1), 300.0);
			} else if(i == 2) {
				_player->setPosition(_player->enterPoint);
				UpdateList::sendSignal(BOX, RESET_SECTION, _player->section);
			} else if(i == 3) {
				UpdateList::sendSignal(MENU, TOGGLE_MENU, _player->section);
			}
		};

		//Animations
		frameWidth = 22;
		frameHeight = 29;
		setTextureIntRect(IntRect(0, animationSet * frameHeight, frameWidth, frameHeight));

		//Physics
		setScale(Vector2f(scaleFactor, scaleFactor));
		physics->snapSpeed = 6;
		physics->weight = 0.01;
		//physics->showDebug = true;

		collideWith(BOX);
		collideWith(SECTION);
		collideWith(SIGN);
	}

	void update(double time) {
		if(startPoint == Vector2f(0,0)) {
			startPoint = getPosition();
			if(Settings::getBool("/save/active")) {
				Vector2f target = Vector2f(0,0);
				target.x = Settings::getInt("/save/x");
				target.y = Settings::getInt("/save/y");
				setPosition(target);
			}
		}

		bool tempPlatforms = section != NULL && (section->trigger != section->invertTrigger);
		Indexer *collision = tempPlatforms ? collisionOn : collisionOff;

		Vector2f input = moveInput.getDirection();
		bool jumpInput = input.y < -0.5;
		Vector2f velocity = Vector2f(input.x * time * 320.0f, 0);
		velocity = platformFrictionMovement(getPosition(), velocity, getSize(), time,
			physics->previous, collision, frictionMap, frictionValue, globalPhysics);
		velocity = platformGravityMovement(getPosition(), velocity, getSize(), time, jumpInput,
			collision, globalPhysics, physics, colliding);
		setPosition(getPosition() + velocity);
		colliding.clear();

		UpdateList::hideLayer(TEMPMAP, section == NULL || (!triggerOverride && section->trigger == section->invertTrigger));
		if(section != NULL && section->hasButton)
			lastTrigger = section->trigger != section->invertTrigger;

		//Flip horizontally
		if(velocity.x < 0)
			setScale(Vector2f(-scaleFactor, scaleFactor));
		else if(velocity.x > 0)
			setScale(Vector2f(scaleFactor, scaleFactor));

		//Animation
		if(input.y < -0.5 && velocity.y < -0.2) {
			if(animationSet != 2) {
				timer.frame = 0;
				animationSet = 2;
			}
		} else if(velocity.y > 5 || (input.y < -0.5 && animationSet == 2 && timer.frame > 2)) {
			if(animationSet != 2) {
				timer.frame = 3;
				animationSet = 2;
			}
		} else if(std::abs(input.x) > 0.1) {
			if(animationSet != 1) {
				timer.frame = 0;
				animationSet = 1;
			}
		} else
			animationSet = 0;

		if(timer.next(time))
            setTextureIntRect(IntRect(frameWidth * timer.frame, animationSet * frameHeight, frameWidth, frameHeight));

		//Slide camera
		if(camera->getPosition() != Vector2f(0,-64 / scaleFactor)) {
			Vector2f target = Vector2f(0,-64 / scaleFactor) - camera->getPosition();
			Vector2f target2 = vectorLength(target, 400 * time);
			if(std::abs(target.x) > std::abs(target2.x) && std::abs(target.x) > std::abs(target2.x))
				target = target2;
			camera->setPosition(camera->getPosition() + target);
		}

		//Place backgrounds
		background1.setPosition(Vector2f(lerp(center.x, camera->getGPosition().x, 0.85), lerp(center.y+600, camera->getGPosition().y+600, 0.85)));
		background2.setPosition(Vector2f(lerp(center.x, camera->getGPosition().x, 0.90), lerp(center.y+1500, camera->getGPosition().y+1500, 0.90)));
		background3.setPosition(Vector2f(lerp(center.x, camera->getGPosition().x, 0.95), lerp(center.y+1500, camera->getGPosition().y+1500, 0.95)));
		background4.setPosition(Vector2f(lerp(center.x, camera->getGPosition().x, 1), lerp(center.y, camera->getGPosition().y, 1)));

		//background1->setScale(Vector2f(1/0.7, 1/0.7));
		background2.setScale(Vector2f(1.3, 1.3));
		background3.setScale(Vector2f(1.3, 1.3));
		background4.setScale(Vector2f(1.3, 1.3));

		//Update zoom level
		if(zoomLevel != zoomTarget) {
			zoomTarget = std::clamp(-10.0f, zoomTarget, 10.0f);
			zoomLevel += std::clamp(-0.02f, zoomTarget - zoomLevel, 0.02f);
			UpdateList::setCamera(camera, Vector2f(450, 250) * zoomLevel);
		}

		if(!textVisible)
			textBackground->setHidden(true);
		textVisible = false;
	}

	void collide(Node *other) {
		if(other->getLayer() == SECTION) {
			if(section != other) {
				section = (WorldSection *)other;
				enterPoint = getPosition();
				//std::cout << "Entering room\n";

				//Adjust camera
				Vector2f cameraPosition = camera->getGPosition();
				if(section->grabCamera)
					camera->setParent(other);
				else
					camera->setParent(this);
				camera->setGPosition(cameraPosition);
				if(section->zoomLevel != 0)
					zoomTarget = section->zoomLevel;

				if(!triggerOverride && lastTrigger && !section->hasButton)
					triggerOverride = true;
				else {
					triggerOverride = false;
					lastTrigger = section->trigger != section->invertTrigger;
				}
			}
		} else if(other->getLayer() == SIGN) {
			if(section != NULL && section->signText != "") {
				textNode->setString(section->signText.c_str());
				textNode->setPosition(Vector2f(16.0, 16));
				textBackground->setSize(Vector2f(10.0 * section->signText.length(), 38));
				textBackground->setOrigin(Vector2f(0,0));
				textBackground->setHidden(false);
				textBackground->setPosition(Vector2f(-4.0 * section->signText.length(), -80));
				textVisible = true;
			}
		} else if(other->getLayer() == BOX)
			colliding.push_back(dynamic_cast<PhysicsObject*>(other)->getPhysics());
	}

	void recieveSignal(int id, Node *sender) {
		if(id == RESET_GAME)
			setPosition(startPoint);
		else if(id == SAVE_GAME) {
			if(section->id != 0) {
				Settings::setBool("/save/active", true);
				Settings::setInt("/save/section", section->id);
				Settings::setInt("/save/x", getPosition().x);
				Settings::setInt("/save/y", getPosition().y);
			} else {
				Settings::setBool("/save/active", false);
				Settings::setInt("/save/section", 0);
				Settings::setInt("/save/x", 0);
				Settings::setInt("/save/y", 0);
			}
			Settings::save("res/settings.json");
		}
	}

	PersonalPhysicsStats *getPhysics() override {
		return physics;
	}

	float lerp(float a, float b, float f) {
	    return (a * (1.0 - f)) + (b * f);
	}
};

Node *spawnPlayer(Indexer *collisionMapOn, Indexer *collisionMapOff, Indexer *frictionMap) {
	Player *player = new Player(collisionMapOn, collisionMapOff, frictionMap);
	player->setTexture(playerTexture);
	player->setPosition(Vector2f(0,0));
	UpdateList::addNode(player);

	new Menu(menuButtonsTexture, player);

	return player;
}