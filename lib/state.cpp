#include "state.h"
#include <algorithm>

namespace {
int global_id;

std::string eps_to_string(
    const State &from_state,
    const std::unordered_set<std::shared_ptr<State>> &epsilon_tranisitions) {
  std::string out;

  for (const auto &t : epsilon_tranisitions) {
    out += "\n";
    out += ("State" + std::to_string(from_state.id()));
    out += " -> ";
    out += ("State" + std::to_string(t->id()));
    out += " : ";
    out += "eps";
  }

  return out;
}

std::string tr_to_string(
    const State &from_state,
    const std::unordered_map<char, std::shared_ptr<State>> &transitions) {
  std::string out;

  for (const auto &kv : transitions) {
    out += "\n";
    out += ("State" + std::to_string(from_state.id()));
    out += " -> ";
    out += ("State" + std::to_string(kv.second->id()));
    out += " : ";
    out += kv.first;
  }

  return out;
}

} // namespace

State::State(bool is_end) : m_id(global_id++), m_is_end(is_end) {}

int State::id() const { return m_id; }

bool State::is_end() const { return m_is_end; }
void State::set_is_end(bool is_end) { m_is_end = is_end; }

std::string State::to_string() const { return to_string_impl("", {}); };

std::string State::to_string_impl(std::string so_far,
                                  std::unordered_set<int> seen_states) const {
  if (cend(seen_states) !=
      std::find_if(cbegin(seen_states), cend(seen_states),
                   [this](auto id) { return this->id() == id; })) {
    return so_far;
  }

  so_far.append(eps_to_string(*this, m_epsilon_transitions));
  so_far.append("\n");
  so_far.append(tr_to_string(*this, m_transitions));
  so_far.append("\n");

  seen_states.insert(id());

  for (const auto &eps : m_epsilon_transitions) {
    so_far = eps->to_string_impl(so_far, seen_states);
  }

  for (const auto &kv : m_transitions) {
    so_far = kv.second->to_string_impl(so_far, seen_states);
  }

  return so_far;
};

bool State::has_epsilon_transitions() const {
  return !m_epsilon_transitions.empty();
}

void State::add_epsilon_transition(const std::shared_ptr<State> &to) {
  m_epsilon_transitions.insert(to);
}

void State::add_transition(char symbol, const std::shared_ptr<State> &to) {
  m_transitions[symbol] = to;
}

std::unordered_set<std::shared_ptr<State>> State::epsilon_transitions() const {
  return m_epsilon_transitions;
}

std::optional<std::shared_ptr<State>> State::transition(char symbol) const {
  if (m_transitions.count(symbol) == 0) {
    return std::nullopt;
  }

  return m_transitions.at(symbol);
}
