#include <boost/log/trivial.hpp>

#include "task.hpp"

task::task(boost::asio::io_service &service, const task_settings &settings, const boost::filesystem::path &workspaces_path)
    : _service(service),
      _settings(settings),
      _workspaces_path(workspaces_path)
{
}

boost::asio::io_service &task::get_service() const
{
    return _service;
}

boost::posix_time::time_duration task::get_duration() const
{
    return _settings.duration;
}

bool task::is_start_at_load() const
{
    return _settings.start_at_load;
}

void task::execute(task_finish_handler finish_handler)
{
    boost::filesystem::path workspace_path = _workspaces_path / ("workspace_" + _settings.name);

    boost::filesystem::remove_all(workspace_path);
    boost::filesystem::create_directories(workspace_path);

    chdir(workspace_path.c_str());
    _context_params.clear();

    if (!_settings.has_source)
    {
        auto it = _settings.command_list.begin();
        if (it != _settings.command_list.end())
            this->execute_command(workspace_path, it, finish_handler);
        else
            finish_handler(true);
    }
    else if (!_settings.src.expired())
    {
        BOOST_LOG_TRIVIAL(info) << "Starting task " << _settings.name;

        _settings.src.lock()->fetch(workspace_path, _context_params, [this, workspace_path, finish_handler](auto status) {
            if (!_settings.src.expired())
            {
                BOOST_LOG_TRIVIAL(info) << "fetch source " << (status ? "success " : "failure ") << _settings.name;

                if (status == SOURCE_FINISH_SUCCESS)
                    BOOST_LOG_TRIVIAL(info) << _settings.src.lock()->get_log();
                else if (status == SOURCE_FINISH_FAILURE)
                    BOOST_LOG_TRIVIAL(warning) << _settings.src.lock()->get_log();
                else
                    BOOST_LOG_TRIVIAL(info) << "No need build";
            }

            if (status == SOURCE_FINISH_FAILURE)
            {
                finish_handler(false);
                return;
            }

            if (status == SOURCE_FINISH_NO_NEED)
            {
                finish_handler(true);
                return;
            }

            auto it = _settings.command_list.begin();
            if (it != _settings.command_list.end())
                this->execute_command(workspace_path, it, finish_handler);
            else
                finish_handler(true);
        });
    }
    else
    {
        finish_handler(false);
    }
}

void task::execute_command(const boost::filesystem::path &workspace_path, std::list<command_wptr>::iterator &it, task_finish_handler finish_handler)
{
    command_wptr cmd = *it;

    if (!cmd.expired())
    {
        cmd.lock()->execute(workspace_path, _context_params, [this, cmd, workspace_path, it, finish_handler](auto success) mutable {
            if (!cmd.expired())
            {
                BOOST_LOG_TRIVIAL(info) << "execute command " << (success ? "success " : "failure ") << _settings.name;

                if (success)
                    BOOST_LOG_TRIVIAL(info) << cmd.lock()->get_log();
                else
                    BOOST_LOG_TRIVIAL(warning) << cmd.lock()->get_log();
            }

            if (!success)
            {
                finish_handler(false);
                return;
            }

            ++it;
            if (it != _settings.command_list.end())
                this->execute_command(workspace_path, it, finish_handler);
            else
                finish_handler(true);
        });
    }
    else
    {
        finish_handler(false);
    }
}
