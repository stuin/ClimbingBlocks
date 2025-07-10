#pragma once

#include "Skyrmion/tiling/GridSectioner.h"
#include "indexes.h"

class WorldSection : public GridSection {
public:
	std::string signText = "";
	bool grabCamera = false;
	float zoomLevel = 0;

	bool invertTrigger = false;
	bool hasButton = false;
	int triggers = 0;
	bool trigger = false;

	WorldSection(GridSection *root, json data, Layer layer) : GridSection(root, data, layer) {
		signText = data.value("sign_text", "");
		grabCamera = data.value("grab_camera", false);
		zoomLevel = data.value("zoom", 0.0f);

		invertTrigger = data.value("invert_trigger", false);

		collideWith(BUTTON);
	}

	void collide(Node *other) {
		hasButton = true;
		collideWith(BUTTON, false);
	}
};