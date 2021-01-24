#ifndef SKYVAULT_PARTICLES_HPP
#define SKYVAULT_PARTICLES_HPP

#include <raylib.h>

#include "utils.hpp"
#include "body.hpp"
#include "sprite.hpp"
#include "timed.hpp"
#include "assets.hpp"

enum class AreaType {
  RECTANGLE,
  CIRCLE,
};

struct Emitter {
  AreaType areaType {AreaType::RECTANGLE};
  Rectangle region;
  float radius;

  float maxTime {0.02f};
  float time {0.0};

  int numPerSpawn{1};
};

struct Particle {};

struct ParticleDesc {
  Rectangle region;
  Range2D velocityRange{0};
  Range1D lifeRange{0};
  float gravityScale{0.0f};
};

constexpr Rectangle PARTICLE_HIT_REGIONS[] = {
  {0, 0, 7, 7},
  {0, 7, 7, 7},
  {7, 0, 9, 9},
  {16, 0, 13, 13},
  {29, 0, 11, 11},
};

constexpr Rectangle PARTICLE_WATER_REGIONS[] = {
  {0, 16, 8, 12},
  {8, 16, 8, 8},
  {0, 28, 4, 4},
  {16, 16, 8, 12},
};

entt::entity SpawnParticleWithDesc(entt::registry& reg, ParticleDesc& desc, const Vector2 position);
void SpawnHitParticles(entt::registry& reg, const Vector2 center);
void SpawnWaterParticles(entt::registry& reg, const Vector2 center, float ySpeed=10000.0f);

#endif // SKYVAULT_PARTICLES_HPP
