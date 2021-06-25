#pragma once

struct DrawTriangularPyramidObjData3D
{
	const int VertexSize = 5;
	const int IndexSize = 18;

	Vertex3D vertices[5] =
	{
		{ {		  0.0f,	 1.0f,  0.0f }, {}, { 0.0f, 0.0f } },
		{ {  0.866025f, -0.5f,  0.0f }, {}, { 0.0f, 0.0f } },
		{ { -0.866025f, -0.5f,  0.0f }, {}, { 0.0f, 0.0f } },
		{ {       0.0f,  0.0f,  0.0f }, {}, { 0.0f, 0.0f } },
		{ {       0.0f,  0.0f, -1.0f }, {}, { 0.0f, 0.0f } }
	};

	unsigned short indices[18] =
	{
		1, 0, 3,
		2, 1, 3,
		0, 2, 3,
		0, 1, 4,
		1, 2, 4,
		2, 0, 4
	};
};

struct DrawBoxObjData3D
{

};

struct DrawCircleData2D
{

};

struct DrawBoxData2D
{

};