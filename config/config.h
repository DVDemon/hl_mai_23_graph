#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

struct ParserConfig{
        std::string sheet;
        std::string link_type_name;
        std::string node_type_name;
        std::string source_node_code_index;
        std::string target_node_code_index;
        std::string source_node_type_index;
        std::string target_node_type_index;
        std::string source_node_name_index;
        std::string target_node_name_index;
        std::string link_name_index;

        std::map<int,std::string> names;

        void from_name(std::string&,std::string&);
};

class  Config{
    private:
        Config();
        std::string _host;
        std::string _port;
        std::string _login;
        std::string _password;
        std::string _database;

        std::map<std::string,ParserConfig> _parser_config;

    public:
        static Config& get();

        std::string& port();
        std::string& host();
        std::string& login();
        std::string& password();
        std::string& database();

        std::map<std::string,ParserConfig> &parser_config();

        const std::string& get_port() const ;
        const std::string& get_host() const ;
        const std::string& get_login() const ;
        const std::string& get_password() const ;
        const std::string& get_database() const ;
};

#endif