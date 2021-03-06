#include "interaction.hpp"

void UpdateInteraction(uptr<Game>& game, entt::registry& reg) {
    auto view = reg.view<Body, Interaction>(entt::exclude<Disabled>);

    for (entt::entity e : view) {
        auto& body = reg.get<Body>(e);
        auto& interaction = reg.get<Interaction>(e);

        auto player_view = reg.view<Player, Body>();
        for (auto player : player_view) {
            auto player_body = reg.get<Body>(player);

            bool hot = CheckCollisionRecs(body, player_body);

            if (!interaction.last_intersection) {
                if (interaction.mode == CALL_WHEN_ENTERED && hot) {
                    interaction.action(e, reg);
                    interaction.last_intersection = true;
                }

                if (interaction.mode == CALL_WHEN_INTERACTED &&
                    hot &&
                    Input::I()->Interact()) { 
                    std::invoke(interaction.action, e, reg);
                    interaction.last_intersection = true;
                }
            }

            if (interaction.last_intersection && !hot) {
                if (interaction.mode == CALL_WHEN_EXITED) {
                    interaction.action(e, reg);
                }
                interaction.last_intersection = false;
            }
        }
    }
}

void DrawInteraction(const uptr<Game>& game, entt::registry& reg) {
    auto view = reg.view<Body, Interaction>();
    auto player_view = reg.view<Player>();

    auto gui = Assets::I()->textures[Textures::TEX_GUI];

    for (entt::entity e : view) {
        auto& body = reg.get<Body>(e);
        auto& interaction = reg.get<Interaction>(e);

        for (auto player : player_view) {
            auto player_body = reg.get<Body>(player);

            bool hot = CheckCollisionRecs(body, player_body);

            if (hot && interaction.icon != ActionIcon::NONE) {
                DrawTexturePro(
                    gui,
                    Rectangle{0, 0, 8, 8},
                    Rectangle{
                        body.center().x - body.width / 2,
                        body.y - 10,
                        8,
                        8
                    },
                    Vector2{0,0},
                    0.0f,
                    WHITE);
            }
        }
    }
}
