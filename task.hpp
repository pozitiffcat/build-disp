#ifndef TASK_HPP
#define TASK_HPP

#include <list>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "source.hpp"
#include "command.hpp"

typedef std::function<void(bool)> task_finish_handler;

struct task_settings
{
    std::string name;
    source_wptr src;
    bool has_source;
    std::list<command_wptr> command_list;
    boost::posix_time::time_duration duration;
    bool start_at_load = false;
};

class task
{
public:
    explicit task(boost::asio::io_service &service, const task_settings &settings, const boost::filesystem::path &workspaces_path);

    boost::asio::io_service &get_service() const;
    boost::posix_time::time_duration get_duration() const;
    bool is_start_at_load() const;

    void execute(task_finish_handler finish_handler);

private:
    void execute_command(const boost::filesystem::path &workspace_path, std::list<command_wptr>::iterator &it, task_finish_handler finish_handler);

private:
    boost::asio::io_service &_service;
    task_settings _settings;
    boost::filesystem::path _workspaces_path;
    params_map _context_params;
};

typedef std::shared_ptr<task> task_ptr;

#endif // TASK_HPP
