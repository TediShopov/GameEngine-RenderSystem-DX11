#pragma once
#include "TextureShader.h"
class GizmosOverlay :
    public TextureShader
{
public:


public:
	//ShaderBuffer<ViggneteMask> vignetteMask;
	//ShaderBuffer<ScreenResolutionBuffer> resolutionParams;
	//ShaderBuffer<BloomData> bloomDataParam;
	//BloomData bloomData;
	ShaderTextureParam gizmosSRVResource;
	ID3D11ShaderResourceView* gizmosSRV;


	GizmosOverlay(ID3D11Device* device, HWND hwnd) : TextureShader(device, hwnd)
	{
		loadVertexShader(L"BaseTextureVertexShader.cso");
		loadPixelShader(L"GizmosOverlayPixelShader.cso");
		//Another texture passed into 
		gizmosSRVResource.setToPosition = 1;
		gizmosSRVResource.setToStage = PIXEL;

	}
	void setIntrinsicParams(ID3D11DeviceContext* devCon) override
	{
		gizmosSRVResource.SetTo(devCon, gizmosSRV);
	}
};

