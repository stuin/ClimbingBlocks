#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/input/MovementSystems.h"
#include "Skyrmion/input/Settings.h"
#include "GridSection.hpp"

class MovableBox : public Node, public PhysicsObject {
	Indexer *collisionOn;
	Indexer *collisionOff;
	Vector2f startPosition;

	Indexer *frictionMap;
	float frictionValue = 1;
	float rotation = 0;

	PersonalPhysicsStats *physics = new PersonalPhysicsStats();
	GlobalPhysicsStats *globalPhysics = new GlobalPhysicsStats();
	std::vector<PersonalPhysicsStats *> colliding;

	WorldSection *section = NULL;
	WorldSection *mainSection = NULL;

public:
	MovableBox(Indexer *_collisionOn, Indexer *_collisionOff, Indexer *_friction, uint c, Vector2f _startPosition, int blockTexture) :
	Node(BOX, Vector2i(16, 16)), collisionOn(_collisionOn), collisionOff(_collisionOff), startPosition(_startPosition),
	frictionMap(_friction) {

		setPosition(startPosition);
		setScale(Vector2f(3, 3));
		setTexture(blockTexture);
		physics->weight = 0.2;
		frictionValue = 0.85;

		switch(c) {
		case 'i': case 'i'+SNOW_OFFSET:
			setTextureIntRect(IntRect(0, 16, 16, 16));
			frictionValue = 0.2;
			physics->weight = 0.05;
			break;
		case 'w': case 'w'+SNOW_OFFSET:
			setTextureIntRect(IntRect(48, 0, 16, 16));
			break;
		case 'g': case 'g'+SNOW_OFFSET:
			setTextureIntRect(IntRect(32, 0, 16, 16));
			physics->weight = 0.5;
			break;
		case 'm': case 'm'+SNOW_OFFSET:
			setTextureIntRect(IntRect(16, 16, 16, 16));
			physics->weight = 0.5;
			setScale(Vector2f(5, 5));
			break;
		}

		collideWith(PLAYER);
		collideWith(BOX);
		collideWith(SECTION);
		UpdateList::addNode(this);
	}

	void update(double time) {
		bool tempPlatforms = section != NULL && (section->trigger != section->invertTrigger);
		Indexer *collision = tempPlatforms ? collisionOn : collisionOff;

		//std::cout << pushDirection << "\n";
		Vector2f velocity = physics->pushDirection * (float)time;
		velocity = platformFrictionMovement(getPosition(), velocity, getSize(), time,
			physics->previous, collision, frictionMap, frictionValue, globalPhysics);
		velocity = platformGravityMovement(getPosition(), velocity, getSize(), time, false,
			collision, globalPhysics, physics, colliding);
		setPosition(getPosition() + velocity);
		colliding.clear();

		if(!Settings::getBool("/debug_block_rotation")) {
			int floor = collision->getTile(getPosition() + Vector2f(0, getSize().x/2+4));
			if(floor == SLOPE_UPLEFT)
				rotation = -45;
			else if(floor == SLOPE_UPRIGHT)
				rotation = 45;
			else
				rotation = 0;
		}
	}

	void collide(Node *other) {
		if(other->getLayer() == SECTION) {
			if(mainSection == NULL)
				mainSection = (WorldSection *) other;
			section = (WorldSection *) other;
		} else if(other->getLayer() == BOX || other->getLayer() == PLAYER)
			colliding.push_back(dynamic_cast<PhysicsObject*>(other)->getPhysics());
	}

	void recieveSignal(int id, Node *sender) {
		if(id == RESET_SECTION && (sender == section || sender == mainSection))
			setPosition(startPosition);
		else if(id == RESET_GAME)
			setPosition(startPosition);
	}

	PersonalPhysicsStats *getPhysics() override {
		return physics;
	}
};