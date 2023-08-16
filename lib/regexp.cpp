
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

using namespace std;

static int global_id;

struct State {
  int id;
  bool is_end;
  std::unordered_map<char, std::shared_ptr<State>> transitions;
  std::vector<std::shared_ptr<State>> epsilon_transitions;
};

struct NFA {
  std::shared_ptr<State> start;
  std::shared_ptr<State> end;
};

std::string
to_string(const std::shared_ptr<State> &from_state,
          const std::vector<std::shared_ptr<State>> &epsilon_tranisitions) {
  std::string out;

  for (const auto &t : epsilon_tranisitions) {
    out += "\n";
    out += ("State" + std::to_string(from_state->id));
    out += " -> ";
    out += ("State" + std::to_string(t->id));
    out += " : ";
    out += "eps";
  }

  return out;
}

std::string
to_string(const std::shared_ptr<State> &from_state,
          const std::unordered_map<char, std::shared_ptr<State>> &transitions) {
  std::string out;

  for (const auto &kv : transitions) {
    out += "\n";
    out += ("State" + std::to_string(from_state->id));
    out += " -> ";
    out += ("State" + std::to_string(kv.second->id));
    out += " : ";
    out += kv.first;
  }

  return out;
}

void print(const std::shared_ptr<State> &state, std::vector<int> &seen_states) {
  if (cend(seen_states) !=
      std::find_if(cbegin(seen_states), cend(seen_states),
                   [&state](auto id) { return state->id == id; })) {
    return;
  }

  // cout << "State" << to_string(state->id) << endl;

  std::cout << to_string(state, state->epsilon_transitions) << std::endl;
  std::cout << to_string(state, state->transitions) << std::endl;

  // cout << "State (" << state->id << ") {" << endl;
  // cout << "  end: " << (state->is_end ? "true" : "false") << endl;
  // cout << "  ep: " << to_string(state->epsilon_tranisitions) << endl;
  // cout << "  tr: " << to_string(state->transitions) << endl;
  // cout << "}" << endl;

  seen_states.push_back(state->id);

  for (const auto &eps : state->epsilon_transitions) {
    print(eps, seen_states);
  }

  for (const auto &kv : state->transitions) {
    print(kv.second, seen_states);
  }
}

void print(const NFA &nfa) {
  std::vector<int> seen_states;

  std::cout << "@startuml test" << std::endl
            << "hide empty description" << std::endl;

  std::cout << "[*] --> State" << std::to_string(nfa.start->id) << std::endl;

  print(nfa.start, seen_states);

  std::cout << "State" << std::to_string(global_id - 1) << " --> [*]"
            << std::endl;

  std::cout << "@enduml" << std::endl;
}

std::shared_ptr<State> create_state(bool is_end) {
  auto id = global_id;
  global_id++;
  return std::make_shared<State>(State{id, is_end, {}, {}});
}

void add_epsilon_transition(const std::shared_ptr<State> &from,
                            const std::shared_ptr<State> &to) {
  from->epsilon_transitions.push_back(to);
}

void add_transition(const std::shared_ptr<State> &from,
                    const std::shared_ptr<State> &to, char symbol) {
  from->transitions[symbol] = to;
}

NFA from_epsilon() {
  auto start = create_state(false);
  auto end = create_state(true);

  add_epsilon_transition(start, end);
  return NFA{start, end};
}

NFA from_symbol(char symbol) {
  auto start = create_state(false);
  auto end = create_state(true);

  add_transition(start, end, symbol);
  return NFA{start, end};
}

// ab : a and b
NFA nfa_concat(const NFA &first, const NFA &second) {
  add_epsilon_transition(first.end, second.start);
  first.end->is_end = false;
  return NFA{first.start, second.end};
}

// a|b : a or b
NFA nfa_union(const NFA &first, const NFA &second) {
  auto start = create_state(false);
  add_epsilon_transition(start, first.start);
  add_epsilon_transition(start, second.start);

  auto end = create_state(true);
  add_epsilon_transition(first.end, end);
  first.end->is_end = false;
  add_epsilon_transition(second.end, end);
  second.end->is_end = false;

  return NFA{start, end};
}

// a* : one or more
NFA nfa_closure(const NFA &nfa) {
  auto start = create_state(false);
  auto end = create_state(true);

  add_epsilon_transition(start, end);
  add_epsilon_transition(start, nfa.start);

  add_epsilon_transition(nfa.end, end);
  add_epsilon_transition(nfa.end, nfa.start);
  nfa.end->is_end = false;

  return NFA{start, end};
}

// a? : zero or one
NFA zero_or_one(const NFA &nfa) {
  auto start = create_state(false);
  auto end = create_state(true);

  // eps from start to end
  add_epsilon_transition(start, end);

  // eps from start to nfa.start
  add_epsilon_transition(start, nfa.start);

  // eps from nfa.end to end
  add_epsilon_transition(nfa.end, end);

  // set nfa.end.is_end to false
  nfa.end->is_end = false;

  return NFA{start, end};
}

// a+ : one or more
NFA one_or_more(const NFA &nfa) {
  auto start = create_state(false);
  auto end = create_state(true);

  assert(nfa.end->is_end);

  // eps from start to nfa.start
  add_epsilon_transition(start, nfa.start);

  // set nfa.end.is_end to false
  nfa.end->is_end = false;

  // eps from nfa.end to end
  add_epsilon_transition(nfa.end, end);

  // eps from end to nfa.start
  add_epsilon_transition(nfa.end, nfa.start);

  auto new_ = NFA{start, end};

  assert(!new_.start->is_end);
  assert(new_.end->is_end);
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

  if (!state->epsilon_transitions.empty()) {
    for (const auto &st : state->epsilon_transitions) {

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
      auto next_state = state->transitions[symbol];

      if (next_state != nullptr) {
        add_next_state(next_state, next_states, {});
      }
    }

    current_states = next_states;
  }

  return find_if(cbegin(current_states), cend(current_states),
                 [](const std::shared_ptr<State> &state) {
                   assert(state != nullptr);
                   return state->is_end;
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
