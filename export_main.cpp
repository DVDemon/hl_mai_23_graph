#include <iostream>
#include <string>
#include <experimental/filesystem>
#include <vector>
#include <map>

#include "Poco/JSON/Stringifier.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/JSON.h"

#include <OpenXLSX/OpenXLSX.hpp>

#include "config/config.h"
#include "database/node.h"
#include "database/link.h"

auto main() -> int
{

    // init config
    Config::get().login() = "neo4j";
    Config::get().password() = "stud";
    Config::get().database() = "neo4j";
    Config::get().port() = "7474";
    Config::get().host() = "127.0.0.1";

    std::string node_label{"module"};
    std::string base_dir{"export"};

    // load nodes
    std::vector<database::Node> results = database::Node::by_label(node_label);

    // process nodes
    if (!std::experimental::filesystem::exists(base_dir))
        std::experimental::filesystem::create_directory(base_dir);

    for (database::Node &node : results)
    {
        // create dir
        std::string node_code = node.get()["code"];
        std::string node_name = node.get()["name"];
        
        if (!node_code.empty())
        {
            std::string encoded;
            std::transform(std::begin(node_name),std::end(node_name),std::back_inserter<std::string>(encoded),
                            [](char& c){
                                switch(c){
                                    case '\'': return ' ';
                                    case '\"': return ' ';
                                    case '/':  return '_';
                                    case '\\': return '_';
                                    case ':': return '_';

                                    default:
                                        return c;
                                };
                            });
            std::string node_dir = base_dir + "/" + encoded;
            if (!std::experimental::filesystem::exists(node_dir))
                std::experimental::filesystem::create_directory(node_dir);
            std::cout << node_code << " [" << node_name << "]" << std::endl;

            // create description XLS
            std::string xls_name = node_dir+"/links_"+node_code+".xlsx";
            std::string inbound  = "inbound "+node_code;
            std::string outbound = "outound "+node_code;
            OpenXLSX::XLDocument doc;
            doc.create(xls_name);
            
            auto add_sheet = [&](std::string name,database::Link_type lt){
                doc.workbook().addWorksheet(name);

                std::map<std::string,database::Node> node_map;

                std::vector<database::Link> inbound_links;
                std::vector<database::Node> inbound_nodes;
                database::Link::load_node_links(node_code, inbound_links, inbound_nodes,lt);

                for(auto &n: inbound_nodes)
                    node_map[n.get()["code"]] = n;


                OpenXLSX::XLWorksheet wks = doc.workbook().sheet(name);

                wks.cell(1,1).value() = "Source Code";
                wks.cell(1,2).value() = "Source Name";
                wks.cell(1,3).value() = "Target Code";
                wks.cell(1,4).value() = "Target Name";
                wks.cell(1,5).value() = "Message";

                int i=2;
                for(database::Link &l : inbound_links){
                    wks.cell(i,1).value() = l.source_node_code();
                    wks.cell(i,2).value() = node_map[l.source_node_code()].get()["name"];
                    wks.cell(i,3).value() = l.target_node_code();
                    wks.cell(i,4).value() = node_map[l.target_node_code()].get()["name"];
                    wks.cell(i,5).value() = l.get()["name"];
                    ++i;
                }
            };

            add_sheet(inbound,database::Link_type::in);
            add_sheet(outbound,database::Link_type::out);
            doc.workbook().deleteSheet("Sheet1");
            doc.save();
            doc.close();

            // create inbound diagram
            // create outbound diagram
        }
    }

    return 0;
}