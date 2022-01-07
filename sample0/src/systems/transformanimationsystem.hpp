/**
 *  Mana - 3D Game Engine
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MANA_TRANSFORMANIMATIONSYSTEM_HPP
#define MANA_TRANSFORMANIMATIONSYSTEM_HPP


#include "ecs/system.hpp"

#include "components/transformanimationcomponent.hpp"

using namespace engine;

class TransformAnimationSystem : public System {
public:
    void update(float deltaTime, EntityManager &entityManager) override {
        auto &componentManager = entityManager.getComponentManager();
        for (auto &pair: componentManager.getPool<TransformAnimationComponent>()) {
            auto transform = componentManager.lookup<TransformComponent>(pair.first);

            transform.transform.setPosition(transform.transform.getPosition() + pair.second.translation * deltaTime);
            transform.transform.setRotation(transform.transform.getRotation() * Quaternion(pair.second.rotation * deltaTime));

            componentManager.update<TransformComponent>(pair.first, transform);
        }
    }
};

#endif //MANA_TRANSFORMANIMATIONSYSTEM_HPP
