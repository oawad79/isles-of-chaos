#include "actor.hpp"

void UpdateActor(entt::registry& reg) {
    auto view = reg.view<Actor, Physics, Body>();

    for (auto& ent : view) {
        auto& actor = reg.get<Actor>(ent);
        auto& body = reg.get<Body>(ent);
        auto& physics = reg.get<Physics>(ent);

        if (actor.type == ActorType::ENEMY) {
            reg.view<PlayerHit, Body, Item>().each([&](auto& phit, auto& obody, auto& item){
                const auto damage = item.effectValue;
                if (CheckCollisionRecs(body, obody)) {
                    if (reg.has<Health>(ent)) {
                        auto& health = reg.get<Health>(ent);
                        health.hit(damage);
                    }
                }
            });
        }
    }
}
