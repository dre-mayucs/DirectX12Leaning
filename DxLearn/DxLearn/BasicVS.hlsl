float4 main(float4 pos : POSITION) : SV_POSITION
{
	//float4x4 world;
	//float4 Out = pos * float4(.2f, .2f, 0.f, 0.f);
    float4 Out = pos;
	
	return Out;
}