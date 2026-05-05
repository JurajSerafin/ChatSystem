#ifndef I_PROTOCOL_HANDLER_H
#define I_PROTOCOL_HANDLER_H

#include <string>

#include <Request/request.h>
#include <Response/response.h>

class IProtocolHandler {
public:
    virtual std::string ProtocolName() const = 0;
    virtual Response Handle(const Request& request) = 0;
    virtual ~IProtocolHandler() = default;
};

#endif // I_PROTOCOL_HANDLER_H