#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint outEntity;

layout(scalar, set = 2, binding = 0) writeonly buffer SelectedEntityData {
  uint selectedEntity;
}
uSelectedEntity;

void main() { uSelectedEntity.selectedEntity = outEntity; }
