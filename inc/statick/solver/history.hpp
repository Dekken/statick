#ifndef TICK_SOLVER_HISTORY_HPP_
#define TICK_SOLVER_HISTORY_HPP_

namespace statick {
namespace solver {

template <typename T>
class History {
 public:
  History &save_history(double time, size_t epoch, T *iterate, size_t size) {
    time_history[i] = last_record_time + time;
    epoch_history[i] = last_record_epoch + epoch;
    iterate_history[i] = std::vector<T>(size);
    T *last = iterate_history[i].data();
    for (size_t i = 0; i < size; ++i) last[i] = iterate[i];
    return *this;
  }
  History &operator+=(size_t iterations) { return *this; }
  History &add_time(double time) { return *this; }

  size_t record_every = 10, last_record_epoch = 0, i = 0;
  double last_record_time = 0;
  std::vector<double> time_history;
  std::vector<size_t> epoch_history;
  std::vector<std::vector<T>> iterate_history;

  History(){};
  ~History(){};

  History(History &that) = delete;
  History(const History &that) = delete;
  History(History &&that) = delete;
  History(const History &&that) = delete;
  History &operator=(History &that) = delete;
  History &operator=(History &&that) = delete;
  History &operator=(const History &that) = delete;
  History &operator=(const History &&that) = delete;
};

template <typename T>
class NoHistory {
 public:
  NoHistory &save_history() { return *this; }
  NoHistory operator+=(size_t iterations) { return *this; }
  NoHistory add_time(double time) { return *this; }
};
}  // namespace solver
}  // namespace statick

#endif  // TICK_SOLVER_HISTORY_HPP_