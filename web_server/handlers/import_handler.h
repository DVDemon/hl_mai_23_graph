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
#include "../config/config.h"

class ImportHandler : public HTTPRequestHandler
{
public:
    ImportHandler(const std::string &format) : _format(format)
    {
    }

    void import(const std::string &file)
    {
        auto action =
            [file]()
        {
            std::cout << "Importing:" << file << std::endl;
            OpenXLSX::XLDocument doc;
            doc.open(file);

            std::map<std::string, database::Node> nodes;
            std::vector<database::Link> links;

            std::cout << "Sheets count:" << doc.workbook().worksheetCount() << std::endl;
            for (unsigned int wi = 1; wi <= doc.workbook().worksheetCount(); ++wi)
            {
                OpenXLSX::XLWorksheet wks = doc.workbook().sheet(wi);

                auto get_value = [](std::vector<OpenXLSX::XLCellValue> &rv, size_t index)
                {
                    try
                    {
                        std::string val = rv[index].get<std::string>();
                        val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](unsigned char ch)
                                                            { return !std::isspace(ch); }));
                        return rv[index].get<std::string>();
                    }
                    catch (...)
                    {
                        return std::string();
                    }
                };

                std::cout << "Sheet name [" << wks.name() << "]" << std::endl;

                if (Config::get().parser_config().find(wks.name()) != std::end(Config::get().parser_config()))
                {
                    ParserConfig cfg = Config::get().parser_config()[wks.name()];
                    std::cout << "Processing:" << cfg.sheet << std::endl;

                    unsigned long i = 1;
                    for (auto &row : wks.rows())
                    {
                        if (i < 2)
                        {
                            ++i;
                        }
                        else
                        {
                            std::vector<OpenXLSX::XLCellValue> row_vector(row.values());

                            if (!row_vector.empty())
                            {
                                std::string source_node_code = get_value(row_vector, std::stoi(cfg.source_node_code_index));
                                std::string target_node_code = get_value(row_vector, std::stoi(cfg.target_node_code_index));
                                std::string source_node_type = cfg.source_node_type_index.empty() ? cfg.node_type_name : get_value(row_vector, std::stoi(cfg.source_node_type_index));
                                std::string target_node_type = cfg.target_node_type_index.empty() ? cfg.node_type_name : get_value(row_vector, std::stoi(cfg.target_node_type_index));
                                std::string source_node_name = cfg.source_node_name_index.empty() ? "" : get_value(row_vector, std::stoi(cfg.source_node_name_index));
                                std::string target_node_name = cfg.target_node_name_index.empty() ? "" : get_value(row_vector, std::stoi(cfg.target_node_name_index));
                                std::string link_name = cfg.link_name_index.empty() ? cfg.link_type_name : get_value(row_vector, std::stoi(cfg.link_name_index));
                                std::string link_type = cfg.link_type_name;
                                database::Node a, b;
                                database::Link link;

                                a.label() = source_node_type;
                                a.get()["type"] = source_node_type;
                                a.get()["code"] = source_node_code;
                                if (!source_node_name.empty())
                                    a.get()["name"] = source_node_name;
                                nodes[source_node_code] = a;

                                b.label() = target_node_type;
                                b.get()["type"] = target_node_type;
                                b.get()["code"] = target_node_code;
                                if (!target_node_name.empty())
                                    b.get()["name"] = target_node_name;
                                nodes[target_node_code] = b;

                                link.source_node_code() = source_node_code;
                                link.target_node_code() = target_node_code;
                                link.get()["type"] = link_type;
                                link.get()["name"] = link_name;
                                link.label() = link_type;
                                links.push_back(link);
                            }
                        }
                    }
                }

                
                std::cout << "Parsed links:" << links.size() << std::endl;
                std::cout << "Parsed nodes:" << nodes.size() << std::endl;

                neo4j::rest_request::query_nodes({"CREATE CONSTRAINT ON (n) ASSERT (n.code) IS UNIQUE"});
                neo4j::rest_request::query_nodes({"CREATE INDEX FOR (m) ON (m.code)"});

                int i = 0;
                for (auto &[name, node] : nodes)
                {
                    name.size();
                    node.save();
                    ++i;
                    if (i % 100 == 0)
                        std::cout << "." << std::flush;
                }

                i = 0;
                for (auto &l : links)
                {
                    l.save();
                    ++i;
                    if (i % 100 == 0)
                        std::cout << "." << std::flush;
                }
                std::cout << "done" << std::endl;
            };
            doc.close();
            if (std::experimental::filesystem::remove(file))
                    std::cout << "file " << file << " deleted.\n";
        };
        database::ImportTask::get().add(action);
    }
        

        void handleRequest(HTTPServerRequest & request,
                           HTTPServerResponse & response)
        {
            std::ofstream file;

            auto uuid_str = Poco::UUIDGenerator().createRandom().toString();
            uuid_str += ".xls";
            const std::string xlsx_name{uuid_str};
            file.open(xlsx_name, std::ofstream::binary);
            std::istream &is = request.stream();
            Poco::StreamCopier::copyStream(is, file, request.getContentLength());
            Poco::Net::HTMLForm form(request, is);

            import(xlsx_name);

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