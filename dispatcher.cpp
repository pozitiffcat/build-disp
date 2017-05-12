#include "dispatcher.hpp"

void dispatcher::schedule_task(const task_ptr &t)
{
    if (t->is_start_at_load())
        this->execute_task_if_available(t);

    schedule_task_timer(t);
}

void dispatcher::execute_task_if_available(const task_ptr &t)
{
    if (_progress)
    {
        if (!task_is_in_queue(t))
            _tasks.push_back(t);

        return;
    }

    _progress = true;

    t->execute([this, t](auto success){
        _progress = false;
        this->take_from_queue_if_available(t);
        this->execute_next_task_if_available();
    });
}

void dispatcher::execute_next_task_if_available()
{
    if (!_tasks.empty())
        execute_task_if_available(_tasks.front());
}

void dispatcher::take_from_queue_if_available(const task_ptr &t)
{
    _tasks.erase(std::remove(_tasks.begin(), _tasks.end(), t), _tasks.end());
}

bool dispatcher::task_is_in_queue(const task_ptr &t) const
{
    return std::find(_tasks.begin(), _tasks.end(), t) != _tasks.end();
}

void dispatcher::schedule_task_timer(const task_ptr &t)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(t->get_service(), t->get_duration());
    timer->async_wait([timer, this, t](auto err){
        this->execute_task_if_available(t);
        this->schedule_task_timer(t);
    });
}
