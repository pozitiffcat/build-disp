#ifndef PLUGINS_MANAGER_API_HPP
#define PLUGINS_MANAGER_API_HPP

#include <map>
#include <string>
#include <boost/asio.hpp>

#include "source.hpp"
#include "command.hpp"
#include "params_map.hpp"

#define API extern "C" BOOST_SYMBOL_EXPORT

class source_builder
{
public:
    virtual ~source_builder() = default;
    virtual source_ptr build(boost::asio::io_service &service, const params_map &params) const = 0;
};

typedef std::shared_ptr<source_builder> source_builder_ptr;

class command_builder
{
public:
    virtual ~command_builder() = default;
    virtual command_ptr build(boost::asio::io_service &service, const params_map &params) const = 0;
};

typedef std::shared_ptr<command_builder> command_builder_ptr;

class plugins_manager_api
{
public:
    virtual ~plugins_manager_api() = default;
    virtual void register_source_builder(const std::string &type, const source_builder_ptr &builder) = 0;
    virtual void register_command_builder(const std::string &type, const command_builder_ptr &builder) = 0;
};

API void connect_plugin(plugins_manager_api *api) noexcept;

typedef void(connect_plugin_func_ptr)(plugins_manager_api *);

#endif // PLUGINS_MANAGER_API_HPP
