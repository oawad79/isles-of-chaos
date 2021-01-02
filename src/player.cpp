#include "player.hpp"
#include <iostream>

void UpdateCameraTracking(Body& body, Physics& physics, Camera2D& camera) {
    const auto dt = GetFrameTime();

    constexpr auto xsmooth{20.0f};
    constexpr auto ysmooth{2.0f};

    const auto ideal_x = body.x + body.width / 2 + physics.velocity.x * dt * xsmooth;
    const auto ideal_y = body.y + body.height / 2 + physics.velocity.y * dt * ysmooth;

    camera.target.x = (int)lerp(camera.target.x, ideal_x, xsmooth * dt);
    camera.target.y = (int)lerp(camera.target.y, ideal_y, ysmooth * dt);

    camera.offset.x = CANVAS_WIDTH/2;
    camera.offset.y = CANVAS_HEIGHT/2;

    const auto delta = GetMouseWheelMove();
}

void UpdatePlayer(uptr<Game>& game, entt::registry& reg) {
    auto view = reg.view<Player, Physics, Body>();

    const auto dt = GetFrameTime();

    view.each([dt, &game](auto &player, auto& physics, auto& body){
        const auto [ax, ay] = Input::I()->GetMovementVector();
        physics.velocity.x += ax * 400.0f * dt;

        if (Input::I()->Jump() && physics.on_ground) {
            physics.velocity.y -= 18000.0f * dt;
        }

        if (Input::I()->Ascend() && physics.on_ladder) {
            physics.velocity.y -= 1000.0f * dt;
        }

        if (Input::I()->Descend() && physics.on_ladder) {
            physics.velocity.y += 1000.0f * dt;
        }

        if (Input::I()->Descend() && physics.on_ladder && physics.on_ground) {
            body.y += 100.0f * dt;
        }

        UpdateCameraTracking(body, physics, game->mainCamera);
    });
}
