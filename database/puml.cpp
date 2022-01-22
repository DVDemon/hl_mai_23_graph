#include "puml.h"
#include "Poco/UUIDGenerator.h"


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <experimental/filesystem>

std::string ssystem (const char *command) {
    char tmpname [L_tmpnam];
    std::tmpnam ( tmpname );
    std::string scommand = command;
    std::string cmd = scommand + " >> " + tmpname;
    std::system(cmd.c_str());
    std::ifstream file(tmpname, std::ios::in | std::ios::binary );
    std::string result;
    if (file) {
        while (!file.eof()) result.push_back(file.get())
            ;
        file.close();
    }
    remove(tmpname);
    return result;
}

namespace database
{
    Puml::Puml()
    {
    }

    Puml &Puml::get()
    {
        static Puml _instance;
        return _instance;
    }

    void Puml::wait_for(const std::string & key){
        if(_futures.find(key)!=std::end(_futures)) _futures[key].get();
    }

    std::string Puml::generate_puml(const std::vector<Node> &nodes,
                                    const std::vector<Link> &links)
    {
        const std::string result= Poco::UUIDGenerator().createRandom().toString();

        _futures[result]=std::async(std::launch::async,
                    [nodes,links,result](){
                        
                        const auto escape_string = [](const std::string &str){
                            std::string res;
                            std::transform(std::begin(str),std::end(str),std::back_inserter(res),[](const char c){ return c==','?'_':c;});
                            return res;
                        };
                        // create directory
                        if(!std::experimental::filesystem::exists("puml"))
                        std::experimental::filesystem::create_directory("puml");

                        // create puml
                        std::ofstream   file;
                        std::string     file_name="puml/"+result+".puml";
                        file.open(file_name, std::ios::binary);

                        file    << "@startuml" << std::endl;
                        //file    << "!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Component.puml"  << std::endl;
                        //file    << "LAYOUT_WITH_LEGEND()"  << std::endl;
                        //file    << "title Solution Library"  << std::endl;
                        file << "left to right direction" << std::endl;
                        size_t i=0;
                        for(const Node &n: nodes){

                            std::string type = "Container";
                            auto it = n.get().find("type");
                            if(it != std::end(n.get()) && it->second == "Подразделение") {
                                type="Person";
                            }
                            it = n.get().find("code");                            
                            const std::string code = (it != std::end(n.get())) ? escape_string(it->second) : "code_" + std::to_string(++i);

                            it = n.get().find("name");
                            const std::string name = (it != std::end(n.get())) ? it->second : code;

                            std::string description;
                            for(const auto & [k1,k2]: n.get()) description += k1 +":"+k2+" ";

                            file << "component \"" << name << "\" as " << code << std::endl;
                            // file    << type << "(" << code <<",\"" << name <<"\",\"" << description<< "\")" << std::endl;
                        }

                        std::map<std::string,std::vector<Link>> unique_links;

                        for (const Link &l : links) {
                            std::string key;
                            key = l.source_node_code() + l.target_node_code();
                            if (unique_links.find(key) ==
                                std::end(unique_links))
                                unique_links[key] = {l};
                            else
                                unique_links[key].push_back(l);
                        }

                        for(const auto &[key,arr] : unique_links){
                            std::string source_node_code;
                            std::string target_node_code;
                            std::string name;

                            for(const Link &l: arr){
                                source_node_code = l.source_node_code();
                                target_node_code = l.target_node_code();
                                name += l.get().at("name")+"; ";
                            }
                            file << escape_string(source_node_code) << " --> " << escape_string(target_node_code) << " : " << name << std::endl;
                            //file    << "Rel(" << escape_string(source_node_code) <<"," << escape_string(target_node_code) <<",\"" << name <<"\")" << std::endl;
                        }

                        /*for( Link  l: links){
                            if(!l.source_node_code().empty() && !l.target_node_code().empty())

                            file    << "Rel(" << l.source_node_code() <<"," << l.target_node_code() <<",\"" << l.get()["name"]<<"\")" << std::endl;
                        }*/
                        
                        file    << "@enduml";
                        file.close();

                        // run puml generator
                        std::string command="java -jar plantuml.jar "+file_name;
                        ssystem(command.c_str());
                        std::experimental::filesystem::remove(file_name);
                        
                    });
        return result;
    }
}