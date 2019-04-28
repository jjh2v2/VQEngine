//	VQEngine | DirectX11 Renderer
//	Copyright(C) 2018  - Volkan Ilbeyli
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
#pragma once

#include <vector>

#include "Application/HandleTypedefs.h"

#include "Utilities/vectormath.h"

struct FrustumPlaneset;
struct vec3;
struct MeshDrawData;

class GameObject;


struct BoundingBox
{
	vec3 low = vec3::Zero;
	vec3 hi = vec3::Zero;
	DirectX::XMMATRIX GetWorldTransformationMatrix() const;
};

struct CullMeshData
{
	FrustumPlaneset frustumPlanes;
	XMMATRIX matWorld; // rename to matTransform to be more generic?
	const std::vector<BoundingBox>* pLocalSpaceBoundingBoxes;

	const std::vector<MeshID>* pMeshIDs;
	MeshDrawData* pMeshDrawData;
};

namespace VQEngine
{
	bool IsSphereInFrustum(const FrustumPlaneset& frustum, const vec3& sphereCenter, const float sphereRadius);

	bool IsVisible(const FrustumPlaneset& frustum, const BoundingBox& aabb);

	// deprecated
	size_t CullMeshes(const FrustumPlaneset& frustumPlanes, const GameObject* pObj, MeshDrawData& meshDrawData);


	// returns the indices of visible bounding boxes
	//
	std::vector<int> CullMeshes(CullMeshData& data);

	size_t CullGameObjects
	(
		const FrustumPlaneset&                  frustumPlanes
		, const std::vector<const GameObject*>& pObjs
		, std::vector<const GameObject*>&       pCulledObjs
	);
}