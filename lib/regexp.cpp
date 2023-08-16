
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "state.h"

using namespace std;

struct NFA {
  std::shared_ptr<State> start;
  std::shared_ptr<State> end;
};

void print(const std::shared_ptr<State> &state) {
  std::cout << state->to_string() << std::endl;
}

void print(const NFA &nfa) {
  std::cout << "@startuml test" << std::endl
            << "hide empty description" << std::endl;

  std::cout << "[*] --> State" << std::to_string(nfa.start->id()) << std::endl;

  print(nfa.start);

  std::cout << "State" << std::to_string(nfa.end->id()) << " --> [*]"
            << std::endl;

  std::cout << "@enduml" << std::endl;
}

NFA from_epsilon() {
  auto start = create_state(false);
  auto end = create_state(true);

  start->add_epsilon_transition(end);
  return NFA{start, end};
}

NFA from_symbol(char symbol) {
  auto start = create_state(false);
  auto end = create_state(true);

  start->add_transition(symbol, end);
  return NFA{start, end};
}

// ab : a and b
NFA nfa_concat(const NFA &first, const NFA &second) {
  first.end->add_epsilon_transition(second.start);
  first.end->set_is_end(false);
  return NFA{first.start, second.end};
}

// a|b : a or b
NFA nfa_union(const NFA &first, const NFA &second) {
  auto start = create_state(false);
  start->add_epsilon_transition(first.start);
  start->add_epsilon_transition(second.start);

  auto end = create_state(true);
  first.end->add_epsilon_transition(end);
  first.end->set_is_end(false);
  second.end->add_epsilon_transition(end);
  second.end->set_is_end(false);

  return NFA{start, end};
}

// a* : one or more
NFA nfa_closure(const NFA &nfa) {
  auto start = create_state(false);
  auto end = create_state(true);

  start->add_epsilon_transition(end);
  start->add_epsilon_transition(nfa.start);

  nfa.end->add_epsilon_transition(end);
  nfa.end->add_epsilon_transition(nfa.start);
  nfa.end->set_is_end(false);

  return NFA{start, end};
}

// a? : zero or one
NFA zero_or_one(const NFA &nfa) {
  auto start = create_state(false);
  auto end = create_state(true);

  // eps from start to end
  start->add_epsilon_transition(end);

  // eps from start to nfa.start
  start->add_epsilon_transition(nfa.start);

  // eps from nfa.end to end
  nfa.end->add_epsilon_transition(end);

  // set nfa.end.is_end to false
  nfa.end->set_is_end(false);

  return NFA{start, end};
}

// a+ : one or more
NFA one_or_more(const NFA &nfa) {
  auto start = create_state(false);
  auto end = create_state(true);

  assert(nfa.end->is_end());

  // eps from start to nfa.start
  start->add_epsilon_transition(nfa.start);

  // set nfa.end.is_end to false
  nfa.end->set_is_end(false);

  // eps from nfa.end to end
  nfa.end->add_epsilon_transition(end);

  // eps from end to nfa.start
  nfa.end->add_epsilon_transition(nfa.start);

  auto new_ = NFA{start, end};

  assert(!new_.start->is_end());
  assert(new_.end->is_end());
  return new_;
}

NFA to_nfa(const std::string &post_fix_expr) {
  if (post_fix_expr.empty()) {
    return from_epsilon();
  }

  std::stack<NFA> stack;

  for (auto token : post_fix_expr) {
    switch (token) {
    case '*': {
      assert(!stack.empty());
      auto top = stack.top();
      stack.pop();
      stack.push(nfa_closure(top));
      break;
    }
    case '|': {
      assert(stack.size() >= 2);
      auto right = stack.top();
      stack.pop();
      auto left = stack.top();
      stack.pop();
      stack.push(nfa_union(left, right));
      break;
    }
    case '.': {
      assert(stack.size() >= 2);
      auto right = stack.top();
      stack.pop();
      auto left = stack.top();
      stack.pop();
      stack.push(nfa_concat(left, right));
      break;
    }
    case '?': {
      assert(!stack.empty());
      auto top = stack.top();
      stack.pop();
      stack.push(zero_or_one(top));
      break;
    }
    case '+': {
      assert(!stack.empty());
      auto top = stack.top();
      stack.pop();
      stack.push(one_or_more(top));
      break;
    }
    default: {
      stack.push(from_symbol(token));
    }
    }
  }

  assert(stack.size() == 1);
  return stack.top();
}

void add_next_state(const std::shared_ptr<State> &state,
                    std::vector<std::shared_ptr<State>> &next_states,
                    std::vector<std::shared_ptr<State>> visited) {

  assert(state != nullptr);

  if (state->has_epsilon_transitions()) {
    for (const auto &st : state->epsilon_transitions()) {

      assert(st != nullptr);

      // value NOT found
      if (find(cbegin(visited), cend(visited), st) == cend(visited)) {
        visited.push_back(st);
        add_next_state(st, next_states, visited);
      }
    }
  } else {
    next_states.push_back(state);
  }
}

bool search(const NFA &nfa, const std::string &word) {
  std::vector<std::shared_ptr<State>> current_states;
  add_next_state(nfa.start, current_states, {});

  for (auto symbol : word) {
    std::vector<std::shared_ptr<State>> next_states;

    for (const auto &state : current_states) {
      auto next_state = state->transition(symbol);

      if (next_state.has_value()) {
        add_next_state(*next_state, next_states, {});
      }
    }

    current_states = next_states;
  }

  return find_if(cbegin(current_states), cend(current_states),
                 [](const std::shared_ptr<State> &state) {
                   assert(state != nullptr);
                   return state->is_end();
                 }) != cend(current_states);
}

std::string insert_explicit_concat_operator(const std::string &regexp) {
  std::string output;

  for (uint i = 0; i < regexp.length(); i++) {
    auto token = regexp[i];
    output += token;

    if (token == '(' || token == '|') {
      continue;
    }

    if (i < regexp.length() - 1) {
      auto lookahead = regexp[i + 1];

      if (lookahead == '*' || lookahead == '?' || lookahead == '+' ||
          lookahead == '|' || lookahead == ')') {
        continue;
      }

      output += '.';
    }
  }

  return output;
}

constexpr int operator_precedence(char op) {
  switch (op) {
  case '|':
    return 0;
  case '.':
    return 1;
  case '?':
  case '*':
  case '+':
    return 2;
  default:
    return -1;
  }
}

std::string to_postfix(const std::string &post_fix_expr) {
  std::string output;
  std::stack<char> operator_stack;

  for (auto token : post_fix_expr) {
    if (token == '.' || token == '|' || token == '*' || token == '?' ||
        token == '+') {
      while (!operator_stack.empty() && operator_stack.top() != '(' &&
             (operator_precedence(operator_stack.top()) >=
              operator_precedence(token))) {
        output += operator_stack.top();
        operator_stack.pop();
      }

      operator_stack.push(token);
    } else if (token == '(' || token == ')') {
      if (token == '(') {
        operator_stack.push(token);
      } else {
        while (!operator_stack.empty() && operator_stack.top() != '(') {
          output += operator_stack.top();
          operator_stack.pop();
        }

        operator_stack.pop();
      }
    } else {
      output += token;
    }
  }

  while (!operator_stack.empty()) {
    output += operator_stack.top();
    operator_stack.pop();
  }

  return output;
}

std::function<bool(std::string)> create_matcher(const std::string &regexp) {
  const auto with_concat = insert_explicit_concat_operator(regexp);

  const auto post_fix_expr = to_postfix(with_concat);
  const auto nfa = to_nfa(post_fix_expr);

  print(nfa);

  return [nfa](const std::string &word) { return search(nfa, word); };
}
