#pragma once

#include "../common/Traits.h"
#include "../component/Component.h"
#include "../component/Element.h"
#include "../reactive/Scope.h"


namespace qui {

class Custom : public Component {
  template <typename TFunction>
  using ArgTypes = traits::tuple_remove_first_if_matches_type_t<
      typename traits::function_traits<TFunction>::ArgTypes, Scope>;

public:
  template <typename TFunction>
  Custom(TFunction &&func, ArgTypes<TFunction> args) {
    if constexpr (traits::tuple_is_first_same_v<
                      typename traits::function_traits<TFunction>::ArgTypes,
                      Scope>) {
      mResult = std::apply(
          func, std::tuple_cat(std::make_tuple(std::ref(mScope)), args));
    } else {
      mResult = std::apply(func, args);
    }
  }

  void build() override;

  View *getView() override;

  Element &getResult();

private:
  Scope mScope;
  Element mResult;
};

template <typename TFunction> class CustomComponentFactory {
  using Traits = traits::function_traits<TFunction>;
  using ArgTypes =
      traits::tuple_remove_first_if_matches_type_t<typename Traits::ArgTypes,
                                                   Scope>;

public:
  explicit CustomComponentFactory(TFunction &&func)
      : mFunc(std::forward<TFunction>(func)) {}

  template <typename... Args> Element operator()(Args &&...args) {
    ArgTypes tuple = std::make_tuple(std::forward<Args>(args)...);

    return std::move(Custom(std::forward<TFunction>(mFunc), tuple));
  }

private:
  TFunction mFunc;
};

template <typename TFunction>
static constexpr CustomComponentFactory<TFunction> component(TFunction &&func) {
  return CustomComponentFactory<TFunction>(std::forward<TFunction>(func));
}

} // namespace qui
