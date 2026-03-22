#ifndef USER_H
#define USER_H

#include <Tags/tag.h>
#include <User/i_user_role.h>
#include <User/user_id.h>
#include <memory>
#include <string>

class User {
private:
    UserId id_;

    Tag tag_;

    std::string login_;

    std::string password_hash_;

    std::string public_key_;

    std::unique_ptr<IUserRole> role_;

    User() = default;

public:
    [[nodiscard]] const UserId& GetId() const;

    [[nodiscard]] const Tag& GetTag() const;

    [[nodiscard]] const std::string& GetLogin() const;

    [[nodiscard]] const std::string& GetPasswordHash() const;

    [[nodiscard]] const std::string& GetPublicKey() const;

    [[nodiscard]] const IUserRole& GetRole() const;

    void SetRole(std::unique_ptr<IUserRole> role);

    [[nodiscard]] bool CanPerform(const UserAction action) const;

    class Builder {
    private:
        friend class User;

        User user_;

        Builder() = default;

    public:
        [[nodiscard]] Builder& WithId(UserId&& id);

        [[nodiscard]] Builder& WithTag(Tag&& tag);

        [[nodiscard]] Builder& WithLogin(std::string&& login);

        [[nodiscard]] Builder& WithPasswordHash(std::string&& hash);

        [[nodiscard]] Builder& WithPublicKey(std::string&& pubKey);

        [[nodiscard]] Builder& WithRole(std::unique_ptr<IUserRole> role);

        [[nodiscard]] User Build();
    };

    [[nodiscard]] static Builder CreateBuilder();
};

// User method definitions

inline const UserId& User::GetId() const {
    return id_;
}

inline const Tag& User::GetTag() const {
    return tag_;
}

inline const std::string& User::GetLogin() const {
    return login_;
}

inline const std::string& User::GetPasswordHash() const {
    return password_hash_;
}

inline const std::string& User::GetPublicKey() const {
    return public_key_;
}

inline const IUserRole& User::GetRole() const {
    return *role_;
}

inline void User::SetRole(std::unique_ptr<IUserRole> role) {
    role_ = std::move(role);
}


// Builder method definitions

inline User::Builder& User::Builder::WithId(UserId&& id) {
    user_.id_ = std::move(id);
    return *this;
}

inline User::Builder& User::Builder::WithTag(Tag&& tag) {
    user_.tag_ = std::move(tag);
    return *this;
}

inline User::Builder& User::Builder::WithLogin(std::string&& login) {
    user_.login_ = std::move(login);
    return *this;
}

inline User::Builder& User::Builder::WithPasswordHash(std::string&& hash) {
    user_.password_hash_ = std::move(hash);
    return *this;
}

inline User::Builder& User::Builder::WithPublicKey(std::string&& pubKey) {
    user_.public_key_ = std::move(pubKey);
    return *this;
}

inline User::Builder& User::Builder::WithRole(std::unique_ptr<IUserRole> role) {
    user_.role_ = std::move(role);
    return *this;
}

inline User User::Builder::Build() {
    return std::move(user_);
}

inline User::Builder User::CreateBuilder() {
    return Builder{};
}

#endif  // USER_H