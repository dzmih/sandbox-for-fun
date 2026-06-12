#include "ui.h"

#include <algorithm>
#include <string>

namespace sand
{

void drawPanel(Element selected, int brushRadius, bool paused)
{
    const int x = GridWidth * CellSize;
    DrawRectangle(x, 0, PanelWidth, ScreenHeight, {19, 23, 31, 255});
    DrawText("Falling Sand", x + 18, 18, 24, RAYWHITE);
    DrawText(paused ? "Paused" : "Running", x + 18, 52, 18, paused ? YELLOW : GREEN);
    DrawText(TextFormat("Selected: %s", infoFor(selected).name), x + 18, 72, 18, LIGHTGRAY);

    DrawText("Materials", x + 18, 92, 18, RAYWHITE);
    for (int i = 1; i < static_cast<int>(Materials.size()); ++i)
    {
        const auto& material = Materials[static_cast<std::size_t>(i)];
        const int rowY = 122 + (i - 1) * 34;
        const bool isSelected = selected == material.material;

        DrawRectangleLinesEx({static_cast<float>(x + 14), static_cast<float>(rowY - 5), 196.0f, 28.0f}, 2.0f,
                             isSelected ? YELLOW : DARKGRAY);
        DrawRectangle(x + 22, rowY, 18, 18, material.color);

        const std::string label = std::to_string(i) + " - " + material.name;
        DrawText(label.c_str(), x + 50, rowY, 16, RAYWHITE);
    }

    DrawText(TextFormat("Brush: %d", brushRadius), x + 18, 356, 18, RAYWHITE);
    DrawText("Left mouse: paint", x + 18, 404, 15, LIGHTGRAY);
    DrawText("Right mouse: erase", x + 18, 426, 15, LIGHTGRAY);
    DrawText("Middle mouse: pick", x + 18, 448, 15, LIGHTGRAY);
    DrawText("Wheel: brush size", x + 18, 470, 15, LIGHTGRAY);
    DrawText("Q/E: prev/next material", x + 18, 492, 15, LIGHTGRAY);
    DrawText("Space: pause, N: step", x + 18, 514, 15, LIGHTGRAY);
    DrawText("C: clear", x + 18, 536, 15, LIGHTGRAY);
}

void handleSelectionKeys(Element& selected)
{
    // direct keys are just faster to use
    if (IsKeyPressed(KEY_ONE))
    {
        selected = Element::Sand;
    }
    else if (IsKeyPressed(KEY_TWO))
    {
        selected = Element::Water;
    }
    else if (IsKeyPressed(KEY_THREE))
    {
        selected = Element::Wall;
    }
    else if (IsKeyPressed(KEY_FOUR))
    {
        selected = Element::Wood;
    }
    else if (IsKeyPressed(KEY_FIVE))
    {
        selected = Element::Fire;
    }
    else if (IsKeyPressed(KEY_SIX))
    {
        selected = Element::Acid;
    }

    int cycleDir = 0;
    if (IsKeyPressed(KEY_Q))
    {
        cycleDir = -1;
    }
    else if (IsKeyPressed(KEY_E))
    {
        cycleDir = 1;
    }

    if (cycleDir != 0)
    {
        // wrap from first to last slot and back
        int slot = findMaterialSlot(selected);
        slot += cycleDir;
        if (slot < 1)
        {
            slot = static_cast<int>(Materials.size()) - 1;
        }
        else if (slot >= static_cast<int>(Materials.size()))
        {
            slot = 1;
        }

        selected = Materials[static_cast<std::size_t>(slot)].material;
    }
}

void handleSimKeys(World& world, bool& paused, bool& stepRequested)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        paused = !paused;
    }
    if (IsKeyPressed(KEY_C))
    {
        world.clear();
    }
    if (IsKeyPressed(KEY_N))
    {
        stepRequested = true;
    }
}

void updateBrushRadius(int& brushRadius)
{
    const float wheel = GetMouseWheelMove();
    if (wheel == 0.0f)
    {
        return;
    }

    constexpr int MinBrush = 1;
    constexpr int MaxBrush = 18;
    brushRadius = std::clamp(brushRadius + static_cast<int>(wheel), MinBrush, MaxBrush);
}

void applyBrushFromMouse(World& world, int brushRadius, Element selected, int& cellX, int& cellY, bool& mouseInGrid)
{
    const Vector2 mouse = GetMousePosition();
    cellX = static_cast<int>(mouse.x) / CellSize;
    cellY = static_cast<int>(mouse.y) / CellSize;
    mouseInGrid = inBounds(cellX, cellY);

    if (!mouseInGrid)
    {
        return;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        world.paint(cellX, cellY, brushRadius, selected);
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        world.paint(cellX, cellY, brushRadius, Element::Empty);
    }
}

void pickUnderCursor(const World& world, int cellX, int cellY, bool mouseInGrid, Element& selected)
{
    if (!mouseInGrid || !IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    {
        return;
    }

    const Element picked = world.peek(cellX, cellY).material;
    if (picked != Element::Empty)
    {
        selected = picked;
    }
}

void drawBrushRing(int cellX, int cellY, int brushRadius, bool mouseInGrid)
{
    if (!mouseInGrid)
    {
        return;
    }

    // helps aiming with larger brush
    DrawCircleLines(cellX * CellSize + CellSize / 2,
                    cellY * CellSize + CellSize / 2,
                    static_cast<float>(brushRadius * CellSize),
                    WHITE);
}

} // namespace sand
