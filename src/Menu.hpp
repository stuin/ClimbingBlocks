class Menu : public Node {
private:
	//sf::RectangleShape backgroundShape;

	Node *startButton;
	Node *restartButton;
	Node *saveButton;
	Node *quitButton;

public:

	Menu(int buttonTextures, Node *parent) : Node(MENU, Vector2i(512, 192), false, parent) {
		//backgroundShape.setFillColor(sf::Color(0,0,0,200));
		//backgroundShape.setSize(Vector2f(512, 192));
		setHidden();
		setPosition(Vector2f(0,-64));
		setScale(getScale(), false);
		setTextureIntRect(IntRect(0, 0, 0, 0));
		UpdateList::addListener(this, EVENT_MOUSE);
		UpdateList::addNode(this);

		startButton = new Node(MENUBUTTON, Vector2i(128, 48), false, this);
		startButton->setPosition(Vector2f(-160, -16));
		startButton->setTexture(buttonTextures);
		startButton->setTextureIntRect(IntRect(0, 0, 128, 48));
		UpdateList::addNode(startButton);

		restartButton = new Node(MENUBUTTON, Vector2i(128, 48), false, this);
		restartButton->setPosition(Vector2f(-160, 48));
		restartButton->setTexture(buttonTextures);
		restartButton->setTextureIntRect(IntRect(0, 48, 128, 48));
		UpdateList::addNode(restartButton);

		saveButton = new Node(MENUBUTTON, Vector2i(128, 48), false, this);
		saveButton->setPosition(Vector2f(160, -16));
		saveButton->setTexture(buttonTextures);
		saveButton->setTextureIntRect(IntRect(0, 48*2, 128, 48));
		UpdateList::addNode(saveButton);

		quitButton = new Node(MENUBUTTON, Vector2i(128, 48), false, this);
		quitButton->setPosition(Vector2f(160, 48));
		quitButton->setTexture(buttonTextures);
		quitButton->setTextureIntRect(IntRect(0, 48*3, 128, 48));
		UpdateList::addNode(quitButton);
	}

	void recieveEvent(Event event) {
		if(!isHidden() && event.code == 0 && event.down) {
			Vector2f pos = screenToGlobal(event.x, event.y);
			if(getRect().contains(pos)) {
				if(startButton->getRect().contains(pos))
					pauseGame(false);
				else if(restartButton->getRect().contains(pos))
					UpdateList::sendSignal(RESET_GAME, this);
				else if(saveButton->getRect().contains(pos))
					saveGame();
				else if(quitButton->getRect().contains(pos))
					UpdateList::stopEngine();
			}
		}
	}

	void recieveSignal(int id, Node *sender) {
		if(id == TOGGLE_MENU)
			pauseGame(isHidden());
	}

	void pauseGame(bool pause) {
		setHidden(!pause);
	}

	void saveGame() {
		UpdateList::sendSignal(PLAYER, SAVE_GAME, this);
		pauseGame(false);
	}
};