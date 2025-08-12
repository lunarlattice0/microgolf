#include "putrid/world/world.hpp"
#include "vendor/entt/src/entt/entity/fwd.hpp"
#include <raylib.h>

WorldState::WorldState() {
    this->world = entt::registry();
}

WorldState::~WorldState() {
    this->world.clear();
}

entt::entity WorldState::addNewEntity(std::string entityName) {
    auto entity = this->world.create();
    this->world.emplace<EntityName>(entity, entityName);
    this->world.emplace<Location>(entity, (Vector3){0,0,0});
    return entity;
}

void WorldState::destroyEntity(entt::entity entity) {
    this->world.destroy(entity);
}
