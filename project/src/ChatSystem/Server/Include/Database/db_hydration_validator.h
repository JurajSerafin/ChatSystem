#ifndef DB_HYDRATION_VALIDATOR_H
#define DB_HYDRATION_VALIDATOR_H

#include <User/user_params.h>

template<typename TParams>
struct DbHydrationValidator {
  [[nodiscard]] constexpr auto Validate(const TParams&) const noexcept;
};

template <typename TParams>
constexpr auto DbHydrationValidator<TParams>::Validate(const TParams&) const noexcept {
    constexpr struct DummyValidationResult {
        [[nodiscard]] constexpr bool Ok() const noexcept {
            return true;
        }
    } valid_dummy_result;

    return valid_dummy_result;
}

#endif  // DB_HYDRATION_VALIDATOR_H