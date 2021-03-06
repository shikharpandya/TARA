/*
 * Copyright 2014, IST Austria
 *
 * This file is part of TARA.
 *
 * TARA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TARA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TARA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <unordered_set>
#include <list>
#include <vector>
#include <ostream>
#include <boost/concept_check.hpp>
#include <z3++.h>

namespace std {
  template <>
  struct hash<z3::expr>
  {
    std::size_t operator()(const z3::expr& k) const
    {
      using std::size_t;
      using std::hash;
      
      
      return (hash<Z3_ast>()((Z3_ast)k));
    }
  };
}

namespace tara {
  
namespace input {
  enum class data_type {
    bv32,
    bv16,
    bv64,
    integer,
    boolean,
    real,
    smt, // an smt string
  };
  
  struct variable {
    std::string name;
    data_type type;
    std::string smt_type; // in case it is not one of the above types
    operator std::string() const { return name;}
    variable(std::string name, data_type type) : name(name), type(type) {}
    variable(std::string name, std::string type) : name(name), type(data_type::smt), smt_type(type) {}
  };
  
  struct variable_hash {
    size_t operator () (const variable &v) const { return std::hash<std::string>()(v.name); }
  };
  
  struct variable_equal : std::binary_function <variable,variable,bool> {
    bool operator() (const variable& x, const variable& y) const {
      return std::equal_to<std::string>()(x.name, y.name);
    }
  };
  
  typedef std::unordered_set<variable, variable_hash, variable_equal> variable_set;
  typedef std::unordered_set<std::string> string_set;
}

namespace cssa {
  struct variable {
    std::string name;
    z3::sort sort;
    operator std::string() const { return name;}
    variable(std::string name, z3::sort sort) : name(name), sort(sort) {}
    variable(std::string name, z3::context& ctx) : name(name), sort(z3::sort(ctx)) {}
    variable(z3::context& ctx) : sort(z3::sort(ctx)) {}
    
    friend variable operator+ (const variable& v, const std::string& str) {
      return variable(v.name+str, v.sort);
    }
    friend variable operator+ (const std::string& str, const variable& v) {
      return variable(str+v.name, v.sort);
    }
    
    operator z3::expr() const {
      return sort.ctx().constant(name.c_str(), sort);
    }
    
    friend std::ostream& operator<< (std::ostream& stream, const variable& var) {
      stream << var.name;
      return stream;
    }
    bool operator==(const variable& rhs) const {
      return std::equal_to<std::string>()(this->name, rhs.name);
    }
    bool operator!=(const variable& rhs) const {
      return !(*this==rhs);
    }
  };
  
  
  
  struct variable_hash {
    size_t operator () (const variable &v) const { return std::hash<std::string>()(v.name); }
  };
  
  struct variable_equal : std::binary_function <variable,variable,bool> {
    bool operator() (const variable& x, const variable& y) const {
      return x==y;
    }
  };
  
  typedef std::unordered_set<variable, variable_hash, variable_equal> variable_set;
}

namespace helpers {
  
inline cssa::variable get_unprimed(const cssa::variable& variable) {
  if (variable.name[variable.name.size()-1] != '.') {
    return variable;
  } else {
    std::string v1 = variable.name.substr(0, variable.name.size()-1);
    return cssa::variable(v1, variable.sort);
  }
}

inline std::string get_unprimed(const std::string& variable) {
  if (variable[variable.size()-1] != '.') {
    return variable;
  } else {
    std::string v1 = variable.substr(0, variable.size()-1);
    return v1;
  }
}

inline bool is_primed(const std::string& variable) {
  return (variable[variable.size()-1] == '.');
}

inline std::string get_before_hash(const std::string& variable) {
  std::string result;
  for (char c:variable) {
    if (c=='#')
      break;
    result.push_back(c);
  }
  return result;
}

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
std::unordered_set<Key, Hash, Pred> set_union(const std::unordered_set<Key, Hash, Pred>& set1, const std::unordered_set<Key, Hash, Pred>& set2) {
  std::unordered_set<Key, Hash, Pred> res = set1;
  res.insert(set2.begin(), set2.end());
  return res;
}

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
void print_set(const std::unordered_set<Key, Hash, Pred>& set1, std::ostream& out) {
  for (Key c : set1) {
    out << c << " ";
  }
  out << std::endl;
}

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
std::unordered_set<Key, Hash, Pred> set_intersection(const std::unordered_set<Key, Hash, Pred>& set1, const std::unordered_set<Key, Hash, Pred>& set2) {
  std::unordered_set<Key, Hash, Pred> result;
  for (auto& e : set1) {
    if (set2.find(e) != set2.end())
      result.insert(e);
  }
  return result;
}

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
bool intersection_nonempty(const std::unordered_set<Key, Hash, Pred>& set1, const std::unordered_set<Key, Hash, Pred>& set2) {
  for (auto& e : set1) {
    if (set2.find(e) != set2.end())
      return true;
  }
  return false;
}

template <class content>
void print_vector(const std::vector<content>& v, std::ostream& out) {
  for(content c: v)
    out << c << std::endl;
}

template <class content>
void remove_duplicates(std::list<content>& v) {
  for(auto it = v.begin(); it!=v.end(); it++) {
    auto j = it;
    for (j++; j!=v.end(); ) {
      if (*j==*it)
        j = v.erase(j);
      else
        j++;
    }
  }
}

template <class content>
void print_list(const std::list<content>& v, std::ostream& out) {
  for(content c: v)
    out << c << std::endl;
}

template <class content>
void print_array(content v[], unsigned length, std::ostream& out) {
  out << "[ ";
  for(unsigned i=0; i<length; i++)
    out << v[i] << " ";
  out << "]" << std::endl;
}

template <class T1>
inline std::pair<T1,T1> swap_pair(std::pair<T1,T1> p) {
  T1 h = p.first;
  p.first = p.second;
  p.second = h;
  return p;
}

template <typename Iterator, class T>
bool last_element(Iterator element, const std::list<T>& list) {
  return ++element == list.end();
}

template <typename Iterator, class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
bool last_element(Iterator element, const std::unordered_set<Key, Hash, Pred>& list) {
  return ++element == list.end();
}

template <class content>
bool list_contains(const std::list<content>& v, const content& element) {
  for(content c: v)
    if (c==element) return true;
  return false;
}

}}

#endif // HELPERS_H
