#include "Skyrmion/tiling/TileMap.hpp"

#define PUSHCOUNT 20

class GravityNode : public Node {
	float jumpTime = 0;
	float verticalSpeed = 0;
	float horizontalSpeed = 0;

	bool tempPlatforms = false;

	const int jumpPower = 300;
	const int jumpBoost = 8;
	const int fallSpeed = 48;
	const int fallMax = 600;
	const int slideSpeed = 300;
	const int slideMax = 200;
	const int slideReverse = 2;
	const int pushPower = 20;
	const bool showDebug = false;

public:
	Indexer *collision;
	WorldSection *section = NULL;
	std::vector<GravityNode *> colliding;
	bool blocked = false;
	bool isPlayer = false;
	int snapSpeed = 2;

	Vector2f pushDirection = Vector2f(0,0);
	std::vector<GravityNode *> pushing;
	float weight = 0;
	float pushWeight = 0;

	Indexer *frictionMap;
	float frictionValue = 1;

	GravityNode(Indexer *_collision, Indexer *_friction, Layer layer, Vector2i size) :
	Node(layer, size), collision(_collision), frictionMap(_friction) {

	}

	Vector2f gravityVelocity(Vector2f input, double time) {
		bool jumpInput = input.y < -0.5;
		Vector2f velocity = Vector2f((input.x + pushDirection.x) * time, 0);
		Vector2f pos = getPosition();
		Vector2f foot = pos;
		foot.y += getSize().y / 2 - 2;
		Vector2f footL = foot - Vector2f(getSize().x / 2, 0);
		Vector2f footR = foot + Vector2f(getSize().x / 2, 0);
		foot.y += 6;

		if(isPlayer && showDebug)
			std::cout << velocity.x << "," << velocity.y << ">1 ";

		//Friction
		float friction = (frictionMap->getTile(foot) / 100.0) * frictionValue;
		friction = std::clamp(0.01f, friction, 1.0f);
		if(friction < 1 && (collision->getTile(foot) != EMPTY ||
			collision->getTile(footL) == SLOPE_UPLEFT || collision->getTile(footR) == SLOPE_UPRIGHT)) {

			velocity.x *= friction;
			velocity.x += horizontalSpeed * (1.0 - friction/3) * time;

			if((collision->getTile(footL) == SLOPE_UPLEFT || collision->getTile(foot) == SLOPE_UPLEFT) && velocity.x > -slideReverse) {
				velocity.x += slideSpeed * (1 - friction) * std::min(pushWeight*2, 1.0f) * time;
				velocity.x = std::min(velocity.x, (float)(slideMax * time));
			}
			if((collision->getTile(footR) == SLOPE_UPRIGHT || collision->getTile(foot) == SLOPE_UPRIGHT) && velocity.x < slideReverse) {
				velocity.x -= slideSpeed * (1 - friction) * std::min(pushWeight*2, 1.0f) * time;
				velocity.x = std::max(velocity.x, -(float)(slideMax * time));
			}
		}

		if(isPlayer && showDebug)
			std::cout << velocity.x << "," << velocity.y << ">2 ";

		//Check for wall
		tempPlatforms = section != NULL && (section->trigger != section->invertTrigger);
		Vector2f collisionOffset = velocity + Vector2f(velocity.x / std::abs(velocity.x) * getSize().x / 2, getSize().y / 4);
		if(verticalSpeed != 0 || foot.y - 8 < collision->snapPosition(foot).y) {
			if(collision->getTile(pos + collisionOffset) == FULL)
				velocity.x = 0;
			else if(velocity.x > 0 && collision->getTile(pos) != SLOPE_UPLEFT && collision->getTile(pos + collisionOffset) == SLOPE_UPLEFT)
				velocity.x = 0;
			else if(velocity.x < 0 && collision->getTile(pos) != SLOPE_UPRIGHT && collision->getTile(pos + collisionOffset) == SLOPE_UPRIGHT)
				velocity.x = 0;
		}

		if(isPlayer && showDebug)
			std::cout << velocity.x << "," << velocity.y << ">f ";

		//Falling and jumping
		foot += velocity;
		footL = foot - Vector2f(getSize().x / 4, 0);
		footR = foot + Vector2f(getSize().x / 4, 0);
		if(isPlayer && showDebug)
			std::cout << foot.x-pos.x << "," << foot.y-pos.y << ">t ";
		int tileL = collision->getTile(footL);
		int tileR = collision->getTile(footR);
		if(isPlayer && showDebug)
			std::cout << tileL << "," << tileR << ":" << EMPTY << ">3 ";
		if(tileL == EMPTY && tileR == EMPTY) {
			if(jumpTime < 0.2 && jumpInput)
				verticalSpeed -= jumpBoost;
			else
				verticalSpeed += fallSpeed;
			verticalSpeed = std::min(verticalSpeed, (float)fallMax);
			velocity.y += verticalSpeed * time;

			if(isPlayer && showDebug)
				std::cout << velocity.x << "," << velocity.y << ">4 ";

			//Ceiling check
			if(collision->getTile(pos + velocity) != EMPTY && collision->getTile(pos + velocity) != ONEWAY_UP) {
				verticalSpeed = 0;
				velocity.y = 0;
				jumpTime += 0.2;
			}

			foot.y += verticalSpeed * time;
			jumpTime += time;
		} else if(jumpInput && jumpTime == 0) {
			//Start jump
			verticalSpeed = -jumpPower;
			velocity.y += verticalSpeed * time;
		}

		if(isPlayer && showDebug)
			std::cout << velocity.x << "," << velocity.y << ">5 ";

		//Snap to ground
		int tile = collision->getTile(foot);
		if(tile != EMPTY && !(jumpInput && jumpTime < 0.2)) {

			Vector2f ground = collision->snapPosition(foot);
			Vector2f foot2 = foot - Vector2f(0, 8);

			//Allow for upwards slope
			if(tile != EMPTY && tile != SLOPE_UPLEFT && tile != SLOPE_UPRIGHT &&
				(collision->getTile(foot2) == SLOPE_UPLEFT || collision->getTile(foot2) == SLOPE_UPRIGHT)) {
				foot = foot2;
				ground = collision->snapPosition(foot);
			}
			velocity.y += ground.y - pos.y - getSize().y/2;

			if(collision->getTile(foot) == SLOPE_UPLEFT)
				velocity.y -= ground.x - pos.x;
			else if(collision->getTile(foot) == SLOPE_UPRIGHT)
				velocity.y -= pos.x - ground.x - collision->getScale().x;

			velocity.y = std::min(std::max((float)snapSpeed, verticalSpeed * (float)time), velocity.y);

			if(velocity.y == 0) {
				verticalSpeed = 0;
				jumpTime = 0;
			}
		}

		if(isPlayer && showDebug)
			std::cout << velocity.x << "," << velocity.y << ">6 ";

		pushing.clear();
		pushWeight = 0;
		for(GravityNode *other : colliding) {
			if(isAbove(other)) {
				if(jumpInput && jumpTime == time) {
					jumpTime = 0;
					verticalSpeed = -jumpPower;
					velocity.y += verticalSpeed * time;
				} else if(velocity.y > 0 && other->verticalSpeed >= 0) {
					velocity.y = (other->getPosition().y - other->getSize().y/2) - (pos.y + getSize().y/2)+2;
					//velocity.y += other->verticalSpeed * time;
					jumpTime = 0;

					other->pushWeight += pushWeight;
				}
			} else if(velocity.x != 0 && (other->getPosition().x - getPosition().x) * velocity.x > 0) {
				if(other->blocked || other->pushWeight > 1)
					velocity.x = 0;

				pushing.push_back(other);
				pushWeight += other->pushWeight;
			}
		}
		colliding.clear();

		pushWeight += weight;
		velocity.x *= 1-std::clamp(0.0f, pushWeight * friction, 1.0f);

		pushDirection = Vector2f(0,0);
		for(GravityNode *other : pushing) {
			other->pushDirection.x = velocity.x / time;
			if(velocity.x > 0)
				velocity.x = std::min(other->horizontalSpeed * (float)time, velocity.x);
			else
				velocity.x = std::max(other->horizontalSpeed * (float)time, velocity.x);
		}

		if(isPlayer && showDebug)
			std::cout << velocity.x << "," << velocity.y << ">7\n";

		horizontalSpeed = velocity.x / time;
		blocked = std::abs(input.x) > 0.1 && std::abs(velocity.x) < 0.1;
		return velocity;
	}

	bool isAbove(Node *other) {
		float dx = getPosition().x - other->getPosition().x;
		float dy = (other->getPosition().y - other->getSize().y/2) - (getPosition().y + getSize().y/4);
		float side = getSize().x/3 + other->getSize().x/2;
		return std::abs(dx) < side && dy > 0;
	}

	void addCollision(GravityNode *other) {
		colliding.push_back(other);
	}
};