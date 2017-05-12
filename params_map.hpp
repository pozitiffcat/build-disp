#ifndef PARAMS_MAP_HPP
#define PARAMS_MAP_HPP

#include <map>
#include <string>

class params_map
{
public:
    void set_param(const std::string &name, const std::string &value);
    std::string get_param(const std::string &name, const std::string &def = std::string()) const;
    const std::map<std::string, std::string> &get_map() const;
    void clear();

private:
    std::map<std::string, std::string> _params;
};

#endif // PARAMS_MAP_HPP
