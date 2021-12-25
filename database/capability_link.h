#ifndef H_CAPABILITY_LINK
#define H_CAPABILITY_LINK

#include <string>
#include "Poco/JSON/Object.h"

namespace database
{
    class CapabilityLink
    {
    private:
        std::string _node_code;
        std::string _capability_code;
        

    public:
        CapabilityLink();
        CapabilityLink(const std::string& node_code,const std::string& capability_code);


        void save();
        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif // !H_CAPABILITY_LINK
