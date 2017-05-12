#ifndef SOURCE_HPP
#define SOURCE_HPP

#include <memory>
#include <boost/filesystem.hpp>

#include "params_map.hpp"

enum source_finish_status
{
    SOURCE_FINISH_FAILURE,
    SOURCE_FINISH_SUCCESS,
    SOURCE_FINISH_NO_NEED,
};

typedef std::function<void(source_finish_status)> source_finish_handler;

class source
{
public:
    virtual ~source() = default;
    virtual void fetch(const boost::filesystem::path &workspace_path, params_map &context_params, source_finish_handler finish_handler) = 0;
    virtual std::string get_log() const = 0;
};

typedef std::shared_ptr<source> source_ptr;
typedef std::weak_ptr<source> source_wptr;

#endif // SOURCE_HPP
