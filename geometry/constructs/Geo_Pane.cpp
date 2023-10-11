#include "Geo_Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_decoySquare = Geo_FlatSquare(PANE_RADIUS);
Geo_Actor Geo_PaneLayout::_decoyActor = Geo_Actor((Geo_Mesh*)&_decoySquare);

static std::string genPaneName(unsigned num) { return "pane" + std::to_string(num); }

void Geo_PaneLayout::resize(unsigned rows, unsigned columns) {
	_rows = rows;
	_columns = columns;

	_rootSquare.scale({(_width * 2) + _border, (_height * 2) + _border, 0.0f});

	_panes.resize(_geoActors.size() - 1); // resizing child panes
	_childSquare.scale({((_width * 2) / _columns) - _border, ((_height * 2) / _rows) - _border, 0.0f});
}

void Geo_PaneLayout::configure(Topl_Scene* scene) {
	Geo_Actor* rootActor = &_geoActors[0]; // 1st actor is root pane

	// child panes
	for (unsigned p = 1; p < _geoActors.size(); p++) {
		Geo_Actor* actor = &_geoActors[p];
		_panes[p - 1].actor = actor;
		actor->setMesh((Geo_Mesh*)&_childSquare);

		float xInc = (_width / _columns);
		float yInc = (_height / _rows);
		Vec2f origin = Vec2f({(-0.5f * _width) + (xInc / 2.0f), (_height * 0.5f) - (yInc / 2.0f) });
		unsigned short xOffset = (p - 1) % _columns;
		unsigned short yOffset = (p - 1) / _columns;

		actor->updatePos(Vec3f({ origin[0] + (xInc * xOffset), origin[1] + (float)(-1.0 * yInc * yOffset), 0.0f })); // adjust these values
		scene->addGeometry(getPrefix() + genPaneName(p), actor);
#ifdef RASTERON_H
		scene->addTexture(getPrefix() + genPaneName(p), _panes[p - 1].getBackground()->getImage());
#endif
	}

	// root pane
	_rootPane.actor = rootActor;
	rootActor->setMesh((Geo_Mesh*)&_rootSquare);
	scene->addGeometry(getPrefix() + "root", rootActor);
#ifdef RASTERON_H
	scene->addTexture(getPrefix() + "root", _rootPane.getBackground()->getImage());
#endif
}