#ifndef DOMAIN_CLASS_TYPE_VARIANT_FACTORY_H
#define DOMAIN_CLASS_TYPE_VARIANT_FACTORY_H

#include <variant>
#include <optional>
#include <format>
#include <stdexcept>
#include <string_view>

template<typename TVariant>
class DomainClassTypeVariantFactory;

template<typename ... TTypes>
class DomainClassTypeVariantFactory<std::variant<TTypes...>> {
public:
  template<typename ... TArgs>
  [[nodiscard]] static std::variant<TTypes...> Create(std::string_view typeStr, TArgs&&... args) {
    std::optional<std::variant<TTypes...>> domain_class_type_variant;

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