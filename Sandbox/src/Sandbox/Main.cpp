#include "Luna.h"
#include <imgui/imgui.h>

#include <random>


//Clrs
//{ 255.0f, 0.0f, 0.0f, 1.0f };				  //Red
//{ 65.0f, 65.0f, 0.0f, 1.0f };				  //Yellow
//{ 255.0f, 0.0f, 180.0f, 255.0f };           //LightPink
//{ 0.0f, 188.0f, 255.0f, 255.0f };           //GrayBlue
//glm::vec4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
//glm::vec4 White = { 1.0f, 1.0f, 1.0f, 1.0f };

static float startColor[4] = { 255.0f, 0.0f, 180.0f, 255.0f };
static float endColor[4] = { 0.0f, 188.0f, 255.0f, 255.0f };

static float LifeTime = 1.0f;
static float ColorSpeed = 0.5f;
static float Transparency = 1.0f;
static int ParticleCounter = 0;
static float Size = 35;
static float force = 3.5f;

void Luna::Application::BuildUI()
{
	ImGui::Begin("ParticleStats");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Amount of particles in scene %i", ParticleCounter);
		ImGui::SliderFloat("Lifetime (seconds)", &LifeTime, 0, 10);

		ImGui::SliderFloat("Force ", &force, 0, 15);
		ImGui::SliderFloat("ColorSpeedTime (seconds)", &ColorSpeed, 0, 10);
		ImGui::SliderFloat("Size", &Size, 0, 50);
		ImGui::ColorEdit4("StartColor", startColor);
		ImGui::ColorEdit4("EndColor", endColor);

	ImGui::End();
}

class ParticleSystem
{
private:
	struct Particle
	{
		Luna::Anchor pos;

		float LifeTime = 2.0f;
		float ColorChangeSpeed = 2.0f;
		float Force = 2.0f;
		float Size = 10.0f;
		float Transparency = 1.0f;

		glm::vec4 MainColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec4 StartColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec4 EndColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

public:
	int GetParticleAmount()
	{
		return particles.size();
	}

	void SetParticleSpecification()
	{
		particle.Transparency = Transparency;
		particle.LifeTime = LifeTime;
		particle.ColorChangeSpeed = ColorSpeed;
	}

	void SetParticleStartColor()
	{
		particle.MainColor[0] = startColor[0];
		particle.MainColor[1] = startColor[1];
		particle.MainColor[2] = startColor[2];
		particle.MainColor[3] = particle.Transparency; //transparency
	}

	void SetParticleScale()
	{
		particle.pos.Scale.x = Size;
		particle.pos.Scale.y = Size;
	}

	void SetParticlePos()
	{
		int posX = 640;
		int posY = 365;
		particle.pos.Translation.x = posX;
		particle.pos.Translation.y = posY / 2;
	}

	void UpdateAndEmitParticles(Luna::DeltaTime dt)
	{
		particles.push_back(particle);
		
		//Render
		for (int i = 0; i < particles.size(); i++)
		{
			particles[i].LifeTime -= dt;
			particles[i].ColorChangeSpeed -= dt;
			particles[i].Size -= dt;
			particles[i].Transparency -= dt;
			particles[i].pos.Translation.y += force;
			particles[i].MainColor[3] = particles[i].Transparency;

			if (particles[i].ColorChangeSpeed <= ColorSpeed / 2)
			{
				particles[i].MainColor[0] = endColor[0];
				particles[i].MainColor[1] = endColor[1];
				particles[i].MainColor[2] = endColor[2];
				particles[i].MainColor[3] = particles[i].Transparency;
			}

			float temp = std::rand() % 4;
			if (temp == 0)
			{
				particles[i].pos.Translation.x += force + 2.0f;
			}
			else if (temp == 1)
			{
				particles[i].pos.Translation.x -= force + 2.0f;
			}
			else if (temp == 2)
			{
				particles[i].pos.Translation.y += force;
			}

			Luna::Renderer::Draw(particles[i].MainColor, particles[i].pos.GetTransform());


			if (particles[i].LifeTime <= 0)
			{
				particles.erase(particles.begin() + i);
			}
		}
	}

private:
	Particle particle;
	std::vector<Particle> particles;
};


int main()
{
	Luna::Application app("App");
	ParticleSystem particleSystem;

	std::srand(time(0));

	float lastFrameTime = 0.0f;
	glm::vec4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };

	while (app.IsRunning())
	{
		float elapsedTime = app.GetElapsedRuntime();
		Luna::DeltaTime deltaTime = elapsedTime - lastFrameTime;
		lastFrameTime = elapsedTime;

		ParticleCounter = particleSystem.GetParticleAmount();

		app.Clear(Black);

		//----------------------------------------------------
		//					PARTICLE DEMO
		//----------------------------------------------------
		{
			particleSystem.SetParticleSpecification();
			particleSystem.SetParticleScale();
			particleSystem.SetParticleStartColor();
			particleSystem.SetParticlePos();
			particleSystem.UpdateAndEmitParticles(deltaTime);
		}

		app.Display();
	}
}