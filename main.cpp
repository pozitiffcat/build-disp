#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "plugins_manager.hpp"
#include "dispatcher.hpp"
#include "tasks_loader.hpp"

int main(int argc, char *argv[])
{
    boost::log::add_console_log();

    boost::asio::io_service service;

    // todo: workspace path get from config
    boost::filesystem::path workspaces_path = boost::filesystem::current_path();

    // todo: plugins path get from config
    boost::filesystem::path plugins_path = boost::filesystem::current_path() / "plugins";

    // todo: tasks file path get from config
    boost::filesystem::path tasks_settings_file_path = boost::filesystem::current_path() / "tasks.json";

    // load plugins
    plugins_manager pmanager(service, plugins_path);
    pmanager.load();

    // read tasks to dispatcher and schedule it
    dispatcher disp;
    tasks_loader tloader(service, workspaces_path, tasks_settings_file_path, pmanager, disp);
    tloader.load();

    return service.run();
}
