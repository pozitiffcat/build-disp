#include <boost/log/trivial.hpp>
#include <boost/function.hpp>

#include "plugins_manager.hpp"

plugins_manager::plugins_manager(boost::asio::io_service &service, const boost::filesystem::path &plugins_path)
    : _service(service),
      _plugins_path(plugins_path)
{
}

void plugins_manager::load()
{
    boost::filesystem::directory_iterator dir_it(_plugins_path), dir_end_it;
    _libraries.clear();

    for (; dir_it != dir_end_it; dir_it++)
    {
        boost::filesystem::path it_plugin_path = *dir_it;
        BOOST_LOG_TRIVIAL(debug) << "-- Found file in plugins directory: " << it_plugin_path.filename().string();

        auto lib = std::make_shared<boost::dll::shared_library>(it_plugin_path, boost::dll::load_mode::append_decorations);
        if (!lib->has("connect_plugin"))
        {
            BOOST_LOG_TRIVIAL(debug) << "The file is not a plugin: " << it_plugin_path.filename().string();
        }
        else
        {
            boost::function<connect_plugin_func_ptr> connect_plugin_func = lib->get<connect_plugin_func_ptr>("connect_plugin");
            connect_plugin_func(this);
            _libraries.push_back(lib);
        }
    }
}

source_wptr plugins_manager::build_source(const std::string &type, const params_map &params)
{
    auto src = _source_builders.at(type)->build(_service, params);
    _sources.push_back(src);
    return src;
}

command_wptr plugins_manager::build_command(const std::string &type, const params_map &params)
{
    auto cmd = _command_builders.at(type)->build(_service, params);
    _commands.push_back(cmd);
    return cmd;
}

void plugins_manager::register_source_builder(const std::string &type, const source_builder_ptr &builder)
{
    BOOST_LOG_TRIVIAL(debug) << "Register '" << type << "' source buider";
    _source_builders[type] = builder;
}

void plugins_manager::register_command_builder(const std::string &type, const command_builder_ptr &builder)
{
    BOOST_LOG_TRIVIAL(debug) << "Register '" << type << "' command buider";
    _command_builders[type] = builder;
}
