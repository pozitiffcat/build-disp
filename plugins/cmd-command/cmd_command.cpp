#include <boost/process.hpp>
#include "plugins_manager_api.hpp"

class cmd_command : public command
{
public:
    cmd_command(boost::asio::io_service &service, const std::string &command)
        : _service(service),
          _command(command)
    {
    }

    void execute(const boost::filesystem::path &workspace_path, params_map &context_params, command_finish_handler finish_handler)
    {
        namespace bp = boost::process;

        _log_buf.consume(_log_buf.size());

        bp::environment env = boost::this_process::environment();
        const auto &params = context_params.get_map();
        for (const auto &p : params)
            env[p.first] = p.second;

        // todo: if windows?
        _child = boost::process::child("/bin/bash -c \"" + _command + "\"", _service, (bp::std_out & bp::std_err) > _log_buf,
                                       bp::env(env), bp::on_exit([finish_handler](auto res, auto err){
            finish_handler(res == EXIT_SUCCESS ? SOURCE_FINISH_SUCCESS : SOURCE_FINISH_FAILURE);
        }));
    }

    std::string get_log() const
    {
        auto bufs = _log_buf.data();
        size_t buf_size = _log_buf.size();
        return std::move(std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + buf_size));
    }

private:
    boost::asio::io_service &_service;
    std::string _command;
    boost::process::child _child;
    boost::asio::streambuf _log_buf;
};

class cmd_command_builder : public command_builder
{
public:
    command_ptr build(boost::asio::io_service &service, const params_map &params) const
    {
        return std::make_shared<cmd_command>
        (
            service,
            params.get_param("command")
        );
    }
};

void connect_plugin(plugins_manager_api *api) noexcept
{
    api->register_command_builder("cmd", std::make_shared<cmd_command_builder>());
}
