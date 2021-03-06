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
 */

#include "thread.h"
#include "cssa/cssa_exception.h"

using namespace std;
using namespace tara::helpers;

namespace tara {
namespace cssa {
  instruction::instruction(z3interf& z3, hb_enc::location_ptr location, cssa::thread* thread, string& name, instruction_type type, z3::expr original_expression) :
  loc(location), instr(z3::expr(z3.c)), path_constraint(z3::expr(z3.c)), in_thread(thread), name(name), type(type), original_expr(original_expression)
{
  if (thread==nullptr) 
    throw cssa_exception("Thread cannot be null");
}


std::ostream& operator <<(std::ostream& stream, const instruction& i) {
  stream << i.loc->name << ": ";
  if (i.havok_vars.size() > 0) {
    stream << "havok(";
    for (auto it = i.havok_vars.begin(); it!=i.havok_vars.end(); it++) {
      stream << *it;
      if (!last_element(it, i.havok_vars))
        stream << ", ";
    }
    stream << ") ";
  }
  switch (i.type) {
    case instruction_type::assert:
      stream << "assert " << i.original_expr;
      break;
    case instruction_type::assume:
      stream << "assume " << i.original_expr;
      break;
    default:
      stream << i.original_expr;
  }
  return stream;
}

variable_set instruction::variables() const
{
  return helpers::set_union(variables_read, variables_write);
}

variable_set instruction::variables_orig() const
{
  return helpers::set_union(variables_read_orig, variables_write_orig);
}

thread::thread(const string& name, const tara::cssa::variable_set locals) : name(name), locals(locals)
{}

bool thread::operator==(const thread &other) const {
  return this->name == other.name;
}

bool thread::operator!=(const thread &other) const {
  return !(*this==other);
}

unsigned int thread::size() const
{
  return instructions.size();
}

const instruction& thread::operator[](unsigned int i) const
{
  return *instructions[i];
}

instruction& thread::operator[](unsigned int i)
{
  return *instructions[i];
}

void thread::add_instruction(const std::shared_ptr<instruction>& instr)
{
  instructions.push_back(instr);
}
}}