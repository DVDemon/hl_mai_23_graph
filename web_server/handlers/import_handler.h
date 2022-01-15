#ifndef IMPORTHANDLER_H
#define IMPORTHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Net/HTMLForm.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <OpenXLSX/OpenXLSX.hpp>

#include "../../database/node.h"
#include "../../database/link.h"
#include "../neo4j/rest_client.h"

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include <experimental/filesystem>
#include "../database/import_task.h"
class ImportHandler : public HTTPRequestHandler
{
public:
    ImportHandler(const std::string &format) : _format(format)
    {
    }

    void import_node_links(const std::string &file)
    {
        database::ImportTask::get().add([file]()
                                        {
        std::cout << "Starting to process nodes:" << file << std::endl;
        OpenXLSX::XLDocument doc;
        doc.open(file);
        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet("Список");

        std::cout << "Worksheet opened" << std::endl;

        unsigned long i = 1;

        std::map<std::string, database::Node> nodes;
        std::vector<database::Link> links;

        for (auto &row : wks.rows())
        {
            if (i < 2)
            {
                ++i;
            }
            else
            {
                std::vector<OpenXLSX::XLCellValue> row_vector(row.values());

                std::string source_type = row_vector[0].get<std::string>();
                std::string source_code = row_vector[1].get<std::string>();
                std::string source_name = row_vector[2].get<std::string>();
                std::string target_type = row_vector[3].get<std::string>();
                std::string target_code = row_vector[4].get<std::string>();
                std::string target_name = row_vector[5].get<std::string>();
                //float link_percent = row_vector[7].get<float>();
                //std::cout<< "link percent:" << link_percent << std::endl;
                std::string link_name = row_vector[13].get<std::string>();

                database::Node a,b;
                a.label() = source_type;
                a.get()["type"] = source_type;
                a.get()["code"] = source_code;
                a.get()["name"] = source_name;

                nodes[source_code] = a;

                b.label() = target_type;
                b.get()["type"] = target_type;
                b.get()["code"] = target_code;
                b.get()["name"] = target_name;

                nodes[target_code] = b;

                database::Link link;
                link.source_node_code()= source_code;
                link.target_node_code()= target_code;
                link.get()["type"] = link_name;
                link.label() = link_name;
                links.push_back(link);

                ++i;
                if (i % 100 == 0)
                    std::cout << "." << std::flush;
            }
        }

        doc.close();
        std::cout << std::endl
                  << "Links processed:" << links.size() << std::endl;
        std::cout << "Nodes  created:" << nodes.size() << std::endl;

        //neo4j::rest_request::query_nodes({"MATCH (n) DETACH DELETE n"});
        neo4j::rest_request::query_nodes({"CREATE CONSTRAINT ON (n) ASSERT (n.code) IS UNIQUE"});
        neo4j::rest_request::query_nodes({"CREATE INDEX FOR (m) ON (m.code)"});

        std::cout << "Neo4j cleared" << std::endl;
        i = 0;
        for (auto &[name, node] : nodes)
        {
            name.size();
            node.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Nodes  saved" << std::endl;

        i = 0;
        for (auto &l : links)
        {
            l.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Links  saved" << std::endl; 
        
        if (std::experimental::filesystem::remove(file))
            std::cout << "file " << file << " deleted.\n"; });
    }

    void import_capabilities(const std::string &file)
    {
        database::ImportTask::get().add([file]()
                                        {
        std::cout << "Starting to process capabilities:" << file << std::endl;
        OpenXLSX::XLDocument doc;
        doc.open(file);
        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet("Позиционирование");

        std::cout << "Worksheet opened" << std::endl;

        unsigned long i = 1;

        std::map<std::string, database::Node> nodes;
        std::vector<database::Link> links;

        
        for (auto &row : wks.rows())
        {
            if (i < 2)
            {
                ++i;
            }
            else
            {
                std::vector<OpenXLSX::XLCellValue> row_vector(row.values());

                if(!row_vector.empty()){
                    std::string code;
                    std::string name;
                    std::string owner;
                    std::string node_code;
                    
                    
                    code = row_vector[2].get<std::string>();
                    name = row_vector[3].get<std::string>();
                    owner = row_vector[5].get<std::string>();
                    node_code = row_vector[7].get<std::string>();

                    database::Node a;
                    a.label() = "Capability";
                    a.get()["type"] = "Capability";
                    a.get()["code"] = code;
                    a.get()["name"] = name;
                    a.get()["owner"] = owner;
                    nodes[code] = a;
                    database::Link link;
                    link.label() = "CAPABILITY_LINK";
                    link.get()["type"]= "CAPABILITY_LINK";
                    link.source_node_code() = code;
                    link.target_node_code() = node_code;
                    links.push_back(link);
                }

                ++i;
                if (i % 100 == 0)
                        std::cout << "." << std::flush;
            }
        }

        doc.close();
        std::cout << std::endl
                  << "Links processed:" << links.size() << std::endl;
        std::cout << "Capabilities  created:" << nodes.size() << std::endl;

        i = 0;
        for (auto &[name, node] : nodes)
        {
            name.size();
            node.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Nodes  saved" << std::endl;

        i = 0;
        for (auto &l : links)
        {
            l.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Links  saved" << std::endl; 
        
        if (std::experimental::filesystem::remove(file))
            std::cout << "file " << file << " deleted.\n"; });
    }

    void import_tech(const std::string &file)
    {
        database::ImportTask::get().add([file]()
                                        {
        std::cout << "Starting to process tech:" << file << std::endl;
        OpenXLSX::XLDocument doc;
        doc.open(file);
        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet("Анкета технологического стека");

        std::cout << "Worksheet opened" << std::endl;

        unsigned long i = 1;

        std::map<std::string, database::Node> nodes;
        std::vector<database::Link> links;

        
        for (auto &row : wks.rows())
        {
            if (i < 2)
            {
                ++i;
            }
            else
            {
                std::vector<OpenXLSX::XLCellValue> row_vector(row.values());

                if(!row_vector.empty()){
                    std::string code= row_vector[8].get<std::string>();
                    std::string node_code= row_vector[0].get<std::string>();
                    std::string description= row_vector[9].get<std::string>();
                    std::string link= row_vector[10].get<std::string>();
                    std::string status= row_vector[12].get<std::string>();
                    std::string type= row_vector[13].get<std::string>();
                    std::string license= row_vector[15].get<std::string>();
                    std::string license_type= row_vector[16].get<std::string>();
                    
                    database::Node a;
                    a.label() = "Tech";
                    a.get()["code"]=code;
                    a.get()["description"]=description;
                    a.get()["link"]=link;
                    a.get()["status"]=status;
                    a.get()["type"]=type;
                    a.get()["license"]=license;
                    a.get()["license_type"]=license_type;

                    nodes[code] = a;

                    database::Link l;
                    l.label() ="TECH_LINK";
                    l.source_node_code() = code;
                    l.target_node_code() = node_code;
                    l.get()["type"]= "TECH_LINK";
                    links.push_back(l);
                }

                ++i;
                if (i % 100 == 0)
                        std::cout << "." << std::flush;
            }
        }

        doc.close();
        std::cout << std::endl
                  << "Links processed:" << links.size() << std::endl;
        std::cout << "Capabilities  created:" << nodes.size() << std::endl;

        i = 0;
        for (auto &[name, node] : nodes)
        {
            name.size();
            node.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Nodes  saved" << std::endl;

        i = 0;
        for (auto &l : links)
        {
            l.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Links  saved" << std::endl; 
        
        if (std::experimental::filesystem::remove(file))
            std::cout << "file " << file << " deleted.\n"; });
    }


    void import_sequence(const std::string &file,const std::string &tag)
    {
        database::ImportTask::get().add([file,tag]()
                                        {
        std::cout << "Starting to process sequence:" << file << std::endl;
        OpenXLSX::XLDocument doc;
        doc.open(file);
        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet("Sheet1");
        unsigned long i = 1;

        std::map<std::string, database::Node> nodes;
        std::vector<database::Link> links;

        auto get_value =[](std::vector<OpenXLSX::XLCellValue>&rv,size_t index){
            try{
                std::string val = rv[index].get<std::string>();
                val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](unsigned char ch) {
                        return !std::isspace(ch);
                    }));
                return rv[index].get<std::string>();
            }catch(...){
                return std::string();
            }
        };
        
        for (auto &row : wks.rows())
        {
            if (i < 2)
            {
                ++i;
            }
            else
            {
                std::vector<OpenXLSX::XLCellValue> row_vector(row.values());

                if(!row_vector.empty()){

                    std::string source_node_code= get_value(row_vector,0);
                    std::string target_node_code= get_value(row_vector,2);
                    std::string source_node_name= get_value(row_vector,1);
                    std::string target_node_name= get_value(row_vector,3);
                    std::string api= get_value(row_vector,4);
                    std::string diagram= get_value(row_vector,5);
                    
                    if(!source_node_code.empty()){
                        database::Node a;
                        a.label()="MODULE";
                        a.get()["code"]=source_node_code;
                        a.get()["description"]=source_node_name;
                        if(!tag.empty()) a.get()["tag"] = tag;
                        a.get()["diagram"] = diagram;
                        nodes[source_node_code] = a;
                    }

                    if(!target_node_code.empty()){
                        database::Node b;
                        b.label()="MODULE";
                        b.get()["code"]=target_node_code;
                        b.get()["description"]=source_node_name;
                        if(!tag.empty())  b.get()["tag"] = tag;
                        b.get()["diagram"] = diagram;
                        nodes[target_node_code] = b;
                    }

                    if(!target_node_code.empty()&&!source_node_code.empty()){
                        database::Link l;
                        l.label()="sequence_call";
                        l.source_node_code() = source_node_code;
                        l.target_node_code() = target_node_code;
                        if(!tag.empty()) l.get()["tag"] = tag;
                        l.get()["api"]= api;
                        l.get()["diagram"] = diagram;
                        links.push_back(l);
                    }
                    

                    
                }

                ++i;
                if (i % 100 == 0)
                        std::cout << "." << std::flush;
            }
        }

        doc.close();
        std::cout << std::endl
                  << "Links processed:" << links.size() << std::endl;
        std::cout << "Capabilities  created:" << nodes.size() << std::endl;

        //neo4j::rest_request::query_nodes({"CREATE CONSTRAINT ON (n) ASSERT (n.code) IS UNIQUE"});
        neo4j::rest_request::query_nodes({"CREATE INDEX :MODULE(code"});
        i = 0;
        for (auto &[name, node] : nodes)
        {
            name.size();
            
            //Poco::JSON::Stringifier::stringify(node.toJSON(),std::cout);
            //std::cout << std::endl;
            node.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Nodes  saved" << std::endl;

        i = 0;
        for (auto &l : links)
        {
            l.save();
            ++i;
            if (i % 100 == 0)
                std::cout << "." << std::flush;
        }
        std::cout << std::endl
                  << "Links  saved" << std::endl; 
        
        if (std::experimental::filesystem::remove(file))
            std::cout << "file " << file << " deleted.\n"; });
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {

        std::cout << "Upload started" << std::endl;
        std::ofstream file;

        auto uuid_str = Poco::UUIDGenerator().createRandom().toString();
        uuid_str += ".xls";
        const std::string xlsx_name{uuid_str};
        file.open(xlsx_name, std::ofstream::binary);
        std::istream &is = request.stream();
        Poco::StreamCopier::copyStream(is, file, request.getContentLength());
        std::cout << "Document uploaded" << std::endl;
        Poco::Net::HTMLForm form(request, is);

        if (form.has("nodes"))
        {
            import_node_links(xlsx_name);
        }
        else if (form.has("capabilities"))
        {
            import_capabilities(xlsx_name);
        } else if (form.has("tech"))
        {
            import_tech(xlsx_name);
        } else if (form.has("sequence"))
        {
            std::string tag = form.has("tag")?form.get("tag"):"";
            import_sequence(xlsx_name,tag);
        }

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
        std::ostream &ostr = response.send();
        ostr.flush();
    }

private:
    std::string _format;
};
#endif // !IMPORTHANDLER_H