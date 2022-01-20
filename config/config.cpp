#include "config.h"

#include <experimental/filesystem>
#include <iostream>
#include <fstream>

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"

void ParserConfig::from_name(std::string& name,std::string& value) {
    if(name=="sheet") sheet=value;
    if(name=="link_type_name") link_type_name=value;
    if(name=="node_type_name") node_type_name=value;
    if(name=="source_node_code_index") source_node_code_index=value;
    if(name=="target_node_code_index") target_node_code_index=value;
    if(name=="source_node_type_index") source_node_type_index=value;
    if(name=="target_node_type_index") target_node_type_index=value;
    if(name=="source_node_name_index") source_node_name_index=value;
    if(name=="target_node_name_index") target_node_name_index=value;
    if(name=="link_name_index") link_name_index=value;
}

Config::Config()
{
    const std::string filename {"parser_config.json"};
    if(std::experimental::filesystem::exists(filename)){
        std::ifstream file(filename);
        if(file.is_open()){
            
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(file);
            Poco::JSON::Array::Ptr array = result.extract<Poco::JSON::Array::Ptr>();
            
            for(size_t i=0;i<array->size();++i){
                ParserConfig cfg_item;
                Poco::JSON::Object::Ptr item = array->getObject(i);
                for(auto name: item->getNames()){
                    std::string value = item->getValue<std::string>(name);
                    if(!value.empty()) cfg_item.from_name(name,value);   
                }
                _parser_config[cfg_item.sheet]=cfg_item;
            }

            file.close();
        }

    } else std::cout << "unable to load config:" << filename << std::endl;
}

std::map<std::string,ParserConfig> &Config::parser_config(){
    return _parser_config;
}

Config &Config::get()
{
    static Config _instance;
    return _instance;
}

const std::string &Config::get_port() const
{
    return _port;
}

const std::string &Config::get_host() const
{
    return _host;
}

const std::string &Config::get_login() const
{
    return _login;
}

const std::string &Config::get_password() const
{
    return _password;
}
const std::string &Config::get_database() const
{
    return _database;
}

std::string &Config::port()
{
    return _port;
}

std::string &Config::host()
{
    return _host;
}

std::string &Config::login()
{
    return _login;
}

std::string &Config::password()
{
    return _password;
}

std::string &Config::database()
{
    return _database;
}