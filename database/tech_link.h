#ifndef H_TECH_LINK
#define H_TECH_LINK

#include <string>
#include "Poco/JSON/Object.h"

namespace database
{
    class TechLink
    {
    private:
        std::string _node_code;
        std::string _tech_code;

    public:
        TechLink();
        TechLink(const std::string& node_code,const std::string& tech_code);
        void save();
        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif // !H_TECH_LINK
