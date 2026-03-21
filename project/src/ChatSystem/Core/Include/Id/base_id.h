#ifndef BASE_ID_H
#define BASE_ID_H

#include <string>
#include <type_traits>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

template <typename TDerived>
class BaseId {
    friend TDerived;

public:
    [[nodiscard]] static TDerived Generate();

    [[nodiscard]] static TDerived FromString(const std::string& id);

    [[nodiscard]] const std::string& ToString() const;

    [[nodiscard]] bool IsValid() const;

    bool operator==(const BaseId& other) const = default;

    bool operator<(const BaseId& other) const;

protected:
    explicit BaseId(std::string id) : id_{std::move(id)} {}

private:
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

template <typename TDerived>
TDerived BaseId<TDerived>::FromString(const std::string& id) {
    return TDerived{id};
}

template <typename TDerived>
const std::string& BaseId<TDerived>::ToString() const {
    return id_;
}

template <typename TDerived>
bool BaseId<TDerived>::IsValid() const {
    return !id_.empty();
}

template <typename TDerived>
bool BaseId<TDerived>::operator<(const BaseId& other) const {
    return id_ < other.id_;
}


namespace std {
    
    template <typename TDerived>
    requires std::is_base_of<BaseId<TDerived>, TDerived>
    struct hash {
        size_t operator()(const TDerived& id) const { 
            return hash<std::string>{}(id.ToString());
        }
    };
}

#endif  // BASE_ID_H