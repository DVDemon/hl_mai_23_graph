#include "puml.h"
#include "Poco/UUIDGenerator.h"


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>

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

    std::string Puml::generate_puml(std::vector<Node> &nodes,
                                    std::vector<Link> &links)
    {
        std::string result= Poco::UUIDGenerator().createRandom().toString();

        _futures[result]=(std::async(std::launch::async,
                    [nodes,links,result](){
                        // create puml
                        std::ofstream   file;
                        std::string     file_name="puml/"+result+".puml";
                        file.open(file_name, std::ios::binary);

                        file    << "@startuml" << std::endl;
                        file    << "!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Component.puml"  << std::endl;
                        file    << "LAYOUT_WITH_LEGEND()"  << std::endl;
                        file    << "title Solution Library"  << std::endl;
                        size_t i=0;
                        for( Node  n: nodes){

                            std::string type = "Container";
                            if(n.get().find("type")!=std::end(n.get())) {
                                if(n.get()["type"]=="Подразделение") type="Person";
                            }
                            std::string code = "code_"+std::to_string(++i);
                            if(n.get().find("code")!=std::end(n.get())) code = n.get()["code"];

                            std::string name = code;
                            if(n.get().find("name")!=std::end(n.get())) name = n.get()["name"];

                            std::string description;
                            for(auto & [k1,k2]: n.get()) description += k1 +":"+k2+" ";

                            file    << type << "(" << code <<",\"" << name <<"\",\"" << description<< "\")" << std::endl;
                        }


                        for( Link  l: links){
                            if(!l.source_node_code().empty() && !l.target_node_code().empty())

                            file    << "Rel(" << l.source_node_code() <<"," << l.target_node_code() <<",\"Uses\")" << std::endl;
                        }
                        
                        file    << ""  << std::endl;
                        file    << ""  << std::endl;
                        file    << ""  << std::endl;
                        file    << ""  << std::endl;
                        file    << "@enduml";
                        file.close();

                        // run puml generator
                        std::string command="java -jar plantuml.jar "+file_name;
                        ssystem(command.c_str());
                        
                    }));
        return result+".png";
    }
}