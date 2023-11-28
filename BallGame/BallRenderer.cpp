#include "BallRenderer.h"




void BallRenderer::renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls
	, const glm::mat4& projectionMatrix)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // setting background colour
	if (m_program == nullptr)//lazy init, only inits when needed
	{
		m_program = std::make_unique<Bengine::GLSLProgram>();//allocating
		m_program->compileShaders("Shaders/textureShading.frag", "Shaders/textureShading.vert");
		m_program->compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
		m_program->addAttribute("vertexPosition");
		m_program->addAttribute("vertexColor");
		m_program->addAttribute("vertexUV");
		m_program->linkShaders();

	}
	m_program->use();
	spriteBatch.begin();

	// Make sure the shader uses texture 0
	GLint textureUniform = m_program->getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	GLint pUniform = m_program->getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	//render all the balls
	for (auto& ball : balls)
	{
		const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
			ball.radius * 2.0f, ball.radius * 2.0f);
		spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, ball.color);

	}

	spriteBatch.end();
	spriteBatch.renderBatch();

	m_program->unuse();
}

void MomentumBallRender::renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls
	, const glm::mat4& projectionMatrix)
{

	glClearColor(0.0f, 0.1f, 0.5f, 1.0f); // setting background colour
	if (m_program == nullptr)//lazy init, only inits when needed
	{
		m_program = std::make_unique<Bengine::GLSLProgram>();//allocating
		m_program->compileShaders("Shaders/textureShading.frag", "Shaders/textureShading.vert");
		m_program->compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
		m_program->addAttribute("vertexPosition");
		m_program->addAttribute("vertexColor");
		m_program->addAttribute("vertexUV");
		m_program->linkShaders();

	}
	m_program->use();
	spriteBatch.begin();

	// Make sure the shader uses texture 0
	GLint textureUniform = m_program->getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	GLint pUniform = m_program->getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	//render all the balls
	for (auto& ball : balls)
	{
		const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
			ball.radius * 2.0f, ball.radius * 2.0f);
		Bengine::ColorRGBA8 color;
		GLubyte colorVal =(GLubyte) (glm::clamp(glm::length(ball.velocity) * ball.mass * 12.0f, 0.0f, 255.0f)); // momentum
		color.r = colorVal;
		color.g = colorVal;
		color.b = colorVal;
		color.a = colorVal;
		spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, color);

	}

	spriteBatch.end();
	spriteBatch.renderBatch();

	m_program->unuse();
}

VelocityBallRender::VelocityBallRender(int screenWidth, int screenHeight)
	: m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{



}

void VelocityBallRender::renderBalls(Bengine::SpriteBatch& spriteBatch, const std::vector<Ball>& balls, const glm::mat4& projectionMatrix)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // setting background colour
	if (m_program == nullptr)//lazy init, only inits when needed
	{
		m_program = std::make_unique<Bengine::GLSLProgram>();//allocating
		m_program->compileShaders("Shaders/textureShading.frag", "Shaders/textureShading.vert");
		m_program->compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
		m_program->addAttribute("vertexPosition");
		m_program->addAttribute("vertexColor");
		m_program->addAttribute("vertexUV");
		m_program->linkShaders();

	}
	m_program->use();
	spriteBatch.begin();

	// Make sure the shader uses texture 0
	GLint textureUniform = m_program->getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	GLint pUniform = m_program->getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	//render all the balls
	for (auto& ball : balls)
	{
		const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		const glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
			ball.radius * 2.0f, ball.radius * 2.0f);
		Bengine::ColorRGBA8 color;

		float mult = 100.0f;
		GLubyte colorVal = (GLubyte)(glm::clamp(ball.velocity.x  * mult, 0.0f, 255.0f)); // momentum
		color.r = 120;
		color.g = (GLubyte)((ball.position.x /  m_screenWidth) * 255.0f);
		color.b = (GLubyte)((ball.position.y / m_screenHeight) * 255.0f);
		color.a = colorVal;
		spriteBatch.draw(destRect, uvRect, ball.textureId, 0.0f, color);

	}

	spriteBatch.end();
	spriteBatch.renderBatch();

	m_program->unuse();

}
