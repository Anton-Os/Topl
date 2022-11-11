#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_decoySquare = Geo_FlatSquare(PANE_RADIUS);
Geo_Actor Geo_PaneLayout::_decoyActor = Geo_Actor((Geo_RenderObj*)&_decoySquare);

static std::string genPaneName(unsigned num) { return "pane" + std::to_string(num); }

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
	_rootSquare.modify(::stretchTForm, (_height * 2) + _border, AXIS_Y);
	_rootSquare.modify(::stretchTForm, (_width * 2) + _border, AXIS_X);

	// Modifying Child Panes RenderObj
	_panes.resize(getActorCount() - 1);
	_childSquare.modify(::stretchTForm, ((_height * 2) / _rows) -_border, AXIS_Y);
	_childSquare.modify(::stretchTForm, ((_width * 2) / _columns) -_border, AXIS_X);
}

void Geo_PaneLayout::configure(Topl_Scene* scene) {
	Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane
	Geo_Actor* actor = nullptr; // keeps track of child actor being processed 

	// child panes
	for (unsigned p = 1; p < getActorCount(); p++) {
		actor = getNextActor();
		actor->setRenderObj((Geo_RenderObj*)&_childSquare);

		float xInc = (_width / _columns);
		float yInc = (_height / _rows);
		Vec2f origin = Vec2f({(-0.5f * _width) + (xInc / 2.0f), (_height * 0.5f) - (yInc / 2.0f) });
		unsigned short xOffset = (p - 1) % _columns;
		unsigned short yOffset = (p - 1) / _columns;

		actor->updatePos(Vec3f({ origin[0] + (xInc * xOffset), origin[1] + (float)(-1.0 * yInc * yOffset), 0.0f })); // adjust these values
		scene->addGeometry(getPrefix() + genPaneName(p), actor);
#ifdef RASTERON_H
		scene->addTexture(getPrefix() + genPaneName(p), _panes[p - 1].getBackground());
#endif
	}

	// root pane
	rootActor->setRenderObj((Geo_RenderObj*)&_rootSquare);
	scene->addGeometry(getPrefix() + "root", rootActor);
#ifdef RASTERON_H
	scene->addTexture(getPrefix() + "root", _rootPane.getBackground());
#endif
}

bool Geo_PaneLayout::interact(float xPos, float yPos, unsigned color){
	if(xPos > getOrigin()[0] + _width || xPos < getOrigin()[0] - _width) return false; // lies out of x bounds
	if(yPos > getOrigin()[1] + _height || yPos < getOrigin()[1] - _height) return false; // lies out of y bounds
	
	for(std::vector<Geo_Pane>::iterator pane = _panes.begin(); pane != _panes.end(); pane++)
		if(pane->getColor() == color && pane->callback != nullptr)
			pane->callback(); // pane interaction has occured
	return true;
}