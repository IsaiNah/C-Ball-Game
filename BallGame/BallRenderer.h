#pragma once


#include <Bengine/SpriteBatch.h>
#include <Bengine/GLSLProgram.h>
#include <vector>
#include <memory>
#include "Ball.h"


class BallRenderer {
public:
	
   virtual void renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls
	   , const glm::mat4& projectionMatrix);
protected:
	std::unique_ptr<Bengine::GLSLProgram> m_program = nullptr;
};


class MomentumBallRender : public BallRenderer {

public:
	virtual void renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls
		, const glm::mat4& projectionMatrix) override;




};



class VelocityBallRender : public BallRenderer {

public:
	VelocityBallRender(int screenWidth, int screenHeight);
	virtual void renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls
		, const glm::mat4& projectionMatrix) override;


private:
	int m_screenWidth;
	int m_screenHeight;

};
//
//class BlinkerBallRender : public BallRenderer {
//
//public:
//	BlinkerBallRender(int screenWidth, int screenHeight);
//	virtual void renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls
//		, const glm::mat4& projectionMatrix) override;
//
//
//private:
//	int m_screenWidth;
//	int m_screenHeight;
//
//};