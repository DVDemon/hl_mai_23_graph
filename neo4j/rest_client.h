#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/Dynamic/Var.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

using result_pair_t = std::pair<std::string,std::string>;
using result_pair_collection_t = std::vector<result_pair_t>;

namespace neo4j{



    class rest_request{
        static std::string connection;
        static std::string login;
        static std::string password;


        static Poco::JSON::Object::Ptr post_object(const std::vector<std::string>& params);
        static Poco::Dynamic::Var  get_object(const std::string& url);

        public:
        static const std::string   get_connection();
        static const std::string & get_base_connection();

        static const std::string & get_login();
        static const std::string & get_password();
        static void config(const std::string &host,const std::string& port,const std::string& database,const std::string& user,const std::string& login);
        

        static std::vector<result_pair_collection_t> query_nodes(const std::vector<std::string>& params);
        static std::vector<std::string> query_values(const std::vector<std::string> &params);

    };
}
#endif