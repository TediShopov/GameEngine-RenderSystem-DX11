#pragma once
#include "RenderItem.h"
class LimitedTimeRenderCollection :
    public RenderItemCollection
{
protected:
public:
    LimitedTimeRenderCollection(DefaultShader* shader,
		std::function<void(DefaultShader* shaderToRenderWith)> shaderSetupFunction,
		std::function<void(DefaultShader* shaderToRenderWith,MeshInstance*)>  drawInstanceFunctionParameter) : RenderItemCollection(
        shader, shaderSetupFunction,drawInstanceFunctionParameter)
    {
            
    }
    //A mapping of the time in seconds the object will be rendered before deleted
    std::vector<float> timeToStay;
    
    void updateTimeStep(float timeToSubtract) {
        //Update all time with the current time step
        for (int i = 0; i < this->timeToStay.size(); i++)
        {
            this->timeToStay[i] -= timeToSubtract;
        }


        //Mark any mesh instances indices that have overstayed
        cleanup();
    }
    void cleanup() {
        size_t write_index = 0;

        for (size_t i = 0; i < this->size(); ++i) {
			if (timeToStay[i] < 0.0f) {
				// Erase in both collectoins
				timeToStay.erase(timeToStay.begin() + i);  
				this->erase(this->begin() + i);  
			}
			else {
				++i;  
			}
        }
    }

    void addRenderItem(MeshInstance* meshInstnace, float renderTime)
    {
        this->push_back(meshInstnace);
        this->timeToStay.push_back(renderTime);
    }



};

