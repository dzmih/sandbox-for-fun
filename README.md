# falling sand game

маленкая игра песочница на c++ и raylib
тут песок вода огонь кислота и тд

## build

запускал так в powershell

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\falling_sand.exe
```

если не собралось иногда помагает еще раз прогнать cmake

## controls

- left mouse paint
- right mouse erase
- middle mouse pick material
- wheel brush size
- q e switch material
- space pause
- n one step in pause
- c clear
- 1 sand
- 2 water
- 3 wall
- 4 wood
- 5 fire
- 6 acid
