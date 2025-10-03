#pragma once
#include "DefaultShader.h"

struct  LinearColorGradient 
{
	// contains the color in the first 3 values and STOP point in the gradient in the fourt
    XMFLOAT4 colors[8];
    float power;
    float noiseAmplitude;
    float noiseFrequency;
    float normalStrength;
	
};
class insideOutsideShader :
    public DefaultShader
{

public:
    LinearColorGradient rainbowColors;
    ShaderBuffer<LinearColorGradient> rainbowColorsParam;
    //insideOutsideShader(ID3D11Device* device, HWND hwnd) : DefaultShader(device,hwnd,L"ShadowVertexShader.cso",L"InsideOutsideShader.cso")
    insideOutsideShader(ID3D11Device* device, HWND hwnd) : DefaultShader(device,hwnd)
    {
        rainbowColors.power = 1;
        rainbowColors.noiseAmplitude = 1;

        rainbowColors.noiseFrequency = 500;
        rainbowColors.normalStrength = 0.1;
		D3D11_INPUT_ELEMENT_DESC polygonLayoutWithTangents[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
		loadVertexShaderWLayout(L"InsideOutsideVertexShader.cso",polygonLayoutWithTangents,5);
        loadPixelShader(L"InsideOutsideShader.cso");
        rainbowColorsParam.setToStage = PIXEL;
        rainbowColorsParam.setToPosition = 5;
        rainbowColorsParam.Create(device);


        rainbowColors.colors[0] =XMFLOAT4(1, 0, 0, 0);             // Red
        rainbowColors.colors[1] =XMFLOAT4(1, 0.5, 0, 0.16);           // Orange
        rainbowColors.colors[2] =XMFLOAT4(1, 1, 0, 0.33);              //Yellow 
        rainbowColors.colors[3] =XMFLOAT4(0, 1, 0, 0.5);              //Green
        rainbowColors.colors[4] =XMFLOAT4(0, 0, 1, 0.66);              //Blue
        rainbowColors.colors[5] =XMFLOAT4(0.3, 0, 0.5, 0.83);          //Inigo
        rainbowColors.colors[6] =XMFLOAT4(0.5, 0, 1.0, 0.85);          //Violet
        rainbowColors.colors[7] =XMFLOAT4(1, 1, 1, 1);

        computeEvenlyPlacedStopsForRainbowColors(0, 0.7);
        rainbowColors.colors[7] =XMFLOAT4(1, 1, 1, 1); //White Till The End



    }
    void computeEvenlyPlacedStopsForRainbowColors(float min, float max)
    {
        int colorsNum = 7;
        float increment = (max - min) / (float)colorsNum;
        for (int i = 0; i < 7; i++)
        {
            rainbowColors.colors[i].w = 0 + increment * i;
        }



    }
    void setInstanceOnTheInside(ID3D11DeviceContext* devCon)
    {
        rainbowColorsParam.SetTo(devCon,&rainbowColors);

    }
};

