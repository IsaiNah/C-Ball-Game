#include "BallController.h"
#include "Grid.h"
#include <iostream>

void BallController::updateBalls(std::vector <Ball>& balls, Grid* grid, float deltaTime, int maxX, int maxY) {
    const float FRICTION = 0.050f;
    // Update our grabbed balls velocity
    if (m_grabbedBall != -1) {
        balls[m_grabbedBall].velocity = balls[m_grabbedBall].position - m_prevPos;
    }

    glm::vec2 gravity = getGravityAccel();

    for (size_t i = 0; i < balls.size(); i++) {
        // get handle for less typing
        Ball& ball = balls[i];
        // Update motion if its not grabbed
        if (i != m_grabbedBall) {
            ball.position += ball.velocity * deltaTime;
            // Apply friction
            glm::vec2 momentumVec = ball.velocity * ball.mass;
            if (momentumVec.x != 0 || momentumVec.y != 0) {
                if (FRICTION < glm::length(momentumVec)) {
                    ball.velocity -= deltaTime * FRICTION * glm::normalize(momentumVec) / ball.mass;
                } else {
                    ball.velocity = glm::vec2(0.0f);
                }
            }
            // Apply gravity
            ball.velocity += gravity * deltaTime;
        }
        // Check wall collision
        if (ball.position.x < ball.radius) {
			ball.position.x = ball.radius;
            if (ball.velocity.x < 0) ball.velocity.x *= -1;
        } else if (ball.position.x + ball.radius >= maxX) {
            ball.position.x = maxX - ball.radius - 1;
            if (ball.velocity.x > 0) ball.velocity.x *= -1;
        }
		if (ball.position.y < ball.radius) {
			ball.position.y = ball.radius;
            if (ball.velocity.y < 0) ball.velocity.y *= -1;
        } else if (ball.position.y + ball.radius >= maxY) {
            ball.position.y = maxY - ball.radius - 1;
            if (ball.velocity.y > 0) ball.velocity.y *= -1;
        }
		//std::cout << " passed removeBallFromCell" << std::endl;
		//check to see if ball moved
		Cell* newCell = grid->getCell(ball.position);
		if (newCell != ball.ownerCell)
		{
			//std::cout << ball.ownerCell->balls.size() << std::endl;
			fflush(stdout);
			grid->removeBallFromCell(&balls[i]);
			grid->addBall(&balls[i], newCell);
		}
    }
	//// Check collisions -new with spacial partioning
	updateCollision(grid);
	//// Check collisions -old
	//for (size_t j = i + 1; j < balls.size(); j++) {
	//	checkCollision(ball, balls[j]);
	//}
    // Update our grabbed ball
    if (m_grabbedBall != -1) {
        // Update the velocity again, in case it got changed by collision
        balls[m_grabbedBall].velocity = balls[m_grabbedBall].position - m_prevPos;
        m_prevPos = balls[m_grabbedBall].position;
    }
}

void BallController::onMouseDown(std::vector <Ball>& balls, float mouseX, float mouseY) {
    for (size_t i = 0; i < balls.size(); i++) {
        // Check if the mouse is hovering over a ball
        if (isMouseOnBall(balls[i], mouseX, mouseY)) {
            m_grabbedBall = i; // BE AWARE, if you change the order of the balls in the vector, this becomes invalid!
            m_grabOffset = glm::vec2(mouseX, mouseY) - balls[i].position;
            m_prevPos = balls[i].position;
            balls[i].velocity = glm::vec2(0.0f);
        }
    }
}

void BallController::onMouseUp(std::vector <Ball>& balls) {
    if (m_grabbedBall != -1) {
        // Throw the ball!
        balls[m_grabbedBall].velocity = balls[m_grabbedBall].position - m_prevPos;
        m_grabbedBall = -1;
    }
}

void BallController::onMouseMove(std::vector <Ball>& balls, float mouseX, float mouseY) {
    if (m_grabbedBall != -1) {
        balls[m_grabbedBall].position = glm::vec2(mouseX, mouseY) - m_grabOffset;
    }
}

void BallController::checkCollision(Ball& b1, Ball& b2) {
    // We add radius since position is the top left corner
    glm::vec2 distVec = (b2.position + b2.radius) -
                        (b1.position + b1.radius);// left as is because b2.position - b1.position + b1.radius; was not checking the top
    glm::vec2 distDir = glm::normalize(distVec);
    float dist = glm::length(distVec);
    float totalRadius = b1.radius + b2.radius;
    
    float collisionDepth = totalRadius - dist;
    // Check for collision
    if (collisionDepth > 0) {

        //// Push away the less massive one
        //if (b1.mass < b2.mass) {
        //    b1.position -= distDir * collisionDepth;
        //} else {
        //    b2.position += distDir * collisionDepth;
        //} changed to -> ** previous cause massive fps drops after a certian point**
		//pushes away balls based on raio of masses(prevent isolating effect of small ball getting caught back and froth result in fps drop)
		b1.position -= distDir * collisionDepth * (b2.mass / b1.mass) * 0.5f;
		b2.position += distDir * collisionDepth * (b1.mass / b2.mass) * 0.5f;


        // Calculate deflection. http://stackoverflow.com/a/345863
		
        float aci = glm::dot(b1.velocity, distDir);
        float bci = glm::dot(b2.velocity, distDir);

        float acf = (aci * ( b1.mass - b2.mass) + 2 * b2.mass * bci) / (b1.mass + b2.mass);
		float bcf = (bci * ( b2.mass - b1.mass) + 2 * b1.mass * aci) / (b1.mass + b2.mass);


        b1.velocity += (acf - aci) * distDir;
		b2.velocity += (bcf - bci) * distDir;

		//added for cool effect, should move to seperate controller
		if (glm::length(b1.velocity + b2.velocity) > 0.5f)
		{
			//picks which ball is faster
			bool choice = glm::length(b1.velocity) < glm::length(b2.velocity);

			//faster ball gets its caller color to slower ball
			choice ? b2.color = b1.color : b1.color = b2.color;

		}

    }
}

void BallController::checkCollision(Ball* ball, std::vector<Ball*>& ballsToCheck, int startIndex)
{
	for (int i = startIndex; i < ballsToCheck.size(); i++)
	{
		checkCollision(*ball, *ballsToCheck[i]);

	}

}


void BallController::updateCollision(Grid* grid)
{
	for (int i = 0; i < grid->m_cells.size(); i++)
	{

		int x = i % grid->m_numXCells;
		int y = i / grid->m_numXCells;

		Cell& cell = grid->m_cells[i];

		// loop thru all the balls in a cell
		for (int j = 0; j < cell.balls.size(); j++)
		{
			Ball * ball = cell.balls[j];
			//update with the residing cell
			checkCollision(ball, cell.balls, j + 1);// +1 to check everything else but self
			//update with neigbour cell
			if (x > 0)// check left dir
			{	//left
				if (y > 0)
				{
					// top left
					checkCollision(ball, grid->getCell(x - 1, y - 1)->balls, 0);
				}
				// bottom left
				if (y < grid->m_numYCells - 1)
				{
					checkCollision(ball, grid->getCell(x - 1, y + 1)->balls, 0);
				}
			
			}
			// check up
			if (y > 0)
			{
				checkCollision(ball, grid->getCell(x, y - 1)->balls, 0);
			}
		}
	}


}

bool BallController::isMouseOnBall(Ball&b, float mouseX, float mouseY) {
    return (mouseX >= b.position.x - b.radius && mouseX < b.position.x + b.radius &&
            mouseY >= b.position.y - b.radius && mouseY < b.position.y + b.radius);
}

glm::vec2 BallController::getGravityAccel() {
    const float GRAVITY_FORCE = 0.1f;
    glm::vec2 gravity;

    switch (m_gravityDirection) {
        case GravityDirection::DOWN:
            gravity = glm::vec2(0.0f, -GRAVITY_FORCE);
            break;
        case GravityDirection::LEFT:
            gravity = glm::vec2(-GRAVITY_FORCE, 0.0f);
            break;
        case GravityDirection::RIGHT:
            gravity = glm::vec2(GRAVITY_FORCE, 0.0f);
            break;
        case GravityDirection::UP:
            gravity = glm::vec2(0.0f, GRAVITY_FORCE);
            break;
        default:
            gravity = glm::vec2(0.0f);
            break;
    }
    return gravity;
}