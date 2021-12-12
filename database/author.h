#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database
{
    class Author{
        private:
            std::string _login;
            std::string _first_name;
            std::string _last_name;
            std::string _email;
            std::string _title;

        public:

            static Author fromJSON(const std::string & str);
            static void preload(const std::string & str);

            const std::string &get_login() const;
            const std::string &get_first_name() const;
            const std::string &get_last_name() const;
            const std::string &get_email() const;
            const std::string &get_title() const;

            std::string &login();
            std::string &first_name();
            std::string &last_name();
            std::string &email();
            std::string &title();

            static void init();
            static Author              read_by_login(const std::string & login);
            
            static std::vector<Author> read_all();
            static std::vector<Author> search(std::string first_name,std::string last_name);
            void   save_to_neo4j();

            std::vector<Author>        read_all_friends();
            void add_friend(const std::string &login);
            void remove_friend(const std::string &login);

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif