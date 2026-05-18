#ifndef DOMAIN_CLASS_TYPE_VARIANT_FACTORY_H
#define DOMAIN_CLASS_TYPE_VARIANT_FACTORY_H

#include <variant>
#include <optional>
#include <format>
#include <stdexcept>
#include <string_view>
#include <concepts>


template<typename T>
concept HasTypeString = requires {
  { T::TypeString() } -> std::convertible_to<std::string_view>;
};

/**
 * @brief Forward declaration of the generic variant factory.
 * @tparam TVariant The std::variant type to be constructed.
 */
template<typename TVariant>
class DomainClassTypeVariantFactory;

/**
 * @brief A generic template factory for instantiating a specific type within a std::variant at runtime.
 * It matches a runtime string identifier against each type's static string identifier.
 * * * @note: Every type `T` included in the `std::variant<TTypes...>` has to implement
 * a public static method: `static std::string_view TypeString()`.
 * * @tparam TTypes The parameter pack of underlying types contained within the std::variant.
 */
template<typename ... TTypes>
  requires (HasTypeString<TTypes> && ...)
class DomainClassTypeVariantFactory<std::variant<TTypes...>> {
public:
  /**
   * @brief Dynamically constructs the correct variant alternative based on a runtime string identifier.
   * * @tparam TArgs The parameter pack of arguments to forward to the underlying type's constructor.
   * @param typeStr The runtime string identifier (e.g., "TEXT", "IMAGE") used to resolve the correct type.
   * @param args The arguments perfectly forwarded to the constructor of the matched type.
   * @return A std::variant containing the successfully constructed domain object.
   * @throws std::invalid_argument If the provided `typeStr` does not match any `TypeString()` of the variant's types.
   */
  template<typename ... TArgs>
  [[nodiscard]] static std::variant<TTypes...> Create(std::string_view typeStr, TArgs&&... args) {
    std::optional<std::variant<TTypes...>> domain_class_type_variant;

    // Uses a C++17 fold expression over the comma operator to evaluate the type condition 
    // for every type in the variant pack until a match is found.
    const bool type_exists = (
      (TTypes::TypeString() == typeStr
        ? (domain_class_type_variant = TTypes{ std::forward<TArgs>(args)... }, true)
        : false
        ) || ...
      );

    if (!type_exists) {
      throw std::invalid_argument(
        std::format("Unable to recognize domain class type: '{}'", typeStr)
      );
    }

    return std::move(*domain_class_type_variant);
  }
};

#endif // DOMAIN_CLASS_TYPE_VARIANT_FACTORY_H