#include "params_map.hpp"

void params_map::set_param(const std::string &name, const std::string &value)
{
    _params[name] = value;
}

std::string params_map::get_param(const std::string &name, const std::string &def) const
{
    auto it = _params.find(name);

    if (it == _params.end())
        return def;

    return it->second;
}

const std::map<std::string, std::string> &params_map::get_map() const
{
    return _params;
}

void params_map::clear()
{
    _params.clear();
}
