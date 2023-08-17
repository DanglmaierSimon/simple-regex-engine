#pragma once

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

class State final {
public:
  virtual ~State() = default;
  explicit State(bool is_end);

  [[nodiscard]] int id() const;

  [[nodiscard]] bool is_end() const;
  void set_is_end(bool is_end);

  std::string to_string() const;

  [[nodiscard]] bool has_epsilon_transitions() const;

  void add_epsilon_transition(const std::shared_ptr<State> &to);

  void add_transition(char symbol, const std::shared_ptr<State> &to);

  [[nodiscard]] std::unordered_set<std::shared_ptr<State>>
  epsilon_transitions() const;

  [[nodiscard]] std::optional<std::shared_ptr<State>>
  transition(char symbol) const;

private:
  std::string to_string_impl(std::string so_far,
                             std::unordered_set<int> seen_states) const;

  int m_id;
  bool m_is_end;
  std::unordered_map<char, std::shared_ptr<State>> m_transitions;
  std::unordered_set<std::shared_ptr<State>> m_epsilon_transitions;
};

[[nodiscard]] inline std::shared_ptr<State> create_state(bool is_end) {
  return std::make_shared<State>(is_end);
}
