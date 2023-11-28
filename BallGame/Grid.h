#pragma once
#include "Ball.h"
#include <vector>

struct Cell  // cause all public, contains list of balls in cell
{
	std::vector<Ball*> balls; //used pointer because ball is large in terms of bytes
							  // pointers typically 8 bytes(less space)

};


class Grid
{
	friend class BallController;
public:
	Grid(int width, int height, int cellSize);
	~Grid();
	//adds balls and determines which cell it belongs to
	void addBall(Ball* ball);
	void addBall(Ball* ball, Cell* cell);
	//Gets cell based on cell coords
	Cell* getCell(int x, int y);
	//Gets cell based on window coords
	Cell* getCell(const glm::vec2& pos); // overloading

	void removeBallFromCell(Ball* ball);

private:
	std::vector<Cell> m_cells;
	int m_width;
	int m_height;
	int m_cellSize;
	int m_numXCells;
	int m_numYCells;

};

