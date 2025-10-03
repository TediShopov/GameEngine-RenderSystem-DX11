#pragma once
#include "DXF.h"




template <typename RawInfoStruct, typename T>
/// <summary>
/// The Base class of all ui editor classes. It contains overridable methods for refeshing the state of the
///  Imgui UI, rendering and updating the acutal object with the new information by user. Also contains some
/// helpers methods to convert from XMVECTOR and XMMATRIX variables to floats to be used by ImGui
/// </summary>
/// <typeparam name="RawInfoStruct">The struct which will contain all data collected from the object
///   that can be modfied by the user in the UI </typeparam>
/// <typeparam name="T">the type of the  actual object the UI is trying to manipulate</typeparam>
class BaseEditorUI
{
public:
	RawInfoStruct _rawInfo;

     const RawInfoStruct& getRawData() { return _rawInfo; }

	virtual void updateStateOfUI(T obj)=0;
	virtual void applyChangesTo(T obj)=0;

	virtual void appendToImgui()=0;

    void XMVectorToFloatArr(XMVECTOR vec, float* floatVec)
    {
        XMFLOAT4 vecfour;
        XMStoreFloat4(&vecfour, vec);
        floatVec[0] = vecfour.x;
        floatVec[1] = vecfour.y;
        floatVec[2] = vecfour.z;
    }

    void XMFloat3ToArr(XMFLOAT3 xmfloat3,float float3[3]) 
    {
        float3[0] = xmfloat3.x;
        float3[1] = xmfloat3.y;
        float3[2] = xmfloat3.z;
    }

    void XMFloat4ToArr(XMFLOAT4 xmfloat4, float float4[4])
    {
        float4[0] = xmfloat4.x;
        float4[1] = xmfloat4.y;
        float4[2] = xmfloat4.z;
        float4[3] = xmfloat4.w;

    }

};

