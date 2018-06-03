//	VQEngine | DirectX11 Renderer
//	Copyright(C) 2016  - Volkan Ilbeyli
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//	Contact: volkanilbeyli@gmail.com

#include "SSAOTestScene.h"
#include "Renderer/Renderer.h"
#include "Engine/RenderPasses.h"

#if DO_NOT_LOAD_SCENES
void SSAOTestScene::Load(SerializedScene& scene) {}
void SSAOTestScene::Unload() {}
void SSAOTestScene::Update(float dt) {}
void SSAOTestScene::RenderUI() const {}
#else
void SSAOTestScene::Load(SerializedScene& scene)
{
	SetEnvironmentMap(EEnvironmentMapPresets::MILKYWAY);

	// grid arrangement ( (row * col) cubes that are 'CUBE_DISTANCE' apart from each other )
	constexpr size_t	CUBE_ROW_COUNT = 4;
	constexpr size_t	CUBE_COLUMN_COUNT = 3;
	constexpr size_t	CUBE_COUNT = CUBE_ROW_COUNT * CUBE_COLUMN_COUNT;
	constexpr float		CUBE_DISTANCE = 4.0f * 2.0f;
	{	
		const std::vector<vec3>   rowRotations = { vec3::Zero, vec3::Up, vec3::Right, vec3::Forward };
		static std::vector<float> sCubeDelays = std::vector<float>(CUBE_COUNT, 0.0f);
		const std::vector<LinearColor>  colors = { LinearColor::white, LinearColor::red, LinearColor::green, LinearColor::blue, LinearColor::orange, LinearColor::light_gray, LinearColor::cyan };

		for (int i = 0; i < CUBE_ROW_COUNT; ++i)
		{
			//Color color = c_colors[i % c_colors.size()];
			LinearColor color = vec3(1, 1, 1) * static_cast<float>(i) / (float)(CUBE_ROW_COUNT - 1);

			for (int j = 0; j < CUBE_COLUMN_COUNT && false; ++j)
			{
				GameObject* pCube = Scene::CreateNewGameObject();

				// Transform
				Transform tf;
				float x, y, z;	// position
				x = i * CUBE_DISTANCE - CUBE_ROW_COUNT * CUBE_DISTANCE / 2;
				y = 5.0f + pCubes.size();
				z = j * CUBE_DISTANCE - CUBE_COLUMN_COUNT * CUBE_DISTANCE / 2 + 50;

				tf.SetPosition(x, y, z);
				tf.SetUniformScale(4.0f);
				pCube->SetTransform(tf);

				// Mesh
				pCube->AddMesh(EGeometry::CUBE);

				// Materials
				
				BRDF_Material* pBRDF = static_cast<BRDF_Material*>(Scene::CreateNewMaterial(GGX_BRDF));
				BlinnPhong_Material* pPhong = static_cast<BlinnPhong_Material*>(Scene::CreateNewMaterial(BLINN_PHONG));

				const TextureID texNormalMap = mpRenderer->CreateTextureFromFile("simple_normalmap.png");

				// set common material properties
				std::array<Material*, 2> materials{ pBRDF, pPhong };
				std::for_each(materials.begin(), materials.end(), [&](Material* pMat)
				{
					pMat->diffuse = color;
					pMat->alpha = 1.0f;
					pMat->diffuseMap = AmbientOcclusionPass::whiteTexture4x4;
					pMat->normalMap = texNormalMap;
					pCube->AddMaterial(pMat->ID);
				});

				pCubes.push_back(pCube);
			}
		}
	}
}

void SSAOTestScene::Unload()
{
	pCubes.clear();
}

void SSAOTestScene::Update(float dt){}
void SSAOTestScene::RenderUI() const {}
#endif