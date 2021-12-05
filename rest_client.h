#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <string>
#include <vector>
#include <utility>

namespace rest{
    struct rest_request{
        static Poco::JSON::Object::Ptr get_object(const std::string& url,
                                           const std::vector<std::pair<std::string,std::string>>& params);

        static Poco::JSON::Object::Ptr post_object(const std::string& url,
                                           const std::pair<std::string,std::string> authorization,
                                           const Poco::JSON::Object::Ptr& params);

    };
}
#endif