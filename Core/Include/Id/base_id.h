#ifndef BASE_ID_H
#define BASE_ID_H

#include <string>
#include <type_traits>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/**
 * @brief CRTP base class for strongly-typed UUID identifiers.
 *
 * BaseId provides a reusable implementation for type-safe identifiers
 * (e.g. UserId, SessionId). Each derived type represents a distinct ID type,
 * preventing accidental mixing of identifiers at compile time.
 *
 * The class internally stores IDs as strings but enforces type safety via CRTP.
 *
 * @tparam TDerivedId The concrete ID type inheriting from BaseId.
 */
template <typename TDerivedId>
class BaseId {
  friend TDerivedId;

public:
  /**
   * @brief Generates a new random UUID-based identifier.
   *
   * @return A new instance of the derived ID type.
   */
  [[nodiscard]] static TDerivedId Generate();


  [[nodiscard]] static TDerivedId Parse(const std::string& id);

  /**
   * @brief Creates an ID from an existing string representation.
   *
   * No validation is performed beyond storing the string.
   *
   * @param id String representation of the identifier.
   * @return Constructed derived ID instance.
   */
  [[nodiscard]] static TDerivedId Reconstitute(std::string id);

  /**
   * @brief Returns the string representation of the identifier.
   *
   * @return Constant reference to internal ID string.
   */
  [[nodiscard]] const std::string& ToString() const;

  /**
   * @brief Checks whether the identifier is valid.
   *
   * @return true if the ID is valid, false otherwise.
   */
  [[nodiscard]] bool IsValid() const;

  /**
   * @brief Equality comparison operator.
   */
  bool operator==(const BaseId& other) const = default;

  /**
   * @brief Lexicographical comparison operator.
   */
  bool operator<(const BaseId& other) const;

  /**
   * @brief Hash functor for using IDs in unordered containers.
   */
  struct Hasher {
  size_t operator()(const TDerivedId& id) const noexcept;
  };

protected:
  explicit BaseId(std::string&& id) : id_{ std::move(id) } {}

  explicit BaseId(const std::string& id) : id_{ id } {}

private:
  /**
   * @brief Generates a UUID string using Boost UUID.
   */
  static std::string GenerateUuid() {
    static boost::uuids::random_generator generator;
    return boost::uuids::to_string(generator());
  }

  std::string id_;
};

template <typename TDerived>
TDerived BaseId<TDerived>::Generate() {
  return TDerived{GenerateUuid()};
}

template <typename TDerivedId>
TDerivedId BaseId<TDerivedId>::Parse(const std::string& id) {
  try {
    const boost::uuids::string_generator gen;
    gen(id);
    return TDerivedId{id};
  } catch (const std::exception&) {
    throw std::invalid_argument("Invalid UUID format");
  }
}

template <typename TDerived>
TDerived BaseId<TDerived>::Reconstitute(std::string id) {
  return TDerived{std::move(id)};
}

template <typename TDerived>
const std::string& BaseId<TDerived>::ToString() const {
  return id_;
}

template <typename TDerived>
bool BaseId<TDerived>::IsValid() const {
  return !id_.empty();
}

template <typename TDerivedId>
bool BaseId<TDerivedId>::operator<(const BaseId& other) const {
  return id_ < other.id_;
}

template <typename TDerivedId>
size_t BaseId<TDerivedId>::Hasher::operator()(const TDerivedId& id) const noexcept {
  return std::hash<std::string>{}(id.ToString());
}

#endif // BASE_ID_H