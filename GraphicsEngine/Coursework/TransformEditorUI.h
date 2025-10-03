#pragma once
#include "BaseEditorUI.h"
#include "Transform.h"
struct TransformInfo
{
    float position[3];
    float rotation[3];
    float scale[3];
};
/// <summary>
/// Instance of BaseEditorUI class. Can collect data from Transform, display it to ImGui and set the changes.
/// </summary>
class TransformEditorUI : public BaseEditorUI<TransformInfo, Transform*>
{
public:
    void updateStateOfUI(Transform* transform) override
    {

        auto pos = transform->getPosition();
        XMVectorToFloatArr(pos, this->_rawInfo.position);

        auto scl = transform->getScale();
        XMVectorToFloatArr(scl, this->_rawInfo.scale);

        _rawInfo.rotation[0] = transform->getPitch();
        _rawInfo.rotation[1] = transform->getYaw();
        _rawInfo.rotation[2] = transform->getRoll();

    };
    virtual void applyChangesTo(Transform* transform)  override
    {
        transform->setPosition(
            _rawInfo.position[0],
            _rawInfo.position[1],
            _rawInfo.position[2]);


        transform->setRotation(
            _rawInfo.rotation[0],
            _rawInfo.rotation[1],
            _rawInfo.rotation[2]);
        transform->setScale(
            _rawInfo.scale[0],
            _rawInfo.scale[1],
            _rawInfo.scale[2]);
    }

    virtual void appendToImgui() override
    {
        ImGui::Text("Transform Editor");
        ImGui::DragFloat3("Position", _rawInfo.position, 0.15);
        ImGui::DragFloat3("Rotation", _rawInfo.rotation, 0.15);
        ImGui::DragFloat3("Scale", _rawInfo.scale, 0.15);
    }
};

