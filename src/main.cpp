#include "ui.h"

int main()
{
    using namespace sand;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(ScreenWidth, ScreenHeight, "Falling Sand Game");
    SetTargetFPS(60);

    World world;
    Element selected = Element::Sand;
    int brushRadius = 4;
    bool paused = false;

    while (!WindowShouldClose())
    {
        handleSelectionKeys(selected);

        bool stepRequested = false;
        handleSimKeys(world, paused, stepRequested);
        updateBrushRadius(brushRadius);

        int cellX = 0;
        int cellY = 0;
        bool mouseInGrid = false;
        applyBrushFromMouse(world, brushRadius, selected, cellX, cellY, mouseInGrid);
        pickUnderCursor(world, cellX, cellY, mouseInGrid, selected);

        if (!paused || stepRequested)
        {
            world.update();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        world.draw();
        drawBrushRing(cellX, cellY, brushRadius, mouseInGrid);
        drawPanel(selected, brushRadius, paused);
        DrawFPS(GridWidth * CellSize + 18, ScreenHeight - 32);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

