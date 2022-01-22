#ifndef H_LINK
#define H_LINK

#include <string>
#include <map>
#include "Poco/JSON/Object.h"
#include "node.h"

namespace database
{
    enum class Link_type
    {
        in,
        out,
        both
    };
    class Link
    {
    private:
        std::string _label;
        std::map<std::string, std::string> _map;
        std::string _source_node_code;
        std::string _target_node_code;

    public:
        Link();

        static void load_node_links(const std::string &code, std::vector<Link> &result_links, std::vector<Node> &result_nodes,Link_type link_type);

        void save();
        Poco::JSON::Object::Ptr toJSON() const;

        std::string &label();
        std::map<std::string, std::string> &get();
        std::string &source_node_code();
        std::string &target_node_code();
    };
}

#endif // !H_LINK
