#pragma once

#include "sand_shared.h"

#include <cstdint>
#include <vector>

namespace sand
{

class World
{
public:
    World();

    void clear();
    void paint(int centerX, int centerY, int radius, Element material);
    void update();
    void draw() const;
    const Cell& peek(int x, int y) const;

private:
    static Color variedColor(const Cell& cell, Color base);

    Cell& at(int x, int y);
    const Cell& at(int x, int y) const;

    bool wasUpdated(int x, int y) const;
    void markUpdated(int x, int y);
    bool moveOrSwap(int fromX, int fromY, int toX, int toY);

    void updateCell(int x, int y);
    void updateSand(int x, int y);
    void updateWater(int x, int y);
    void updateAcid(int x, int y);
    void updateFire(int x, int y);
    void corrodeNeighbors(int x, int y);

    template <typename Callback>
    void forEachNeighbor(int x, int y, Callback callback)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                if (dx == 0 && dy == 0)
                {
                    continue;
                }

                const int nx = x + dx;
                const int ny = y + dy;
                if (inBounds(nx, ny))
                {
                    callback(nx, ny);
                }
            }
        }
    }

    std::vector<Cell> cells_;
    std::vector<std::uint32_t> updated_;
    std::uint32_t frame_ = 1;
};

} // namespace sand
