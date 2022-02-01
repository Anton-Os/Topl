#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_dummy = Geo_FlatSquare(PANE_RADIUS);
Geo_Actor Geo_PaneLayout::_dummyGeo = Geo_Actor((Geo_RenderObj*)&_dummy);

namespace _Pane {
	std::string genPaneName(unsigned num) { return "pane" + std::to_string(num); }
}

static float stretchTform(float input, double mod) { return input * mod; } // stretch transformation

Geo_Pane* Geo_PaneLayout::getChildPane(unsigned r, unsigned c) {
	if (r > _rows || c > _columns) {
		perror("Rows and Columns out of bounds!");
		return nullptr;
	}
	return &_panes.at((c * _columns) + r);
}

void Geo_PaneLayout::init(unsigned rows, unsigned columns) {
	_rows = rows;
	_columns = columns;

	_rootSquare.modify(stretchTform, (_radius * 2) + _border, AXIS_Y);
	_rootSquare.modify(stretchTform, (_radius * 2) + _border, AXIS_X);
	Rasteron_Image* rootBk = createImgBlank(256, 256, _rootPane.getColor()); // white solid background
	_rootPane.setImageBk(rootBk);

	_panes.resize(getActorCount() - 1);
	_childSquare.modify(stretchTform, ((_radius * 2) / _rows) - _border, AXIS_Y);
	_childSquare.modify(stretchTform, ((_radius * 2) / _columns) - _border, AXIS_X);
	for (std::vector<Geo_Pane>::iterator currentPane = _panes.begin(); currentPane < _panes.end(); currentPane++)
		currentPane->setImageBk(createImgBlank(256, 256, 0xFFFF0000)); // red color
		// currentPane->setImageBk(createImgBlank(256, 256, ValueGen::genRandColorVal())); // random background
}

void Geo_PaneLayout::fill(Topl_Scene* scene) {
	Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane
	Geo_Actor* actor = nullptr; // keeps track of child actor being processed 

    // Child Actors are added first
	for (unsigned p = 1; p < getActorCount(); p++) {
		actor = getNextActor();
		actor->setRenderObj((Geo_RenderObj*)&_childSquare);

		// positioning
		float xInc = (_radius * 1.5) / _columns;
		float yInc = (_radius * 1.5) / _rows;
		float _halfRadius = _radius / 1.5;
		Eigen::Vector2f origin = Eigen::Vector2f(-1.0f * (_radius - (_radius / _columns)), _radius - (_radius / _rows));
		origin *= 1.0 / (1.25 + 0.125 - 0.0625 + 0.03125); // scaling required since radius is scaled by 1.5
		// Eigen::Vector2f origin = Eigen::Vector2f(-1.0f * (_halfRadius - (_halfRadius / _columns)), _halfRadius - (_halfRadius / _rows));
		unsigned short xOffset = (p - 1) % _columns;
		unsigned short yOffset = (p - 1) / _columns;

		actor->setPos(Eigen::Vector3f(origin.x() + (xInc * xOffset), origin.y() + (-1.0 * yInc * yOffset), 0.0f)); // adjust these values
		// actor->setRot(Eigen::Vector2f(0.1f, 0.1f)); // for testing
		scene->addGeometry(getPrefix() + _Pane::genPaneName(p), actor);
#ifdef RASTERON_H
		scene->addTexture(getPrefix() + _Pane::genPaneName(p), _panes[p - 1].getBackground());
#endif

    // Root Actor is added to the scene last to correct drawing order
    rootActor->setRenderObj((Geo_RenderObj*)&_rootSquare);
	scene->addGeometry(getPrefix() + "root", rootActor);
#ifdef RASTERON_H
	scene->addTexture(getPrefix() + "root", _rootPane.getBackground());
#endif
	}
}