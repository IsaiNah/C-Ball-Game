#define _CRT_SECURE_NO_WARNINGS // To shut up the compiler about sprintf...
#include "MainGame.h"

#include <Bengine/Bengine.h>
#include <Bengine/ResourceManager.h>
#include <SDL.h>
#include <random>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iostream>


//Notes
// texture info off in engine TextureCache GetTexture

// Some helpful constants.
const float DESIRED_FPS = 60.0f; // FPS the game is designed to run at
const int MAX_PHYSICS_STEPS = 6; // Max number of physics steps per frame
const float MS_PER_SECOND = 1000; // Number of milliseconds in a second
const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
const float MAX_DELTA_TIME = 1.0f; // Maximum size of deltaTime

MainGame::~MainGame()
{
	//required to deallocate uniqte pointer
	for (int i = 0; i < m_ballRenderers.size(); i++)
	{
		delete m_ballRenderers[i];

	}
}


void MainGame::run() {
    init();
    initBalls();

    // Start our previousTicks variable
    Uint32 previousTicks = SDL_GetTicks();

    // Game loop
    while (m_gameState == GameState::RUNNING) {
        m_fpsLimiter.begin();
        processInput();

        // Calculate the frameTime in milliseconds
        Uint32 newTicks = SDL_GetTicks();
        Uint32 frameTime = newTicks - previousTicks;
        previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
        // Get the total delta time
        float totalDeltaTime = (float)frameTime / DESIRED_FRAMETIME;

        int i = 0; // This counter makes sure we don't spiral to death!
        // Loop while we still have steps to process.
        while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
            // The deltaTime should be the the smaller of the totalDeltaTime and MAX_DELTA_TIME
            float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
            // Update all physics here and pass in deltaTime

            update(deltaTime);
          
            // Since we just took a step that is length deltaTime, subtract from totalDeltaTime
            totalDeltaTime -= deltaTime;
            // Increment our frame counter so we can limit steps to MAX_PHYSICS_STEPS
            i++;
        }

        m_camera.update();
        draw();
        m_fps = m_fpsLimiter.end();

	//std::cout << m_fps << std::endl; // not actual fps?

    }
}

void MainGame::init() {
    Bengine::init();

    m_screenWidth = 1596;
    m_screenHeight = 998;

    m_window.create("Ball Game", m_screenWidth, m_screenHeight, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    m_camera.init(m_screenWidth, m_screenHeight);
    // Point the camera to the center of the screen
    m_camera.setPosition(glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f));
    
    m_spriteBatch.init();
    // Initialize sprite font
    m_spriteFont = std::make_unique<Bengine::SpriteFont>("Fonts/chintzy.ttf", 40);

    // Compile our texture shader
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();

    m_fpsLimiter.setMaxFPS(60.0f);

	initRenderers();
    
}


void MainGame::initRenderers()
{
	m_ballRenderers.push_back(new BallRenderer);
	m_ballRenderers.push_back(new MomentumBallRender);
	m_ballRenderers.push_back(new VelocityBallRender(m_screenWidth, m_screenHeight));

}
struct BallSpawn { // defines spawn for ball
    BallSpawn(const Bengine::ColorRGBA8& colr,
              float rad, float m, float minSpeed,
              float maxSpeed, float prob) :
              color(colr),
              radius(rad),
              mass(m),
              randSpeed(minSpeed, maxSpeed),
              probability(prob) {
        // Empty
    }
    Bengine::ColorRGBA8 color;
    float radius;
    float mass;
    float probability;
    std::uniform_real_distribution<float> randSpeed;
};
#include <iostream>
void MainGame::initBalls() {

// Initialize the grid
	m_grid = std::make_unique<Grid>(m_screenWidth, m_screenHeight, CELL_SIZE); // making unique pointer
	
// Making macros kinda a inline function or an alias
#define ADD_BALL(p, ...) \
    totalProbability += p; \
    possibleBalls.emplace_back(__VA_ARGS__);

	//... means everything else, __VA_ARGS__ is everything


	//its like an alias for example
	//#define PRINTSTUFF std::cout << "hello Wolrd"; 
    //#define PRINTSTUFF(a) std::cout <<  a; 
	//will use printstuff instead of std::cout

    // Number of balls to spawn
    const int NUM_BALLS = 10000;

    // Random engine stuff
    std::mt19937 randomEngine((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> randX(0.0f, (float)m_screenWidth);
    std::uniform_real_distribution<float> randY(0.0f, (float)m_screenHeight);
    std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

    // Add all possible balls
    std::vector <BallSpawn> possibleBalls;
    float totalProbability = 0.0f;

	//created for random ball size and colour init
	std::uniform_real_distribution<float> r1(2.0f, 6.0f);
	std::uniform_int_distribution<int> r2(0, 255);

    // Adds the balls using a macro if num 0-20 first ball, 20-30 second ball, 30-31 3rd ball
    ADD_BALL(20.0f, Bengine::ColorRGBA8(255, 255, 255, 255),
             2.0f, 1.0f, 0.1f, 7.0f, totalProbability);//was 2
    ADD_BALL(10.0f, Bengine::ColorRGBA8(0, 0, 255, 255),// was 3
             3.0f, 2.0f, 0.1f, 3.0f, totalProbability);
	ADD_BALL(1.0f, Bengine::ColorRGBA8(255, 0, 0, 255),// was 5
		15.0f, 4.0f, 0.0f, 0.0f, totalProbability)
		// added later
		ADD_BALL(1.0f, Bengine::ColorRGBA8(0, 255, 0, 255),// was 5
		14.0f, 4.0f, 0.0f, 0.0f, totalProbability)
		ADD_BALL(1.0f, Bengine::ColorRGBA8(0, 255, 0, 255),// was 5
		16.5f, 4.0f, 0.0f, 0.0f, totalProbability)

		//copied for test create random colours and sizes
		for (int i = 0; i < 10000; i++)
		{
			ADD_BALL(1.0f, Bengine::ColorRGBA8(r2(randomEngine), r2(randomEngine), r2(randomEngine), 255),// was 5
				r1(randomEngine), r1(randomEngine), 0.0f, 0.0f, totalProbability)

		}


    // Random probability for ball spawn
    std::uniform_real_distribution<float> spawn(0.0f, totalProbability);

    // Small optimization that sets the size of the internal array to prevent
    // extra allocations.
    m_balls.reserve(NUM_BALLS);

    // Set up ball to spawn with default value
    BallSpawn* ballToSpawn = &possibleBalls[0];
    for (int i = 0; i < NUM_BALLS; i++) {
        // Get the ball spawn roll
        float spawnVal = spawn(randomEngine);
        // Figure out which ball we picked
        for (size_t j = 0; j < possibleBalls.size(); j++) {
            if (spawnVal <= possibleBalls[j].probability) {
                ballToSpawn = &possibleBalls[j];
                break;
            }
        }

        // Get random starting position
        glm::vec2 pos(randX(randomEngine), randY(randomEngine));

        // Hacky way to get a random direction
        glm::vec2 direction(randDir(randomEngine), randDir(randomEngine));
        if (direction.x != 0.0f || direction.y != 0.0f) { // The chances of direction == 0 are astronomically low
            direction = glm::normalize(direction);
        } else {
            direction = glm::vec2(1.0f, 0.0f); // default direction
        }

        // Add ball
        m_balls.emplace_back(ballToSpawn->radius, ballToSpawn->mass, pos, direction * ballToSpawn->randSpeed(randomEngine),
                             Bengine::ResourceManager::getTexture("Textures/circle.png").id,
                             ballToSpawn->color);
		//Add the ball to the grid. ! IF YOU EVER CALL EMPLACE BACK AFTER INIT BALLS, m_grid will have dangeling pointers !
		m_grid->addBall(&m_balls.back());
	}
}

void MainGame::update(float deltaTime) {
    m_ballController.updateBalls(m_balls, m_grid.get(),deltaTime, m_screenWidth, m_screenHeight);
}

void MainGame::draw() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glActiveTexture(GL_TEXTURE0);

  
    // Grab the camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
   

    m_ballRenderers[m_currentRender]->renderBalls(m_spriteBatch, m_balls, projectionMatrix);
    

	m_textureProgram.use();

	// Make sure the shader uses texture 0
	GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);


	 GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    drawHud();

    m_textureProgram.unuse();

    m_window.swapBuffer();
}

void MainGame::drawHud() {
    const Bengine::ColorRGBA8 fontColor(255, 0, 0, 255);
    // Convert float to char *
    char buffer[64];
    sprintf(buffer, "%.1f", m_fps);

    m_spriteBatch.begin();
    m_spriteFont->draw(m_spriteBatch, buffer, glm::vec2(0.0f, m_screenHeight - 32.0f),
                       glm::vec2(1.0f), 0.0f, fontColor);
    m_spriteBatch.end();
    m_spriteBatch.renderBatch();
}

void MainGame::processInput() {
	// Update input manager!!!
	m_inputManager.update();


	SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
            case SDL_QUIT:
                m_gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                m_ballController.onMouseMove(m_balls, (float)evnt.motion.x, (float)m_screenHeight - (float)evnt.motion.y);
                m_inputManager.setMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                m_inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                m_inputManager.releseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                m_ballController.onMouseDown(m_balls, (float)evnt.button.x, (float)m_screenHeight - (float)evnt.button.y);
                m_inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                m_ballController.onMouseUp(m_balls);
				m_inputManager.releseKey(evnt.button.button);
                break;
        }
    }

    if (m_inputManager.isKeyPressed(SDLK_ESCAPE)) {
        m_gameState = GameState::EXIT;
    }
    // Handle gravity changes
    if (m_inputManager.isKeyPressed(SDLK_LEFT)) {
        m_ballController.setGravityDirection(GravityDirection::LEFT);
    } else if (m_inputManager.isKeyPressed(SDLK_RIGHT)) {
        m_ballController.setGravityDirection(GravityDirection::RIGHT);
    } else if (m_inputManager.isKeyPressed(SDLK_UP)) {
        m_ballController.setGravityDirection(GravityDirection::UP);
    } else if (m_inputManager.isKeyPressed(SDLK_DOWN)) {
        m_ballController.setGravityDirection(GravityDirection::DOWN);
    } else if (m_inputManager.isKeyPressed(SDLK_SPACE)) {
        m_ballController.setGravityDirection(GravityDirection::NONE);
    }


	// switch renderers
	if (m_inputManager.isKeyPressed(SDLK_1)) {
		m_currentRender++;
		if ( m_currentRender >= m_ballRenderers.size())
		{
			m_currentRender = 0;
		}
	}

}