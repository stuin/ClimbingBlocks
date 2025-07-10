class Button : public Node {
	WorldSection *section = NULL;
	bool reset = true;
	bool trigger = false;

public:
	Button(Vector2f pos, bool player) : Node(BUTTON, Vector2i(30, 8), true) {
		setPosition(pos);

		collideWith(PLAYER, player);
		collideWith(BOX);
		collideWith(SECTION);

		UpdateList::addNode(this);
	}

	void update(double time) {
		if(section != NULL) {
			if(reset && trigger)
				section->triggers--;
			else if(!reset && !trigger)
				section->triggers++;
			trigger = !reset;
			section->triggers = std::max(0, section->triggers);
			section->trigger = section->triggers > 0;
			reset = true;
		}
	}

	void collide(Node *object) {
		if(object->getLayer() == SECTION) {
			section = (WorldSection *) object;
			collideWith(SECTION, false);
		} else
			reset = false;
	}

	void recieveSignal(int id, Node *sender) {
		if(id == RESET_SECTION && sender == section) {
			section->triggers--;
			section->trigger = section->triggers > 0;
		}
	}
};