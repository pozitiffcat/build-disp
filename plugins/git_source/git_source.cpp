#include <boost/process.hpp>
#include <boost/lexical_cast.hpp>

#include "plugins_manager_api.hpp"

class git_source : public source
{
public:
    git_source(boost::asio::io_service &service,
               const std::string &host,
               const std::string &path,
               const std::string &user,
               const std::string &pass,
               const std::string &url)
        : _service(service),
          _host(host),
          _path(path),
          _user(user),
          _pass(pass),
          _url(url)
    {
        if (_url.empty())
        {
            _url = "ssh://" + _user + "@" + _host + ":" + _path;
        }
    }

    void fetch(const boost::filesystem::path &workspace_path, params_map &context_params, source_finish_handler finish_handler)
    {
        namespace bp = boost::process;

        // todo: if windows?

        std::string args ="/bin/bash -c \"sshpass -p '" + _pass + "' git clone " + _url + " .\"";

        _log_buf.consume(_log_buf.size());

        _child = boost::process::child(args, _service, (bp::std_out & bp::std_err) > _log_buf,
                                       bp::on_exit([this, &context_params, finish_handler](auto res, auto err){
            if (res != EXIT_SUCCESS)
            {
                finish_handler(SOURCE_FINISH_FAILURE);
                return;
            }

            this->check_version(context_params, finish_handler);
        }));
    }

    std::string get_log() const
    {
        auto bufs = _log_buf.data();
        size_t buf_size = _log_buf.size();
        return std::move(std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + buf_size));
    }

private:
    void check_version(params_map &context_params, source_finish_handler finish_handler)
    {
        namespace bp = boost::process;

        auto buf_p = std::make_shared<boost::asio::streambuf>();
        boost::asio::streambuf &buf = *(buf_p.get());

        _child = boost::process::child("/bin/bash -c \"git rev-list --all --count\"", _service, (bp::std_out & bp::std_err) > buf,
                                       bp::on_exit([this, &context_params, buf_p, finish_handler](auto res, auto err){
            if (res != EXIT_SUCCESS)
            {
                std::ostream ostream(buf_p.get());
                ostream << "failure fetch revision" << std::endl;
                finish_handler(SOURCE_FINISH_FAILURE);
                return;
            }

            int revision = 0;
            std::istream istream(buf_p.get());
            istream >> revision;

            if (_last_revision != revision)
            {
                context_params.set_param("GIT_REVISION", boost::lexical_cast<std::string>(revision));
                _last_revision = revision;
                finish_handler(SOURCE_FINISH_SUCCESS);
            }
            else
            {
                finish_handler(SOURCE_FINISH_NO_NEED);
            }
        }));
    }

private:
    boost::asio::io_service &_service;
    std::string _host;
    std::string _path;
    std::string _user;
    std::string _pass;
    std::string _url;
    boost::process::child _child;
    boost::asio::streambuf _log_buf;
    int _last_revision = 0;
};

class git_source_builder : public source_builder
{
public:
    source_ptr build(boost::asio::io_service &service, const params_map &params) const
    {
        return std::make_shared<git_source>
        (
            service,
            params.get_param("host"),
            params.get_param("path"),
            params.get_param("user"),
            params.get_param("pass"),
            params.get_param("url")
        );
    }
};

void connect_plugin(plugins_manager_api *api) noexcept
{
    api->register_source_builder("git", std::make_shared<git_source_builder>());
}
