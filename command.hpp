#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>
#include <boost/filesystem.hpp>

#include "params_map.hpp"

typedef std::function<void(bool)> command_finish_handler;

class command
{
public:
    virtual ~command() = default;
    virtual void execute(const boost::filesystem::path &workspace_path, params_map &context_params, command_finish_handler finish_handler) = 0;
    virtual std::string get_log() const = 0;
};

typedef std::shared_ptr<command> command_ptr;
typedef std::weak_ptr<command> command_wptr;

#endif // COMMAND_HPP
