#ifndef ISLESOFCHAOS_STORYBOOK_HPP
#define ISLESOFCHAOS_STORYBOOK_HPP

#include <memory>
#include <mutex>

#include "utils.hpp"

struct StoryEvent {

};

struct Chapter {

};

struct TheStory {

};

struct Storybook {
  static auto* I() {
    std::call_once(Storybook::once, [&](){
      it = std::make_unique<Storybook>();
    });
    return it.get();
  }

  inline Storybook() = default;
  inline ~Storybook() = default;

  Storybook(const Storybook&) = delete;
  Storybook& operator=(const Storybook&) = delete;

  private:
    static std::once_flag once;
    static uptr<Storybook> it;
};

#endif//ISLESOFCHAOS_STORYBOOK_HPP
