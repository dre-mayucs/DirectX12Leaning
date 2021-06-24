#pragma once

class SetDrawData
{
public:
	void SetDrawTriangularPyramidObjVertexData3D(Vertex3D *vertices);
	Vertex3D *SetDrawTriangularPyramidObjIndexData3D();
	Vertex3D *SetDrawBoxObjVertexData3D();
	Vertex3D *SetDrawBoxObjIndexData3D();

	Vertex *SetDrawCircleVertexData2D();
	Vertex *SetDrawCircleIndexData2D();
	Vertex *SetDrawBoxVertexData2D();
	Vertex *SetDrawBoxIndexData2D();
};

typedef struct DrawTriangularPyramidObjData3D
{
	Vertex3D vertices[5];

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

typedef struct DrawBoxObjData3D
{

};

typedef struct DrawCircleData2D
{

};

typedef struct DrawBoxData2D
{

};