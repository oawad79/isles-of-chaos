#include "timed.hpp"

void UpdateTimed(entt::registry& reg) {
    auto td_view = reg.view<TimedDestroy>(entt::exclude<Disabled>);
    const auto dt = GetFrameTime();

    for (auto ent : td_view) {
        auto& timedDestroy = reg.get<TimedDestroy>(ent);

        if (timedDestroy.lifeTime <= 0.0f) {
            reg.destroy(ent);
        } else {
            timedDestroy.lifeTime -= GetFrameTime();
        }
    }
}
