#pragma once

#include "world.h"

namespace sand
{

void drawPanel(Element selected, int brushRadius, bool paused);
void handleSelectionKeys(Element& selected);
void handleSimKeys(World& world, bool& paused, bool& stepRequested);
void updateBrushRadius(int& brushRadius);
void applyBrushFromMouse(World& world, int brushRadius, Element selected, int& cellX, int& cellY, bool& mouseInGrid);
void pickUnderCursor(const World& world, int cellX, int cellY, bool mouseInGrid, Element& selected);
void drawBrushRing(int cellX, int cellY, int brushRadius, bool mouseInGrid);

} // namespace sand
