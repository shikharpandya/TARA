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


#ifndef TARA_INPUT_PARSE_H
#define TARA_INPUT_PARSE_H

#include <string>
#include <vector>
#include <string>
#include <unordered_set>
#include "input/program.h"
#include "constants.h"

namespace tara {
namespace input {
class parse
{
public:
  static input::program parseFile(const char* file);
  
  void addInstruction(int thread, std::string* name, std::string* command, string_set* havok_vars, instruction_type t);
  void addLocal(int thread, std::pair<std::string*,std::string*>* local);
  void addGlobal(std::pair<std::string*,std::string*>* global);
  void addThread(std::string* name);
  void addPrecondition(std::string* pre);
  void addAtomicSection(std::string* loc1, std::string* loc2);
  void addHappensBefore(std::string* loc1, std::string* loc2);
  
private:
  parse();
  program p;
  variable translate_type(std::string& name, std::string& type);
};
}}

#endif // TARA_INPUT_PARSE_H
