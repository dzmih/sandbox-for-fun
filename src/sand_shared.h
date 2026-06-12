#pragma once

#include "raylib.h"

#include <array>
#include <cstdint>

namespace sand
{
constexpr int GridWidth = 200;
constexpr int GridHeight = 200;
constexpr int CellSize = 4;
constexpr int PanelWidth = 230;
constexpr int ScreenWidth = GridWidth * CellSize + PanelWidth;
constexpr int ScreenHeight = GridHeight * CellSize;

enum class Element : std::uint8_t
{
    Empty,
    Sand,
    Water,
    Wall,
    Wood,
    Fire,
    Acid,
};

struct Cell
{
    Element material = Element::Empty;
    int life = 0;
};

struct ElementInfo
{
    Element material;
    const char* name;
    Color color;
};

constexpr std::array<ElementInfo, 7> Materials{{
    {Element::Empty, "Empty", {8, 10, 14, 255}},
    {Element::Sand, "Sand", {224, 187, 96, 255}},
    {Element::Water, "Water", {60, 137, 220, 210}},
    {Element::Wall, "Wall", {112, 116, 124, 255}},
    {Element::Wood, "Wood", {116, 72, 38, 255}},
    {Element::Fire, "Fire", {244, 91, 44, 255}},
    {Element::Acid, "Acid", {116, 232, 80, 230}},
}};

inline int indexOf(int x, int y)
{
    return y * GridWidth + x;
}

inline bool inBounds(int x, int y)
{
    return x >= 0 && x < GridWidth && y >= 0 && y < GridHeight;
}

inline const ElementInfo& infoFor(Element material)
{
    for (const auto& info : Materials)
    {
        if (info.material == material)
        {
            return info;
        }
    }
    return Materials[0];
}

inline int findMaterialSlot(Element material)
{
    for (int i = 1; i < static_cast<int>(Materials.size()); ++i)
    {
        if (Materials[static_cast<std::size_t>(i)].material == material)
        {
            return i;
        }
    }

    return 1;
}

} // namespace sand
