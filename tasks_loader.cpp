#include <boost/property_tree/json_parser.hpp>
#include "plugins_manager.hpp"
#include "tasks_loader.hpp"
#include "dispatcher.hpp"
#include "task.hpp"

tasks_loader::tasks_loader(boost::asio::io_service &service, const boost::filesystem::path &workspaces_path, const boost::filesystem::path &tasks_settings_file_path, plugins_manager &pmanager, dispatcher &d)
    : _service(service),
      _workspaces_path(workspaces_path),
      _tasks_settings_file_path(tasks_settings_file_path),
      _plugins_manager(pmanager),
      _dispatcher(d)
{
}

void tasks_loader::load()
{
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(_tasks_settings_file_path.string(), pt);

    for (auto &task_object : pt)
    {
        task_settings settings;
        settings.name = task_object.second.get<std::string>("name");
        settings.start_at_load = task_object.second.get<bool>("start_at_load");
        settings.duration = boost::posix_time::seconds(task_object.second.get<int>("duration"));

        auto source_object = task_object.second.get_child("source", boost::property_tree::ptree());

        if (!source_object.empty())
        {
            auto type_param = source_object.get<std::string>("type");
            settings.src = _plugins_manager.build_source(type_param, parse_params(source_object));
            settings.has_source = true;
        }

        for (auto &command_object : task_object.second.get_child("command_list"))
        {
            auto type_param = command_object.second.get<std::string>("type");
            auto c = _plugins_manager.build_command(type_param, parse_params(command_object.second));
            settings.command_list.push_back(c);
        }

        auto t = std::make_shared<task>(_service, settings, _workspaces_path);
        _dispatcher.schedule_task(t);
    }
}

params_map tasks_loader::parse_params(const boost::property_tree::ptree &pt) const
{
    params_map params;
    for (auto p : pt)
        params.set_param(p.first, p.second.get_value<std::string>());

    return std::move(params);
}
