#include "Geo_Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_decoySquare = Geo_FlatSquare(PANE_RADIUS);
Geo_Actor Geo_PaneLayout::_decoyActor = Geo_Actor((Geo_Renderable*)&_decoySquare);

static std::string genPaneName(unsigned num) { return "pane" + std::to_string(num); }

void Geo_PaneLayout::resize(unsigned rows, unsigned columns) {
	_rows = rows;
	_columns = columns;

	// sizing root pane
	_rootSquare.scale((_height * 2) + _border, AXIS_Y);
	_rootSquare.scale((_width * 2) + _border, AXIS_X);

	// sizing child panes
	_panes.resize(getActorCount() - 1);
	_childSquare.scale(((_height * 2) / _rows) -_border, AXIS_Y);
	_childSquare.scale(((_width * 2) / _columns) -_border, AXIS_X);
}

void Geo_PaneLayout::configure(Topl_Scene* scene) {
	Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane

	// child panes
	for (unsigned p = 1; p < getActorCount(); p++) {
		Geo_Actor* actor = getNextActor();
		_panes[p - 1].actor = actor;
		actor->setRenderable((Geo_Renderable*)&_childSquare);

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
	rootActor->setRenderable((Geo_Renderable*)&_rootSquare);
	scene->addGeometry(getPrefix() + "root", rootActor);
#ifdef RASTERON_H
	scene->addTexture(getPrefix() + "root", _rootPane.getBackground()->getImage());
#endif
}