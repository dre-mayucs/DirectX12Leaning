#include <cmath>
#include <DirectXMath.h>

#include "tempUtility.h"
#include "DrawUtility.h"

void SetDrawData::SetDrawTriangularPyramidObjVertexData3D(Vertex3D *vertices)
{
	for (auto i = 0; i < 3; ++i) {
		vertices[i] = {
			{
				(float)1 * sinf(DirectX::XM_2PI / 3 * i),
				(float)1 * cosf(DirectX::XM_2PI / 3 * i),
				0.f,
			},
			{ },
			{ 0.0f, 0.0f }
		};
	}
	vertices[3] = { {0.0f, 0.0f, 0.0f }, {}, { 0.0f, 0.0f } };
	vertices[4] = { {0.0f, 0.0f, -1 }, {}, { 0.0f, 0.0f } };
}
