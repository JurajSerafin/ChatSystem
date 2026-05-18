#ifndef OVERLOAD_H
#define OVERLOAD_H

/**
 * @brief Utility struct for pattern matching with std::variant.
 * * Combines multiple callable objects into a single overloaded
 * functor, making it suitable for in-place visitation via `std::visit`.
 */
template<class ... Ts> struct overload : Ts ... {
  using Ts::operator() ...;
};

/** @brief Explicit template deduction guide. */
template<class ... Ts> overload(Ts ...) -> overload<Ts...>;

#endif // OVERLOAD_H