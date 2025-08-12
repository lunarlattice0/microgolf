// This file manages the ECS for the world, and defines entity containers.

#pragma once

#include "src/vendor/entt/single_include/entt/entt.hpp"
#include <cereal/cereal.hpp>
#include <raylib.h>

class WorldState {
    entt::registry world;

    public:
        WorldState();
        ~WorldState();
        entt::entity addNewEntity(std::string entityName);
        void destroyEntity(entt::entity entity);
};

class Entity {
    private:
        Model model; // Please use GLB for this.
        BoundingBox bounds;
    public:
        Entity(const char * modelPath);
        ~Entity();
        template<class Archive>
        void serialize(Archive& archive ) {
            archive(CEREAL_NVP(model), CEREAL_NVP(bounds));
        }
};

struct EntityName {
    std::string name;
};

struct Location {
    Vector3 location;
};
