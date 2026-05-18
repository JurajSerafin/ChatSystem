#ifndef QUERY_PARAMS_H
#define QUERY_PARAMS_H

#include <variant>
#include <string>
#include <string_view>
#include <chrono>
#include <cstddef>
#include <vector>

using ParamsVariant = std::variant<
  std::string_view,
  std::string,
  int64_t,
  bool,
  std::chrono::system_clock::time_point,
  std::nullptr_t
>;


class QueryParams {
public:

  QueryParams& BindParam(std::string&& param);

  QueryParams& BindParam(std::string_view param);

  QueryParams& BindParam(int64_t param);

  QueryParams& BindParam(int param);

  QueryParams& BindParam(std::size_t param);

  QueryParams& BindParam(bool param);

  QueryParams& BindParam(std::chrono::system_clock::time_point param);

  QueryParams& BindParam(std::nullptr_t param);


  using iterator = std::vector<ParamsVariant>::iterator;

  using const_iterator = std::vector<ParamsVariant>::const_iterator;

  [[nodiscard]] iterator begin() noexcept;

  [[nodiscard]] iterator end() noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;


private:
  std::vector<ParamsVariant> param_values_;

  template<typename TParam>
  QueryParams& EmplaceBackAndReturnThis(TParam&& param);
};



template<typename TParam>
QueryParams& QueryParams::EmplaceBackAndReturnThis(TParam&& param) {
  param_values_.emplace_back(std::forward<TParam>(param));

  return *this;
}


inline QueryParams& QueryParams::BindParam(std::string&& param) {
  return EmplaceBackAndReturnThis(std::move(param));
}

inline QueryParams& QueryParams::BindParam(std::string_view param) {
  return EmplaceBackAndReturnThis(param);
}

inline QueryParams& QueryParams::BindParam(int64_t param) {
  return EmplaceBackAndReturnThis(param);
}

inline QueryParams& QueryParams::BindParam(int param) {
  return EmplaceBackAndReturnThis(static_cast<int64_t>(param));
}

inline QueryParams& QueryParams::BindParam(std::size_t param) {
  return EmplaceBackAndReturnThis(static_cast<int64_t>(param));
}

inline QueryParams& QueryParams::BindParam(bool param) {
  return EmplaceBackAndReturnThis(param);
}

inline QueryParams& QueryParams::BindParam(std::chrono::system_clock::time_point param) {
  return EmplaceBackAndReturnThis(param);
}

inline QueryParams& QueryParams::BindParam(std::nullptr_t param) {
  return EmplaceBackAndReturnThis(param);
}

inline QueryParams::iterator QueryParams::begin() noexcept {
  return param_values_.begin();
}

inline QueryParams::iterator QueryParams::end() noexcept {
  return param_values_.end();
}

inline QueryParams::const_iterator QueryParams::begin() const noexcept {
  return param_values_.begin();
}

inline QueryParams::const_iterator QueryParams::end() const noexcept {
  return param_values_.end();
}

inline QueryParams::const_iterator QueryParams::cbegin() const noexcept {
  return param_values_.cbegin();
}

inline QueryParams::const_iterator QueryParams::cend() const noexcept {
  return param_values_.cend();
}


#endif // QUERY_PARAMS_H