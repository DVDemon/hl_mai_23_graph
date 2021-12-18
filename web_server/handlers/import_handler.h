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

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <OpenXLSX/OpenXLSX.hpp>

#include "../../database/node.h"
#include "../../database/link.h"

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

class ImportHandler : public HTTPRequestHandler
{
public:
    ImportHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        // Application& app = Application::instance();
        // app.logger().information("HTML Request from "    + request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
        std::cout << "Upload started" << std::endl;
        std::ofstream file;
        const std::string xlsx_name{"import.xls"};
        file.open(xlsx_name, std::ofstream::binary);
        std::istream &is = request.stream();
        Poco::StreamCopier::copyStream(is, file, request.getContentLength());
        std::ostream &ostr = response.send();
        ostr.flush();

        std::cout << "Document uploaded" << std::endl;
        OpenXLSX::XLDocument doc;
        doc.open(xlsx_name);
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

                nodes[source_code] = database::Node{source_code, source_name, source_type};
                nodes[target_code] = database::Node{target_code, target_name, target_type};

                links.push_back(database::Link(link_name, 0, source_code, target_code));

                ++i;
                if (i % 100 == 0)
                    std::cout << "." << std::flush;
            }
        }

        doc.close();
        std::cout << std::endl
                  << "Links processed:" << links.size() << std::endl;
        std::cout << "Nodes  created:" << nodes.size() << std::endl;

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
    }

private:
    std::string _format;
};
#endif // !IMPORTHANDLER_H