﻿#include "TileMap.hpp"

TileMap::TileMap( MapConfig const &config ):
   config ( config ),
   width  ( static_cast<Size>(config.dimensions.y) ),
   height ( static_cast<Size>(config.dimensions.x) ),
	data   ( Vec<Tile>( width * height, Tile::ground ) )
{}

// x = current X (may be mutated if successful)
// y = current Y (may be mutated if successful)
// d = direction to walk
// returns true if successful
bool TileMap::walk(U16& x, U16& y, Direction d, Tile tile) {
	U16  _x = x, _y = y;
	switch (d) {
		case Direction::north: --_y; break;
		case Direction::east:  ++_x; break;
		case Direction::south: ++_y; break;
		case Direction::west:  --_x; break;
	}

	if (_x >= width or _y >= height)
		return false;

	auto idx = index(_x, _y);
	if (data[idx] != Tile::ground)
		return false;
	else {
		data[idx] = tile;
		x = _x;
		y = _y;
		return true;
	}
}

Vec<GameObject> TileMap::loadAsModels(Graphics& graphics) const {
	auto const to_reserve = width * height;
	Vec<GameObject> tiles( to_reserve );
	for (U16 y = 0; y < height; ++y) {
		for (U16 x = 0; x < width; ++x) {
			auto const  tile_i = index(x, y);
			auto       &tile   = tiles[tile_i];
			auto const  gfx_i  = getTileLookupIndex(x, y);
			auto const &[modelName, rotation] = tileGraphicsTable[gfx_i];
			tile.mesh    = graphics.getMeshPointer( modelName.c_str() );
         tile.setTexture( graphics.getTexturePointer(modelName.c_str(),true) );
			if ( rotation != 0 )
				tile.setRotation({ 0.0f, float(rotation) * 3.1415926535f/180.0f, 0.0f });
			tile.setPosition( convertTilePositionToWorldPosition(x,y) );
		}
	}
	return tiles; // RVO/Copy Elision
}

Bool TileMap::neighbourIsRoad(Direction dir, U16 x, U16 y) const noexcept {
	assert( dir == Direction::north or dir == Direction::east
	     or dir == Direction::south or dir == Direction::west );

	if      ( dir == Direction::north)      y--;
	else if ( dir == Direction::east)       x++;
	else if ( dir == Direction::south)      y++;
	else /* ( dir == Dir::west  )  */ x--;

	return !isInBounds(x, y) or (data[index(x, y)] != Tile::ground);
}

Bool TileMap::isRoad(U16 x, U16 y) const noexcept {
	return data[index(x, y)] != Tile::ground; // TODO: revamp if new tiles are addedd!
}

// road generator stream outputter implementation
std::ostream& operator<< (std::ostream& out, TileMap const& map) {
#ifdef _DEBUG
	out << "  PRINTING " << map.width << 'x' << map.height << " MAP:\n\t";
#endif
	for (U16 y = 0; y < map.height; ++y) {
		for (U16 x = 0; x < map.width; ++x) {
			//*[@DEPRECATED]*/  out << ' ' << map.data[map.index(x,y)];
			
#ifdef NO_TERMINAL_COLORS
         out << map.tileTerminalColorTable[    map.getTileColorLookupIndex( x, y )]
             << map.tileTerminalGraphicsTable[ map.getTileLookupIndex(      x, y )]
             << map.terminalColorDefault;
#else
			out << map.tileTerminalGraphicsTable[ map.getTileLookupIndex( x, y ) ];
#endif
		}

		out << "\n\t";
	}
	return out;
}

Vec<V2u> TileMap::getNeighbouringTilePositions( V2u cellPosition ) const noexcept {
   Vec<V2u>  neighbouringTilePositions {};
   neighbouringTilePositions.reserve(8);

   // check if any of the neighbouring tile coords are tiles within map bounds:
   Bool const  isBorderingWest  = ( cellPosition.x ==          0 ),
               isBorderingEast  = ( cellPosition.x == width  - 1 ),
               isBorderingNorth = ( cellPosition.y ==          0 ),
               isBorderingSouth = ( cellPosition.y == height - 1 );
   if ( not isBorderingNorth ) {  // N
      neighbouringTilePositions.emplace_back(    cellPosition.x,   cellPosition.y-1 );
      if ( not isBorderingWest ) // NW
         neighbouringTilePositions.emplace_back( cellPosition.x-1, cellPosition.y-1 );
      if ( not isBorderingEast ) // NE 
         neighbouringTilePositions.emplace_back( cellPosition.x+1, cellPosition.y-1 );
   }
   if ( not isBorderingSouth ) {  // S
      neighbouringTilePositions.emplace_back(    cellPosition.x,   cellPosition.y+1 );
      if ( not isBorderingWest ) // SW
         neighbouringTilePositions.emplace_back( cellPosition.x-1, cellPosition.y+1 );
      if ( not isBorderingEast ) // SE 
         neighbouringTilePositions.emplace_back( cellPosition.x+1, cellPosition.y+1 );
   }
   if ( not isBorderingWest )    // W
      neighbouringTilePositions.emplace_back(    cellPosition.x-1, cellPosition.y );
   if ( not isBorderingEast )    // E
      neighbouringTilePositions.emplace_back(    cellPosition.x+1, cellPosition.y );
   return neighbouringTilePositions;
}

// TODO: rotate bend mesh 180 degrees and update values in table to their proper value
Vec<TileMap::TileEntry> const TileMap::tileGraphicsTable = {
	// idx   WSEN      filename          rot      type                  rotation
	/*   0   0000 */  {"Roads/Road_pavement",        0}, // no road,                 0 deg
	/*   1   0001 */  {"Roads/Road_deadend",         0}, // deadend (south),         0 deg
	/*   2   0010 */  {"Roads/Road_deadend",        90}, // deadend (west),         90 deg
	/*   3   0011 */  {"Roads/Road_bend",          180}, // turn,                    0 deg
	/*   4   0100 */  {"Roads/Road_deadend",       180}, // deadend (north),       180 deg
	/*   5   0101 */  {"Roads/Road_straight",        0}, // straight vertical,       0 deg
	/*   6   0110 */  {"Roads/Road_bend",          270}, // turn,                   90 deg
	/*   7   0111 */  {"Roads/Road_3way",           90}, // 3-way intersection,     90 deg
	/*   8   1000 */  {"Roads/Road_deadend",       270}, // deadend (east),        270 deg
	/*   9   1001 */  {"Roads/Road_bend",           90}, // turn,                  270 deg
	/*  10   1010 */  {"Roads/Road_straight",       90}, // straight horizontal,    90 deg
	/*  11   1011 */  {"Roads/Road_3way",            0}, // 3-way intersection,      0 deg
	/*  12   1100 */  {"Roads/Road_bend",            0}, // turn,                  180 deg
	/*  13   1101 */  {"Roads/Road_3way",          270}, // 3-way intersection,    270 deg
	/*  14   1110 */  {"Roads/Road_3way",          180}, // 3-way intersection,    180 deg
	/*  15   1111 */  {"Roads/Road_4way",            0}, // 4-way intersection,      0 deg
};

// Used with a cellular automata to beautify the terminal output.
Vec<Str> const TileMap::tileTerminalGraphicsTable = {
	// idx   WSEN     getTerminalColorLookupIndex      type                  rotation
	/*   0   0000 */  u8".", // no road,                 0 deg
	/*   1   0001 */  u8"╹", // deadend (south),         0 deg
	/*   2   0010 */  u8"╺", // deadend (west),         90 deg
	/*   3   0011 */  u8"╚", // turn,                    0 deg
	/*   4   0100 */  u8"╻", // deadend (north),       180 deg
	/*   5   0101 */  u8"║", // straight vertical,       0 deg
	/*   6   0110 */  u8"╔", // turn,                   90 deg
	/*   7   0111 */  u8"╠", // 3-way intersection,     90 deg
	/*   8   1000 */  u8"╸", // deadend (east),        270 deg
	/*   9   1001 */  u8"╝", // turn,                  270 deg
	/*  10   1010 */  u8"═", // straight horizontal,    90 deg
	/*  11   1011 */  u8"╩", // 3-way intersection,      0 deg
	/*  12   1100 */  u8"╗", // turn,                  180 deg
	/*  13   1101 */  u8"╣", // 3-way intersection,    270 deg
	/*  14   1110 */  u8"╦", // 3-way intersection,    180 deg
	/*  15   1111 */  u8"╬"  // 4-way intersection,      0 deg
};

// Used to color code tiles (for terminal output)
Vec<Str> const TileMap::tileTerminalColorTable = {
	/* ground */    "\033[38;5;150m",
	/* road0  */    "\033[38;5;255m",
	/* road1  */    "\033[38;5;249m",
	/* road2  */    "\033[38;5;246m",
	/* road3  */    "\033[38;5;243m"
};
