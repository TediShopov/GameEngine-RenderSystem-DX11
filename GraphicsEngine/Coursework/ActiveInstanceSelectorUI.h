#pragma once
#include "BaseEditorUI.h"
#include "MeshInstance.h"
class Scene;
 struct ActiveInstanceInfo 
{
    MeshInstance* oldmeshInstance;
    MeshInstance* newMeshInstance;
    bool isNew;
    Scene* scene;

};
class ActiveInstanceSelectorUI : public BaseEditorUI<ActiveInstanceInfo,Scene*>
{

private:
    std::vector<MeshInstance*> getMeshInstanceChildren(MeshInstance* node);

    template<typename Func>
    void dfsMeshInstanceToImgui(MeshInstance* node, Func f) {

        bool is_open = f(node);
        //Do for all children
        for (auto childNode : getMeshInstanceChildren(node))
        {
            if (is_open)
            {
                dfsMeshInstanceToImgui(childNode, f);
            }
        }

        if (node->transform.getChildrenTransforms().size() != 0 && is_open)
        {
            ImGui::TreePop();
        }
    }

public:

    virtual void updateStateOfUI(Scene* obj);
    virtual void applyChangesTo(Scene* obj);

    virtual void appendToImgui();
   


};

