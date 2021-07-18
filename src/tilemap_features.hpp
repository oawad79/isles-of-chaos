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
  Barrier,
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

inline Feature MakeFeature(FeatureType which, Vector2 pos, Vector2 size) {
  auto self = Feature { };
  self.type = which;
  self.x = pos.x;
  self.y = pos.y;
  self.width = size.x;
  self.height = size.y;
  return self;
}

#endif//ISLESOFCHAOS_TILEMAP_FEATURES_HPP
