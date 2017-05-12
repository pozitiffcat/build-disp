#ifndef TASK_LOADER_HPP
#define TASK_LOADER_HPP

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include "plugins_manager_api.hpp"

class plugins_manager;
class dispatcher;

class tasks_loader
{
public:
    tasks_loader(boost::asio::io_service &service,
                const boost::filesystem::path &workspaces_path,
                const boost::filesystem::path &tasks_settings_file_path,
                plugins_manager &pmanager,
                dispatcher &d);

    void load();

private:
    params_map parse_params(const boost::property_tree::ptree &pt) const;

private:
    boost::asio::io_service &_service;
    boost::filesystem::path _workspaces_path;
    boost::filesystem::path _tasks_settings_file_path;
    plugins_manager &_plugins_manager;
    dispatcher &_dispatcher;
};

#endif // TASK_LOADER_HPP
