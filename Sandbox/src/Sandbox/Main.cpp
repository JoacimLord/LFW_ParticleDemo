#include "Luna.h"
#include <imgui/imgui.h>

#include <random>


namespace GuiSettings {

	float StartColor[4] = { 255.0f, 0.0f, 180.0f, 255.0f };
	float EndColor[4] = { 0.0f, 188.0f, 255.0f, 255.0f };
	float LifeTime = 1.0f;
	float ColorSpeed = 0.5f;
	float Transparency = 1.0f;
	int ParticleCounter = 0;
	float Size = 35;
	float Force = 3.5f;
}


void Luna::Application::BuildUI()
{
	ImGui::Begin("ParticleStats");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Amount of particles in scene %i", GuiSettings::ParticleCounter);
		ImGui::SliderFloat("Lifetime (seconds)", &GuiSettings::LifeTime, 0, 10);

		ImGui::SliderFloat("Force ", &GuiSettings::Force, 0, 15);
		ImGui::SliderFloat("ColorSpeedTime (seconds)", &GuiSettings::ColorSpeed, 0, 10);
		ImGui::SliderFloat("Size", &GuiSettings::Size, 0, 50);
		ImGui::ColorEdit4("StartColor", GuiSettings::StartColor);
		ImGui::ColorEdit4("EndColor", GuiSettings::EndColor);

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
		particle.Transparency = GuiSettings::Transparency;
		particle.LifeTime = GuiSettings::LifeTime;
		particle.ColorChangeSpeed = GuiSettings::ColorSpeed;
	}

	void SetParticleStartColor()
	{
		particle.MainColor[0] = GuiSettings::StartColor[0];
		particle.MainColor[1] = GuiSettings::StartColor[1];
		particle.MainColor[2] = GuiSettings::StartColor[2];
		particle.MainColor[3] = particle.Transparency; //transparency
	}

	void SetParticleScale()
	{
		particle.pos.Scale.x = GuiSettings::Size;
		particle.pos.Scale.y = GuiSettings::Size;
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
			particles[i].pos.Translation.y += GuiSettings::Force;
			particles[i].MainColor[3] = particles[i].Transparency;

			if (particles[i].ColorChangeSpeed <= GuiSettings::ColorSpeed / 2)
			{
				particles[i].MainColor[0] = GuiSettings::EndColor[0];
				particles[i].MainColor[1] = GuiSettings::EndColor[1];
				particles[i].MainColor[2] = GuiSettings::EndColor[2];
				particles[i].MainColor[3] = particles[i].Transparency;
			}

			float temp = std::rand() % 4;
			if (temp == 0)
			{
				particles[i].pos.Translation.x += GuiSettings::Force + 2.0f;
			}
			else if (temp == 1)
			{
				particles[i].pos.Translation.x -= GuiSettings::Force + 2.0f;
			}
			else if (temp == 2)
			{
				particles[i].pos.Translation.y += GuiSettings::Force;
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
	
	while (app.IsRunning())
	{
		float elapsedTime = app.GetElapsedRuntime();
		Luna::DeltaTime deltaTime = elapsedTime - lastFrameTime;
		lastFrameTime = elapsedTime;

		GuiSettings::ParticleCounter = particleSystem.GetParticleAmount();

		// Black background clear color
		app.Clear(0.0f, 0.0f, 0.0f, 1.0f);

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