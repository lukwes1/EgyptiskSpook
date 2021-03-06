#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include "Renderer.h"
#include "GraphicsData.h"
#include "CameraClass.h"
#include <SimpleMath.h>
#include <vector>

class ParticleRenderer : public Renderer {
	private:

		struct ParticleVertex {
			DirectX::SimpleMath::Vector3 position;
			DirectX::SimpleMath::Vector2 dimensions;
		};

		struct ParticleData {
			DirectX::SimpleMath::Vector3 direction;
			float timeLeft;
		};
		GraphicsData *mGraphicsData;

		std::vector<ParticleVertex> mParticleVertices;
		std::vector<ParticleData> mParticleData;
		CameraClass *mCamera;
		UINT frame;

		void updateCameraBuffer(ID3D11DeviceContext *context);
		void updateParticles(ID3D11DeviceContext * context);

		UINT getSize() const;
		void addRandomParticle(bool timeIsRandom = false);
		//check life time, if <= 0, remove and add new one
		void timeCheck(int start, int piece);
		static float getRandomNr();
	public:
		ParticleRenderer(CameraClass *camera, GAMESTATE identifier);
		ParticleRenderer(ParticleRenderer const &renderer) = delete;
		virtual ~ParticleRenderer();

		void setup(ID3D11Device *device, ShaderHandler &shaders);
		void render(ID3D11DeviceContext *context, ShaderHandler &shaders, GAMESTATE const &state);

		ParticleRenderer* operator=(ParticleRenderer const &renderer) = delete;
};

#endif