﻿#pragma once

#include <iostream>
#include <cassert>
#include <DirectXMath.h>
#include "defs.hpp"
#include "MapConfig.hpp"
#include "utils.hpp"
#include "Tile.hpp"
#include "Direction.hpp"
#include "../graphics.h"
#include "../GameObject.h"

// TileMap Generator class
class TileMap {
public:
   Size const width, height;

   MapConfig const config;
	Vec<Tile> data;

	TileMap( MapConfig const & );

	// x = current X (may be mutated if successful)
	// y = current Y (may be mutated if successful)
	// d = direction to walk
	// returns true if successful
	Bool walk(U16 &tileX, U16 &tileY, Direction, Tile);

	// stream outputter friend declaration
	friend std::ostream& operator<< (std::ostream&, TileMap const&);

	// look-up table used with a cellular automata to convert the map data to 3D tiles.
	struct TileEntry { Str filename = "N/A";  I32 rotation = -1; };

	static Vec<TileEntry> const tileGraphicsTable;

	// returns the proper look-up index for the tile @ x,y in the graphics table
	inline Size getTileLookupIndex(U16 tileX, U16 tileY) const noexcept {
		return data[index(tileX, tileY)] == Tile::ground ? 0 : // else:
		    ( (neighbourIsRoad(Direction::north, tileX, tileY) ? cPow<Size>(2,0) : 0)
			 + (neighbourIsRoad(Direction::east,  tileX, tileY) ? cPow<Size>(2,1) : 0)
			 + (neighbourIsRoad(Direction::south, tileX, tileY) ? cPow<Size>(2,2) : 0)
			 + (neighbourIsRoad(Direction::west,  tileX, tileY) ? cPow<Size>(2,3) : 0) );
	}

	// Used with a cellular automata to beautify the terminal output.
	static Vec<Str> const tileTerminalGraphicsTable;

	// Used to color code tiles (for terminal output)
	static Vec<Str> const tileTerminalColorTable;

	static char constexpr terminalColorDefault[6] = "\033[0m"; // default colour resetter

	inline Size getTileColorLookupIndex(U16 tileX, U16 tileY) const noexcept {
		return static_cast<Size>(data[index(tileX, tileY)]);
	}

	Vec<GameObject> loadAsModels(Graphics&) const;

	// NOTE! pretends that all out-of-bounds tiles are roads
	Bool neighbourIsRoad(Direction, U16 tileX, U16 tileY) const noexcept;

	Bool isRoad(U16 tileX, U16 tileY) const noexcept;

	// convert an in-bounds 2D coordinate index (x,y) into an 1D index (i)
	inline Size index(U16 tileX, U16 tileY) const noexcept {
		return static_cast<Size>(tileY) * width + static_cast<Size>(tileX);
	}

	// returns true if the x,y coordinate is in-bounds
	inline Bool isInBounds(U16 tileX, U16 tileY) const noexcept {
		return tileX < width and tileY < height;
	}

	inline Vector3 convertTilePositionToWorldPosition(U16 const tileX, U16 const tileY) const {
		static auto const x_offset = width  / 2.0f * config.tileScaleFactor.x,
			               y_offset = height / 2.0f * config.tileScaleFactor.y;
		return { tileX * config.tileScaleFactor.x - x_offset, 
               .0f,
               tileY * -config.tileScaleFactor.y + y_offset };
	}

   inline Vector3 convertTilePositionToWorldPosition( V2u const &tilePosition ) const {
      return convertTilePositionToWorldPosition( tilePosition.x, tilePosition.y );
	}

   inline V2u convertWorldPositionToTilePosition( Vector3 const &worldPosition ) const {
      static auto const xOffset = width  / 2.0f * config.tileScaleFactor.x,
			               yOffset = height / 2.0f * config.tileScaleFactor.y;
      return { static_cast<U32>((worldPosition.x + xOffset) / config.tileScaleFactor.x), 
               static_cast<U32>((worldPosition.y + yOffset) / config.tileScaleFactor.y) };
   }

   Vec<V2u> getNeighbouringTilePositions( V2u cell_coord ) const noexcept;

   inline Tile const &tileAt( U16 x, U16 y) const noexcept {
      return data[index( x, y )];
   }

   inline Tile const &tileAt( V2u tilePosition ) const noexcept {
      return data[index( tilePosition.x, tilePosition.y )];
   }

   inline Tile &tileAt( U16 x, U16 y) noexcept {
      return data[index( x, y )];
   }

   inline Tile &tileAt( V2u tilePosition ) noexcept {
      return data[index( tilePosition.x, tilePosition.y )];
   }
};



// road generator stream outputter implementation
std::ostream& operator<< (std::ostream& out, TileMap const& map);
