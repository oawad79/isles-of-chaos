#include "character.hpp"
#include "enttypes.hpp"

void Health::hit(float damage) {
    if (!canHit()) return;
    amount -= damage;
    hitTimer = maxHitTimer;
}

void Health::heal(float by){
    amount += by;
}

bool Health::shouldDie(){
    return amount <= 0;
}

bool Health::canHit(){
    return hitTimer <= 0;
}

void UpdateCharacter(entt::registry& reg) {
    auto health_ents = reg.view<Health>(entt::exclude<Disabled>);

    for (auto& ent : health_ents) {
        auto& health = reg.get<Health>(ent);
        if (health.shouldDie()) reg.destroy(ent);
        else health.hitTimer -= GetFrameTime();
    }
}
