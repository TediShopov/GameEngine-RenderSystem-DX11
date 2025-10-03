#include "ActiveInstanceSelectorUI.h"

#include "Scene.h"
 std::vector<MeshInstance*> ActiveInstanceSelectorUI::getMeshInstanceChildren(MeshInstance* node)
{
    std::vector<MeshInstance*> children;
    auto vecTransforms = node->transform.getChildrenTransforms();
    for (auto transformChild : node->transform.getChildrenTransforms())
    {
//        auto found = std::find_if(_rawInfo.scene->meshInstances.begin(), _rawInfo.scene->meshInstances.end(),
//            [=](const std::unique_ptr<MeshInstance>& m) { Transform* t = &(m->transform);
//        return t == transformChild; });
//        if (found != _rawInfo.scene->meshInstances.end())
//        {
//            children.push_back((*found).get());
//        }
    }
    return children;
}

  void ActiveInstanceSelectorUI::updateStateOfUI(Scene* obj)
 {
     _rawInfo.oldmeshInstance = obj->activeMeshInstance;
     _rawInfo.newMeshInstance = nullptr;
     _rawInfo.isNew = false;
     _rawInfo.scene = obj;
 }

  void ActiveInstanceSelectorUI::applyChangesTo(Scene* obj)
 {
     //Need to update
     if (_rawInfo.oldmeshInstance != _rawInfo.newMeshInstance && _rawInfo.newMeshInstance != nullptr)
     {
         _rawInfo.isNew = true;
         obj->activeMeshInstance = _rawInfo.newMeshInstance;

     }
 }

  void ActiveInstanceSelectorUI::appendToImgui()
 {
     ImGui::Begin("Mesh Instance Tree");
     ImGui::Text(
         "This is a more typical looking tree with selectable nodes.\n"
         "Click to select, CTRL+Click to toggle, click on arrows or double-click to open.");
     static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
     static bool align_label_with_current_x_position = false;
     static bool test_drag_and_drop = false;
     base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
     if (align_label_with_current_x_position)
         ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());



     //scene->setRootInstances();
     //bool is_open = false;
     for (auto root : _rawInfo.scene->rootMeshInstances)
     {
         //unsigned int tree_level_counter = 0;

         dfsMeshInstanceToImgui(root,
             [&](MeshInstance* m) -> bool
             {
                 ImGuiTreeNodeFlags node_flags = base_flags;
                 bool is_selected = m == _rawInfo.oldmeshInstance;
                 bool is_open = false;
                 if (is_selected) {
                     node_flags |= ImGuiTreeNodeFlags_Selected;
                 }

                 if (m->transform.getChildrenTransforms().size() == 0)
                 {
                     //Leaf node
                     node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                     ImGui::TreeNodeEx((void*)(intptr_t)m, node_flags, "Selectable Leaf %d", m);
                     if (ImGui::IsItemClicked())
                         _rawInfo.newMeshInstance = m;
                     //ImGui::TreePop();
                 }
                 else
                 {
                     //Normal node
                     bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)m, node_flags, "Selectable Node %d", m);
                     if (ImGui::IsItemClicked())
                     {
                         _rawInfo.newMeshInstance = m;
                     }

                     is_open = node_open;
                     if (node_open)
                     {
                         ImGui::BulletText("Blah blah\nBlah Blah");
                     }
                 }
                 return is_open;
             });
     }

     if (_rawInfo.newMeshInstance != nullptr)
     {
         //update ImGui info on the new values
         //update transform information

     }
     if (align_label_with_current_x_position)
         ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
     ImGui::End();
 }
