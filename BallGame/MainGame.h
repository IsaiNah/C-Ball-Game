#pragma once
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/InputManager.h>
#include <Bengine/Window.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/Timing.h>
#include <Bengine/SpriteFont.h>
#include <memory> // for smart pointers(unique)

#include "BallController.h"
#include "BallRenderer.h"
#include "Grid.h"

enum class GameState { RUNNING, EXIT };

const int CELL_SIZE = 16;// depends on how big yo balls are

class MainGame {
public:
	~MainGame();
    void run();

private:
    void init();
	void initRenderers();
    void initBalls();
    void update(float deltaTime);
    void draw();
    void drawHud();
    void processInput();

    int m_screenWidth = 0;
    int m_screenHeight = 0;

    std::vector<Ball> m_balls; ///< All the balls, assuming not change to no pointer, better performance as no pointer = continunes mem improving performance.
	std::unique_ptr<Grid> m_grid; // grid for spatial partitioning for collision

	int m_currentRender = 0;
	std::vector<BallRenderer*> m_ballRenderers; // all renderers

    BallController m_ballController; ///< Controls balls
   // BallRenderer m_ballRenderer; ///< Renders balls moved to m_ballrenderes

    Bengine::Window m_window; ///< The main window
    Bengine::SpriteBatch m_spriteBatch; ///< Renders all the balls
    std::unique_ptr<Bengine::SpriteFont> m_spriteFont; ///< For font rendering
    Bengine::Camera2D m_camera; ///< Renders the scene
    Bengine::InputManager m_inputManager; ///< Handles input
    Bengine::GLSLProgram m_textureProgram; ///< Shader for textures]

    Bengine::fpsLimiter m_fpsLimiter; ///< Limits and calculates fps
    float m_fps = 0.0f;

    GameState m_gameState = GameState::RUNNING; ///< The state of the game
};

