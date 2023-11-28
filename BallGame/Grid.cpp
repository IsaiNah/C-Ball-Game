#include "Grid.h"


Grid::Grid(int width, int height, int cellSize) :
m_width(width),
m_height(height),
m_cellSize(cellSize)
{
	m_numXCells = ceil((float)m_width / m_cellSize);
	m_numYCells = ceil((float)m_height / m_cellSize);

	//Allocate all the cells
	const int BALLS_TO_RESERVE = 20; //to prevent resize internal mem 
	m_cells.resize(m_numYCells * m_numXCells);
	for (int i = 0; i < m_cells.size(); i++)
	{
		m_cells[i].balls.reserve(BALLS_TO_RESERVE);
	}
}



Grid::~Grid()
{
}

void Grid::addBall(Ball* ball)
{
	Cell* cell = getCell(ball->position);
	cell->balls.push_back(ball);
	ball->ownerCell = cell;
	ball->cellVecIndex = cell->balls.size() - 1;
}


void Grid::addBall(Ball* ball, Cell* cell)
{
	cell->balls.push_back(ball);
	ball->ownerCell = cell;
	ball->cellVecIndex = cell->balls.size() - 1;



}


Cell* Grid::getCell(int x, int y)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= m_numXCells)
	{ 
		x = m_numXCells - 1;
	}

	if (y >= m_numYCells)
	{
		y = m_numYCells - 1;
	}

	return &m_cells[y * m_numXCells + x]; // treat 2d array as 1d

}

Cell* Grid::getCell(const glm::vec2& pos)
{
	
	int cellX =(int)( pos.x / m_cellSize);
	int cellY =(int)( pos.y / m_cellSize);


	return getCell(cellX, cellY);
}

void Grid::removeBallFromCell(Ball* ball)
{
	std::vector<Ball*>& balls = ball->ownerCell->balls;
	//normal vector swap
	ball->ownerCell->balls[ball->cellVecIndex] = ball->ownerCell->balls.back();
	balls.pop_back();

	//update vector index
	if (ball->cellVecIndex < balls.size()){
		balls[ball->cellVecIndex]->cellVecIndex = ball->cellVecIndex;
		//set the index of the ball to -1
	}

	ball->cellVecIndex = -1;
	ball->ownerCell = nullptr;


}
