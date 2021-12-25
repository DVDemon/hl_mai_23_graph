#ifndef H_TECH
#define H_TECH

#include <string>

#include "Poco/JSON/Object.h"
namespace database
{
    class Tech
    {
        private:
            std::string _code;
            std::string _description;
            std::string _link;
            std::string _status;
            std::string _type;
            std::string _license;
            std::string _license_type;
        public:
        Tech();
        Tech(const std::string &code,
            const std::string &description,
            const std::string &link,
            const std::string &status,
            const std::string &type,
            const std::string &license,
            const std::string &license_type);

        void save();
        Poco::JSON::Object::Ptr toJSON() const;
    };
}
#endif // H_TECH