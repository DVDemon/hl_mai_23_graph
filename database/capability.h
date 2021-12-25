#ifndef H_CAPABILITY
#define H_CAPABILITY

#include <string>

#include "Poco/JSON/Object.h"

namespace database
{
    class Capability
    {
    private:
        std::string _code;
        std::string _name;
        std::string _owner;
    public:
        Capability();
        Capability(const std::string &code, const std::string& name,const std::string& owner);

        void save();
        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif // !H_CAPABILITY
