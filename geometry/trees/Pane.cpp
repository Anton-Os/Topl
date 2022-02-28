#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_dummy = Geo_FlatSquare(PANE_RADIUS);
Geo_Actor Geo_PaneLayout::_dummyGeo = Geo_Actor((Geo_RenderObj*)&_dummy);

namespace _Pane {
	std::string genPaneName(unsigned num) { return "pane" + std::to_string(num); }

	void sendRootPaneThruScene(Topl_Scene* scene, std::string prefix, Geo_Pane* pane, Geo_Actor* paneActor, Geo_RenderObj* rootSquare){
		paneActor->setRenderObj(rootSquare);
		scene->addGeometry(prefix + "root", paneActor);
#ifdef RASTERON_H
		scene->addTexture(prefix + "root", pane->getBackground());
#endif
	}
}

Geo_Pane* Geo_PaneLayout::getChildPane(unsigned index) {
	if (index > _panes.size()) {
		perror("Pane index is out of bounds!");
		return nullptr;
	}
	return &_panes.at(index);
}

void Geo_PaneLayout::resize(unsigned rows, unsigned columns) {
	_rows = rows;
	_columns = columns;

	// Modifying Root Pane RenderObj
	_rootSquare.modify(::stretchTForm, (_radius * 2) + _border, AXIS_Y);
	_rootSquare.modify(::stretchTForm, (_radius * 2) + _border, AXIS_X);

	// Modifying Child Panes RenderObj
	_panes.resize(getActorCount() - 1);
	_childSquare.modify(::stretchTForm, ((_radius * 2) / _rows) - _border, AXIS_Y);
	_childSquare.modify(::stretchTForm, ((_radius * 2) / _columns) - _border, AXIS_X);
}

void Geo_PaneLayout::init(Topl_Scene* scene) {
	Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane
	Geo_Actor* actor = nullptr; // keeps track of child actor being processed 

	// sending child panes thru scene
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

		actor->updatePos(Eigen::Vector3f(origin.x() + (xInc * xOffset), origin.y() + (-1.0 * yInc * yOffset), 0.0f)); // adjust these values
		// actor->setRot(Eigen::Vector2f(0.1f, 0.1f)); // for testing
		scene->addGeometry(getPrefix() + _Pane::genPaneName(p), actor);
#ifdef RASTERON_H
		scene->addTexture(getPrefix() + _Pane::genPaneName(p), _panes[p - 1].getBackground());
#endif
	}

	// utilizing helper function to send root pane thru scene
	_Pane::sendRootPaneThruScene(scene, getPrefix(), &_rootPane, rootActor, (Geo_RenderObj*)&_rootSquare);
}