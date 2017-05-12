#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <deque>
#include "task.hpp"

class dispatcher
{
public:
    void schedule_task(const task_ptr &t);

private:
    void execute_task_if_available(const task_ptr &t);
    void execute_next_task_if_available();
    void take_from_queue_if_available(const task_ptr &t);
    bool task_is_in_queue(const task_ptr &t) const;

private:
    void schedule_task_timer(const task_ptr &t);

private:
    bool _progress = false;
    std::deque<task_ptr> _tasks;
};

#endif // DISPATCHER_HPP
