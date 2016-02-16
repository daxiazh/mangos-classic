/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOS_GRIDDEFINES_H
#define MANGOS_GRIDDEFINES_H

#include "Common.h"
#include "GameSystem/NGrid.h"
#include <cmath>

// Forward class definitions
class Corpse;
class Creature;
class DynamicObject;
class GameObject;
class Pet;
class Player;
class Camera;


/**************************************************************************/
/* 每个Map 由最多 MAX_NUMBER_OF_GRIDS x MAX_NUMBER_OF_GRIDS 个 Grid 组成   */
/* 每个Grid 由最多 MAX_NUMBER_OF_CELLS x MAX_NUMBER_OF_CELLS 个 Cell 组成  */
/**************************************************************************/

#define MAX_NUMBER_OF_GRIDS      64											// 一个 Map 中单个边的 Grid 的最大数量

#define SIZE_OF_GRIDS            533.33333f									// 一个 Grid 的世界尺寸(码)
#define CENTER_GRID_ID           (MAX_NUMBER_OF_GRIDS/2)					// 中间 Grid 的下标

#define CENTER_GRID_OFFSET      (SIZE_OF_GRIDS/2)							// 中间 Grid 的世界坐标(码)

#define MIN_GRID_DELAY          (MINUTE*IN_MILLISECONDS)
#define MIN_MAP_UPDATE_DELAY    50

#define MAX_NUMBER_OF_CELLS     16											// 一个 Grid 中单个边 Cell 的最大数量
#define SIZE_OF_GRID_CELL       (SIZE_OF_GRIDS/MAX_NUMBER_OF_CELLS)			// 一个 Cell 的世界尺寸(码)

#define CENTER_GRID_CELL_ID     (MAX_NUMBER_OF_CELLS*MAX_NUMBER_OF_GRIDS/2)	// 中间 Cell 的下标
#define CENTER_GRID_CELL_OFFSET (SIZE_OF_GRID_CELL/2)						// 中间 Cell 的世界坐标(码)

#define TOTAL_NUMBER_OF_CELLS_PER_MAP    (MAX_NUMBER_OF_GRIDS*MAX_NUMBER_OF_CELLS)	// 一个 Map 单个边的 Cell 的数量

#define MAP_RESOLUTION 128

#define MAP_SIZE                (SIZE_OF_GRIDS*MAX_NUMBER_OF_GRIDS)			// 一个 Map 的世界大小(码)
#define MAP_HALFSIZE            (MAP_SIZE/2)								// 半个 Map 的世界大小(码)

// Creature used instead pet to simplify *::Visit templates (not required duplicate code for Creature->Pet case)
// Cameras in world list just because linked with Player objects
typedef TYPELIST_4(Player, Creature/*pets*/, Corpse/*resurrectable*/, Camera)           AllWorldObjectTypes;
typedef TYPELIST_4(GameObject, Creature/*except pets*/, DynamicObject, Corpse/*Bones*/) AllGridObjectTypes;
typedef TYPELIST_4(Creature, Pet, GameObject, DynamicObject)                            AllMapStoredObjectTypes;

typedef GridRefManager<Camera>          CameraMapType;
typedef GridRefManager<Corpse>          CorpseMapType;
typedef GridRefManager<Creature>        CreatureMapType;
typedef GridRefManager<DynamicObject>   DynamicObjectMapType;
typedef GridRefManager<GameObject>      GameObjectMapType;
typedef GridRefManager<Player>          PlayerMapType;

typedef Grid<Player, AllWorldObjectTypes, AllGridObjectTypes> GridType;
typedef NGrid<MAX_NUMBER_OF_CELLS, Player, AllWorldObjectTypes, AllGridObjectTypes> NGridType;

typedef TypeMapContainer<AllGridObjectTypes> GridTypeMapContainer;
typedef TypeMapContainer<AllWorldObjectTypes> WorldTypeMapContainer;

// 描述:	为坐标对(x,y)提供一些通用操作的封装,如 Cell 坐标, Grid 坐标. 注: 0 < x < LIMIT, 0< y < LIMIT
template<const unsigned int LIMIT>
struct CoordPair
{
    CoordPair(uint32 x = 0, uint32 y = 0) : x_coord(x), y_coord(y) {}
    CoordPair(const CoordPair<LIMIT>& obj) : x_coord(obj.x_coord), y_coord(obj.y_coord) {}
    bool operator==(const CoordPair<LIMIT>& obj) const { return (obj.x_coord == x_coord && obj.y_coord == y_coord); }
    bool operator!=(const CoordPair<LIMIT>& obj) const { return !operator==(obj); }
    CoordPair<LIMIT>& operator=(const CoordPair<LIMIT>& obj)
    {
        x_coord = obj.x_coord;
        y_coord = obj.y_coord;
        return *this;
    }

    void operator<<(const uint32 val)
    {
        if (x_coord > val)
            x_coord -= val;
        else
            x_coord = 0;
    }

    void operator>>(const uint32 val)
    {
        if (x_coord + val < LIMIT)
            x_coord += val;
        else
            x_coord = LIMIT - 1;
    }

    void operator-=(const uint32 val)
    {
        if (y_coord > val)
            y_coord -= val;
        else
            y_coord = 0;
    }

    void operator+=(const uint32 val)
    {
        if (y_coord + val < LIMIT)
            y_coord += val;
        else
            y_coord = LIMIT - 1;
    }

    CoordPair& normalize()
    {
        x_coord = std::min(x_coord, LIMIT - 1);
        y_coord = std::min(y_coord, LIMIT - 1);
        return *this;
    }

    uint32 x_coord;			// x 坐标
    uint32 y_coord;			// y 坐标
};

typedef CoordPair<MAX_NUMBER_OF_GRIDS> GridPair;				// Grid 坐标对
typedef CoordPair<TOTAL_NUMBER_OF_CELLS_PER_MAP> CellPair;		// 整个Map的Cell坐标对.不是指一个Grid内的Cell坐标,它可以通过 struct Cell 来转换到一个Grid内的Cell的坐标

namespace MaNGOS
{
	// #Compute把给定的世界坐标变换成给定的二维数组的下标(Xn,Yn). 注: 坐标原点(0,0)对应二维数组的中间下标,如对于 64x64的二维数组-> (0, 0)对应 (32,32).
    template<class RET_TYPE, int CENTER_VAL>
    inline RET_TYPE Compute(float x, float y, float center_offset, float size)
    {
        // calculate and store temporary values in double format for having same result as same mySQL calculations
        double x_offset = (double(x) - center_offset) / size;
        double y_offset = (double(y) - center_offset) / size;

        int x_val = int(x_offset + CENTER_VAL + 0.5);
        int y_val = int(y_offset + CENTER_VAL + 0.5);
        return RET_TYPE(x_val, y_val);
    }

	// #ComputeGridPair把给定的世界坐标(x,y)变换成对应的Grid二维数组的下标GridPair
    inline GridPair ComputeGridPair(float x, float y)
    {
        return Compute<GridPair, CENTER_GRID_ID>(x, y, CENTER_GRID_OFFSET, SIZE_OF_GRIDS);
    }

	// #ComputeCellPair把给定的世界坐标(x,y)变换成对应的Cell二维数组的下标CellPair
    inline CellPair ComputeCellPair(float x, float y)
    {
        return Compute<CellPair, CENTER_GRID_CELL_ID>(x, y, CENTER_GRID_CELL_OFFSET, SIZE_OF_GRID_CELL);
    }

    inline void NormalizeMapCoord(float& c)
    {
        if (c > MAP_HALFSIZE - 0.5)
            c = MAP_HALFSIZE - 0.5;
        else if (c < -(MAP_HALFSIZE - 0.5))
            c = -(MAP_HALFSIZE - 0.5);
    }

    inline bool IsValidMapCoord(float c)
    {
        return std::isfinite(c) && (std::fabs(c) <= MAP_HALFSIZE - 0.5);
    }

    inline bool IsValidMapCoord(float x, float y)
    {
        return IsValidMapCoord(x) && IsValidMapCoord(y);
    }

    inline bool IsValidMapCoord(float x, float y, float z)
    {
        return IsValidMapCoord(x, y) && std::isfinite(z);
    }

    inline bool IsValidMapCoord(float x, float y, float z, float o)
    {
        return IsValidMapCoord(x, y, z) && std::isfinite(o);
    }
}
#endif
