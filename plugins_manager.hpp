#ifndef PLUGINS_MANAGER_HPP
#define PLUGINS_MANAGER_HPP

#include <boost/dll.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "plugins_manager_api.hpp"

class plugins_manager : public plugins_manager_api
{
public:
    plugins_manager(boost::asio::io_service &service, const boost::filesystem::path &plugins_path);

    void load();

    source_wptr build_source(const std::string &type, const params_map &params);
    command_wptr build_command(const std::string &type, const params_map &params);

private:
    void register_source_builder(const std::string &type, const source_builder_ptr &builder);
    void register_command_builder(const std::string &type, const command_builder_ptr &builder);

private:
    boost::asio::io_service &_service;
    boost::filesystem::path _plugins_path;
    std::list<std::shared_ptr<boost::dll::shared_library>> _libraries;
    std::map<std::string, source_builder_ptr> _source_builders;
    std::map<std::string, command_builder_ptr> _command_builders;
    std::list<source_ptr> _sources;
    std::list<command_ptr> _commands;
};

#endif // PLUGINS_MANAGER_HPP
