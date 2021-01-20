#include "particles.hpp"

entt::entity SpawnParticleWithDesc(entt::registry& reg, ParticleDesc& desc, const Vector2 position) {
    auto self = reg.create();

    auto& body = reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = desc.region.width;
    body.height = desc.region.height;

    auto& spr = reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_PARTICLES];
    spr.region = desc.region;
    spr.rotation = 0.0f;
    spr.layer = 100.0f;

    auto& physics = reg.emplace<Physics>(self);
    physics.type = PhysicsType::KINEMATIC;
    physics.friction = 1.0f;
    physics.gravityScale.y = 0.0f;
    physics.velocity.x = RandFloat(desc.velocityRange.xMin, desc.velocityRange.xMax);
    physics.velocity.y = RandFloat(desc.velocityRange.yMin, desc.velocityRange.yMax);

    reg.emplace<TimedDestroy>(self, RandFloat(desc.lifeRange.min, desc.lifeRange.max));

    return self;
}

void SpawnHitParticles(entt::registry& reg, const Vector2 center) {
    auto desc = ParticleDesc{};
    desc.lifeRange = Range1D{0.1f, 0.3f};

    for (int i = 0; i < RandInt(1, 3); i++) {
        desc.region = {
            PARTICLE_HIT_REGIONS[RandInt(0,
                (sizeof(PARTICLE_HIT_REGIONS)/sizeof(PARTICLE_HIT_REGIONS[0])) - 1)]
        };
        SpawnParticleWithDesc(reg, desc, {
            RandFloat(center.x - 8.0f, center.x + 8.0f),
            RandFloat(center.y - 8.0f, center.y + 8.0f),
        });
    }
}
