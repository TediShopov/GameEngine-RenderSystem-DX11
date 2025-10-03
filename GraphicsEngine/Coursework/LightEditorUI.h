#pragma once
#include "BaseEditorUI.h"
#include "Light.h"


#define ARR3_DECOMP(arr) arr[0], arr[1], arr[2]
#define ARR4_DECOMP(arr) arr[0], arr[1], arr[2], arr[3]

#define XMFLOAT3_DECOMP(arr) arr.x, arr.y, arr.z 
/// <summary>
/// Instance of BaseEditorUI class. Can collect data from Transform, display it to ImGui and set the changes.
/// </summary>
class LightEditorUI : public BaseEditorUI<std::vector<Light>, std::vector<Light*>>
{

private: 
    float mulPos = 200;
public:


    LightEditorUI()
    {
       
    }
    void updateStateOfUI(std::vector<Light*> lights) override
    {
        
        for (int i = 0; i < lights.size(); i++)
        {
            int lastElementIndex = this->_rawInfo.size() - 1;
            if (i > lastElementIndex)
            {
                this->_rawInfo.push_back(Light());
            }
            this->_rawInfo[i] = *lights[i];
        }
    };


    virtual void applyChangesTo(std::vector<Light*> lights)  override
    {
        for (size_t i = 0; i < lights.size(); i++)
        {
            (*lights[i])=this->_rawInfo[i];
        }
    }

    void appendPointLightInfoToGUI(Light* info,std::string text,std::string appendText)
    {
        ImGui::Text(text.c_str());
        XMFLOAT3 pos;

        ImGui::DragFloat3(("Position" + appendText).c_str(), info->position.m128_f32, 0.1f, -100, 100);
        //ImGui::DragFloat3(("Attenuation" + appendText).c_str(), &info->attenuationFactors.x, 0.1f, -10, 10);
        ImGui::DragFloat(("Attenuation Linear" + appendText).c_str(), &info->attenuationFactors.x, 0.01f, 0.01, 50);
        ImGui::DragFloat(("Attenuation Cubic" + appendText).c_str(), &info->attenuationFactors.y, 0.001f, 0.0, 2);
        ImGui::DragFloat(("Attenuation Quadratic" + appendText).c_str(), &info->attenuationFactors.z, 0.0001, 0.0, 1);

        ImGui::ColorEdit3(("Ambient" + appendText).c_str(), &info->ambientColour.x);
        ImGui::ColorEdit3(("Diffuse" + appendText).c_str(), &info->diffuseColour.x);
        ImGui::ColorEdit3(("Specular" + appendText).c_str(), &info->specularColour.x);
    }

    void appendSpotLightInfoToGUI(Light* info, std::string text, std::string appendText)
    {
        ImGui::Text(text.c_str());
        XMFLOAT3 pos;

        ImGui::DragFloat3(("Position" + appendText).c_str(), info->position.m128_f32, 0.1f, -100, 100);
        ImGui::DragFloat3(("Direction" + appendText).c_str(), &info->direction.x, 0.1f, -1, 1);
        XMVECTOR dirNormalized = XMLoadFloat3(&info->direction);
        dirNormalized = XMVector3Normalize(dirNormalized);
        XMStoreFloat3(&info->direction, dirNormalized);
        
        ImGui::ColorEdit3(("Ambient" + appendText).c_str(), &info->ambientColour.x);
        ImGui::ColorEdit3(("Diffuse" + appendText).c_str(), &info->diffuseColour.x);
        ImGui::ColorEdit3(("Specular" + appendText).c_str(), &info->specularColour.x);
        //ImGui::DragFloat3(("Attenuation" + appendText).c_str(), &info->attenuationFactors.x, 0.1f, 0, 100);
        ImGui::DragFloat(("Attenuation Linear" + appendText).c_str(), &info->attenuationFactors.x, 0.01f, 0.01, 50);
        ImGui::DragFloat(("Attenuation Cubic" + appendText).c_str(), &info->attenuationFactors.y, 0.001f, 0.0, 2);
        ImGui::DragFloat(("Attenuation Quadratic" + appendText).c_str(), &info->attenuationFactors.z, 0.0001, 0.0, 1);
        ImGui::DragFloat(("CutOff Inner" + appendText).c_str(), &info->innerCutOff, 0.05f, 0, 10);
        ImGui::DragFloat(("CutOff Outer" + appendText).c_str(), &info->outerCutOff, 0.05f, 0, 1);


    }

    virtual void appendToImgui() override
    {
        ImGui::Begin("Lights");
        appendPointLightInfoToGUI(&_rawInfo[0],"PointLight", "1");
        appendSpotLightInfoToGUI(&_rawInfo[1], "SpotLight", "2");

        ImGui::Text("DirectionalLight");
        ImGui::DragFloat3("Direction3", &_rawInfo[2].direction.x, 0.1f, -100, 100);
     
        XMVECTOR dirNormalized = XMLoadFloat3(&_rawInfo[2].direction);
        dirNormalized = XMVector3Normalize(dirNormalized);
        XMStoreFloat3(&_rawInfo[2].direction, dirNormalized);
      
        ImGui::ColorEdit3((" Directional Ambient"), &_rawInfo[2].ambientColour.x);
        ImGui::ColorEdit3((" DirectionalDiffuse"), &_rawInfo[2].diffuseColour.x);
        ImGui::ColorEdit3((" Directional Specular" ), &_rawInfo[2].specularColour.x);
        ImGui::End();

    }
};

