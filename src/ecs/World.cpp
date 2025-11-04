#include "ecs/World.h"
#include "components/Model.h"

// EntityBuilder::With<Model> specialization implementation
template<>
EntityBuilder& EntityBuilder::With<Model>(std::string&& filePath) {
 world_->Add<Model>(entity_, Model{std::move(filePath)});
 return *this;
}