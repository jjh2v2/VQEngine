//	DX11Renderer - VDemo | DirectX11 Renderer
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

#include "LightingCommon.hlsl"
#include "BRDF.hlsl"

struct PSIn
{
	float4 position		 : SV_POSITION;
	float2 uv			 : TEXCOORD0;
};

cbuffer SceneVariables
{
    matrix viewToWorld;
	float ambientFactor;
};

Texture2D tPosition;	//todo remove
Texture2D tDiffuseRoughnessMap;
Texture2D tSpecularMetalnessMap;
Texture2D tNormalMap;
Texture2D tAmbientOcclusion;
Texture2D tIrradianceMap;

Texture2D tSpecularMap;
Texture2D tBRDFIntegrationLUT;

SamplerState sNearestSampler;

// todo: header for general stuff like this sampling
float2 SphericalSample(float3 v)
{
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509575(v=vs.85).aspx
	// The signs of the x and y parameters are used to determine the quadrant of the return values 
	// within the range of -PI to PI. The atan2 HLSL intrinsic function is well-defined for every point 
	// other than the origin, even if y equals 0 and x does not equal 0.
    float2 uv = float2(atan2(v.z, v.x), asin(-v.y));
    uv /= float2(2 * PI, PI);
    uv += float2(0.5, 0.5);
    return uv;
}

float4 PSMain(PSIn In) : SV_TARGET
{
    const float4 kD_roughness = tDiffuseRoughnessMap.Sample(sNearestSampler, In.uv);
    const float4 kS_metalness = tSpecularMetalnessMap.Sample(sNearestSampler, In.uv);

	// view-space vectors
    const float3 Pv = tPosition.Sample(sNearestSampler, In.uv).rgb;
    const float3 Nv = tNormalMap.Sample(sNearestSampler, In.uv).rgb;
    const float3 Vv = normalize(-Pv);
    
	// world-space vectors
	const float3 Vw = normalize(mul(viewToWorld, float4(Vv, 0.0f)).xyz);
    const float3 Nw = mul(viewToWorld, float4(Nv, 0.0f)).xyz;

	// environment map
    const float2 equirectangularUV = SphericalSample(normalize(Nw));
    const float3 environmentIrradience = tIrradianceMap.Sample(sNearestSampler, equirectangularUV).rgb;
    const float3 environmentSpecular = tSpecularMap.Sample(sNearestSampler, equirectangularUV).rgb;
    
	// ambient occl
	const float  ambientOcclusion = tAmbientOcclusion.Sample(sNearestSampler, In.uv).r;	

	// surface
	BRDF_Surface s;
    s.N = Nw;
    s.diffuseColor = kD_roughness.rgb;
    s.specularColor = kS_metalness.rgb;
    s.roughness = kD_roughness.a;
    s.metalness = kS_metalness.a;

	// ambient irradiance
    float3 Ia = EnvironmentBRDF(s, Vw, ambientOcclusion * ambientFactor, environmentIrradience);

	// specular lighting
    const float NdotV = max(dot(Nw, Vw), 0);

    const float3 F0 = 0.04f.xxx;
    const float3 F = FresnelWithRoughness(NdotV, F0, s.roughness);
    const float2 F0ScaleBias = tBRDFIntegrationLUT.Sample(sNearestSampler, float2(NdotV, s.roughness)).rg;
    float3 Is = environmentSpecular * (F * F0ScaleBias.x + F0ScaleBias.y);

    float3 I = (Ia + Is) * 1;
    return float4(I, 1.0f);
}