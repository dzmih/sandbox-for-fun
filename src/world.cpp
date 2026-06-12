#include "world.h"

#include <algorithm>
#include <random>

namespace sand
{
namespace
{
std::mt19937 rng{std::random_device{}()};

bool oneIn(int chance)
{
    std::uniform_int_distribution<int> dist(1, chance);
    return dist(rng) == 1;
}

int randomDir()
{
    return std::uniform_int_distribution<int>(0, 1)(rng) == 0 ? -1 : 1;
}

Cell makeCell(Element material)
{
    Cell cell;
    cell.material = material;

    if (material == Element::Fire)
    {
        cell.life = std::uniform_int_distribution<int>(18, 42)(rng);
    }
    else if (material == Element::Acid)
    {
        cell.life = std::uniform_int_distribution<int>(170, 260)(rng);
    }

    return cell;
}

bool canDisplace(Element mover, Element target)
{
    if (target == Element::Empty)
    {
        return true;
    }

    if (mover == Element::Sand)
    {
        return target == Element::Water || target == Element::Acid;
    }

    if (mover == Element::Acid)
    {
        return target == Element::Water;
    }

    return false;
}

bool canCorrode(Element material)
{
    return material == Element::Sand || material == Element::Wall || material == Element::Wood;
}

} // namespace

World::World()
    : cells_(GridWidth * GridHeight), updated_(GridWidth * GridHeight, 0)
{
}

void World::clear()
{
    std::fill(cells_.begin(), cells_.end(), Cell{});
    std::fill(updated_.begin(), updated_.end(), 0);
    frame_ = 1;
}

void World::paint(int centerX, int centerY, int radius, Element material)
{
    const int radiusSquared = radius * radius;

    for (int y = centerY - radius; y <= centerY + radius; ++y)
    {
        for (int x = centerX - radius; x <= centerX + radius; ++x)
        {
            if (!inBounds(x, y))
            {
                continue;
            }

            const int dx = x - centerX;
            const int dy = y - centerY;
            if (dx * dx + dy * dy <= radiusSquared)
            {
                cells_[indexOf(x, y)] = makeCell(material);
            }
        }
    }
}

void World::update()
{
    ++frame_;
    if (frame_ == 0)
    {
        std::fill(updated_.begin(), updated_.end(), 0);
        frame_ = 1;
    }

    for (int y = GridHeight - 1; y >= 0; --y)
    {
        const bool leftToRight = std::uniform_int_distribution<int>(0, 1)(rng) == 0;
        if (leftToRight)
        {
            for (int x = 0; x < GridWidth; ++x)
            {
                updateCell(x, y);
            }
        }
        else
        {
            for (int x = GridWidth - 1; x >= 0; --x)
            {
                updateCell(x, y);
            }
        }
    }
}

void World::draw() const
{
    for (int y = 0; y < GridHeight; ++y)
    {
        for (int x = 0; x < GridWidth; ++x)
        {
            const Cell& cell = cells_[indexOf(x, y)];
            const Color base = infoFor(cell.material).color;
            DrawRectangle(x * CellSize, y * CellSize, CellSize, CellSize, variedColor(cell, base));
        }
    }
}

const Cell& World::peek(int x, int y) const
{
    return at(x, y);
}

Color World::variedColor(const Cell& cell, Color base)
{
    if (cell.material == Element::Empty)
    {
        return base;
    }

    const int seed = cell.life * 13 + static_cast<int>(cell.material) * 37;
    const int delta = (seed % 19) - 9;

    base.r = static_cast<unsigned char>(std::clamp(static_cast<int>(base.r) + delta, 0, 255));
    base.g = static_cast<unsigned char>(std::clamp(static_cast<int>(base.g) + delta, 0, 255));
    base.b = static_cast<unsigned char>(std::clamp(static_cast<int>(base.b) + delta, 0, 255));
    return base;
}

Cell& World::at(int x, int y)
{
    return cells_[indexOf(x, y)];
}

const Cell& World::at(int x, int y) const
{
    return cells_[indexOf(x, y)];
}

bool World::wasUpdated(int x, int y) const
{
    return updated_[indexOf(x, y)] == frame_;
}

void World::markUpdated(int x, int y)
{
    updated_[indexOf(x, y)] = frame_;
}

bool World::moveOrSwap(int fromX, int fromY, int toX, int toY)
{
    if (!inBounds(toX, toY))
    {
        return false;
    }

    Cell& from = at(fromX, fromY);
    Cell& to = at(toX, toY);
    if (!canDisplace(from.material, to.material))
    {
        return false;
    }

    std::swap(from, to);
    markUpdated(toX, toY);
    markUpdated(fromX, fromY);
    return true;
}

void World::updateCell(int x, int y)
{
    if (wasUpdated(x, y))
    {
        return;
    }

    switch (at(x, y).material)
    {
    case Element::Sand:
        updateSand(x, y);
        break;
    case Element::Water:
        updateWater(x, y);
        break;
    case Element::Acid:
        updateAcid(x, y);
        break;
    case Element::Fire:
        updateFire(x, y);
        break;
    default:
        break;
    }
}

void World::updateSand(int x, int y)
{
    const int dir = randomDir();
    if (moveOrSwap(x, y, x, y + 1) ||
        moveOrSwap(x, y, x + dir, y + 1) ||
        moveOrSwap(x, y, x - dir, y + 1))
    {
        return;
    }

    markUpdated(x, y);
}

void World::updateWater(int x, int y)
{
    const int dir = randomDir();
    if (moveOrSwap(x, y, x, y + 1) ||
        moveOrSwap(x, y, x + dir, y + 1) ||
        moveOrSwap(x, y, x - dir, y + 1) ||
        moveOrSwap(x, y, x + dir, y) ||
        moveOrSwap(x, y, x - dir, y))
    {
        return;
    }

    markUpdated(x, y);
}

void World::updateAcid(int x, int y)
{
    corrodeNeighbors(x, y);

    if (!inBounds(x, y) || at(x, y).material != Element::Acid)
    {
        return;
    }

    --at(x, y).life;
    if (at(x, y).life <= 0)
    {
        at(x, y) = Cell{};
        markUpdated(x, y);
        return;
    }

    const int dir = randomDir();
    if (moveOrSwap(x, y, x, y + 1) ||
        moveOrSwap(x, y, x + dir, y + 1) ||
        moveOrSwap(x, y, x - dir, y + 1) ||
        moveOrSwap(x, y, x + dir, y) ||
        moveOrSwap(x, y, x - dir, y))
    {
        return;
    }

    markUpdated(x, y);
}

void World::updateFire(int x, int y)
{
    Cell& fire = at(x, y);
    --fire.life;

    forEachNeighbor(x, y, [this](int nx, int ny) {
        const Element material = at(nx, ny).material;
        if (material == Element::Water || material == Element::Acid)
        {
            at(nx, ny) = Cell{};
            return;
        }

        if (material == Element::Wood && oneIn(5))
        {
            at(nx, ny) = makeCell(Element::Fire);
            markUpdated(nx, ny);
        }
    });

    if (fire.life <= 0 || oneIn(36))
    {
        at(x, y) = Cell{};
        markUpdated(x, y);
        return;
    }

    if (inBounds(x, y - 1) && at(x, y - 1).material == Element::Empty && oneIn(3))
    {
        std::swap(at(x, y), at(x, y - 1));
        markUpdated(x, y - 1);
        markUpdated(x, y);
        return;
    }

    markUpdated(x, y);
}

void World::corrodeNeighbors(int x, int y)
{
    bool consumed = false;

    forEachNeighbor(x, y, [this, &consumed](int nx, int ny) {
        if (canCorrode(at(nx, ny).material) && oneIn(9))
        {
            at(nx, ny) = Cell{};
            consumed = true;
        }
    });

    if (consumed && oneIn(3))
    {
        at(x, y) = Cell{};
        markUpdated(x, y);
    }
}

} // namespace sand
