#include "particles.hpp"

entt::entity SpawnParticleWithDesc(entt::registry& reg, ParticleDesc& desc, const Vector2 position) {
    auto self = reg.create();
    reg.emplace<Particle>(self);

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
    physics.gravityScale.y = desc.gravityScale;
    physics.velocity.x = RandFloat(desc.velocityRange.xMin, desc.velocityRange.xMax);
    physics.velocity.y = RandFloat(desc.velocityRange.yMin, desc.velocityRange.yMax);

    reg.emplace<TimedDestroy>(self, RandFloat(desc.lifeRange.min, desc.lifeRange.max));

    return self;
}

void SpawnHitParticles(entt::registry& reg, const Vector2 center) {
    auto desc = ParticleDesc{};
    desc.lifeRange = Range1D{0.1f, 0.3f};

    for (int i = 0; i < RandInt(1, 1); i++) {
        desc.region = {
            PARTICLE_HIT_REGIONS[RandInt(0, ARR_LEN(PARTICLE_HIT_REGIONS) - 1)]
        };
        SpawnParticleWithDesc(reg, desc, {
            RandFloat(center.x - 8.0f, center.x + 8.0f),
            RandFloat(center.y - 8.0f, center.y + 8.0f),
        });
    }
}

void SpawnWaterParticles(entt::registry& reg, const Vector2 center, float ySpeed) {
    auto desc = ParticleDesc{};
    desc.lifeRange = Range1D{1.0f, 2.3f};
    desc.velocityRange.yMin = -(ySpeed * 1.5f) * GetFrameTime();
    desc.velocityRange.yMax = -ySpeed* GetFrameTime();
    desc.velocityRange.xMin = -2000 * GetFrameTime();
    desc.velocityRange.xMax =  2000 * GetFrameTime();

    // desc.gravityScale = 1.0f;

    for (int i = 0; i < RandInt(1, 3); i++) {
        desc.region = {
            PARTICLE_WATER_REGIONS[RandInt(0, ARR_LEN(PARTICLE_WATER_REGIONS) - 1)]
        };
        auto p = SpawnParticleWithDesc(reg, desc, {
            RandFloat(center.x - 8.0f, center.x + 8.0f),
            center.y,
        });
        auto& physics = reg.get<Physics>(p);
        physics.friction = 0.002f;
        physics.gravityScale.y = 1.0f;
    }
}
