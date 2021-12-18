#ifndef H_LINK
#define H_LINK

#include <string>

namespace database
{
    class Link
    {
    private:
        std::string _name;
        int _percent;
        std::string _source_node_code;
        std::string _target_node_code;
        

    public:
        Link();
        Link(const std::string &name, int percent,const std::string& source_node_code,const std::string& target_node_code);

        void save();
        const std::string &name() const;
        const std::string & source_node_code() const;
        const std::string & target_node_code() const;
        int percent() const;
    };
}

#endif // !H_LINK
