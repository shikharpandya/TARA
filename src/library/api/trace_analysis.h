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

#ifndef TARA_API_TRACE_SEPERATION_H
#define TARA_API_TRACE_SEPERATION_H

#include "api/options.h"
#include "api/metric.h"
#include "api/output/output_base.h"
#include "input/program.h"
#include "cssa/program.h"
#include "hb_enc/encoding.h"

#include <unordered_set>

namespace tara {
namespace api {


enum class trace_result {
  always,
  never,
  depends,
  undecided,
};

class trace_analysis
{
public:
    trace_analysis(options options, z3::context& ctx);
    void input(std::string input_file);
    void input(tara::input::program& input_program);
    void gather_statistics(tara::api::metric& metric);
    trace_result seperate(output::output_base& output, metric& metric);
    bool atomic_sections(std::vector< tara::hb_enc::as >& output, bool merge_as = true);
    bool check_ambigious_traces(std::unordered_set< std::string >& result);
private:
    options _options;
    helpers::z3interf z3;
    std::shared_ptr<cssa::program> program;
    hb_enc::encoding& hb_encoding;
    /**
     * @brief Creates a solver for good traces
     * 
     * @param infeasable Should infeasable traces also be considered good?
     * This is needed for pruning, when we provide inputs
     * @return z3::solver
     */
    z3::solver make_good(bool include_infeasable);
    z3::solver make_bad();
};
}
}

#endif // TARA_API_TRACE_SEPERATION_H
