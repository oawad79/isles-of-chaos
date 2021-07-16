#ifndef ISLESOFCHAOS_TILEMAP_FEATURES_HPP
#define ISLESOFCHAOS_TILEMAP_FEATURES_HPP

enum class FeatureType {
  None,
  Ladder,
  Port,
  Door,
  Room,
  Checkpoint,
  Kill,
  Banner,
  Water,
};

struct Feature : Rectangle {
  Vector2 offset{0, 0};

  FeatureType type{FeatureType::None};
  std::string target{""};
  std::string id{""};

  bool active = false;

  inline auto bounds() const { return Rectangle{
            x + offset.x,
            y + offset.y,
            width,
            height
    }; }

  int timerLoc = -1;
};

#endif//ISLESOFCHAOS_TILEMAP_FEATURES_HPP
