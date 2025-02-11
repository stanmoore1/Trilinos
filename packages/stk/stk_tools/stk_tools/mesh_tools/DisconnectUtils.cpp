// Copyright 2002 - 2008, 2010, 2011 National Technology Engineering
// Solutions of Sandia, LLC (NTESS). Under the terms of Contract
// DE-NA0003525 with NTESS, the U.S. Government retains certain rights
// in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
//     * Neither the name of NTESS nor the names of its contributors
//       may be used to endorse or promote products derived from this
//       software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "stk_tools/mesh_tools/DisconnectUtils.hpp"
#include "stk_io/IossBridge.hpp"
#include "stk_mesh/base/BulkData.hpp"
#include "stk_util/parallel/CommSparse.hpp"
#include "stk_util/util/SortAndUnique.hpp"
#include <algorithm>
#include <map>

namespace stk {
namespace tools {
namespace impl {

bool is_block(const stk::mesh::BulkData & bulk, stk::mesh::Part & part)
{
  const bool isElementPart = (part.primary_entity_rank() == stk::topology::ELEM_RANK);
  const bool isIoPart      = stk::io::has_io_part_attribute(part);
  return (isElementPart && isIoPart);
}

stk::mesh::Part* get_block_part_for_element(const stk::mesh::BulkData & bulk, stk::mesh::Entity element)
{
  const stk::mesh::PartVector & elementParts = bulk.bucket(element).supersets();
  for (stk::mesh::Part * part : elementParts) {
    if (is_block(bulk, *part)) {
      return part;
    }
  }
  return nullptr;
}

unsigned get_block_id_for_element(const stk::mesh::BulkData & bulk, stk::mesh::Entity element)
{
  const stk::mesh::PartVector & elementParts = bulk.bucket(element).supersets();
  for (stk::mesh::Part * part : elementParts) {
    if (is_block(bulk, *part)) {
      return part->mesh_meta_data_ordinal();
    }
  }
  return -1;
}

void fill_block_membership(const stk::mesh::BulkData& bulk, stk::mesh::Entity node, stk::mesh::PartVector& members)
{
  const unsigned numElems = bulk.num_elements(node);
  const stk::mesh::Entity* elements = bulk.begin_elements(node);

  for(unsigned i=0; i<numElems; ++i) {
    stk::mesh::Part* block = get_block_part_for_element(bulk, elements[i]);
//    std::cerr << "P" << bulk.parallel_rank() << ": " << "Node " << bulk.identifier(node) << " has block " << block->name() << " as member" << std::endl;
    ThrowRequire(block != nullptr);
    stk::util::insert_keep_sorted_and_unique(block, members, stk::mesh::PartLess());
  }
}

}}}
