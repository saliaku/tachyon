// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_CIRCUIT_ASSIGNMENT_H_
#define TACHYON_ZK_PLONK_CIRCUIT_ASSIGNMENT_H_

#include <optional>
#include <string>

#include "tachyon/base/functional/callback.h"
#include "tachyon/math/base/rational_field.h"
#include "tachyon/zk/base/row_index.h"
#include "tachyon/zk/base/value.h"
#include "tachyon/zk/plonk/circuit/column_key.h"
#include "tachyon/zk/plonk/constraint_system/challenge.h"
#include "tachyon/zk/plonk/constraint_system/selector.h"

namespace tachyon::zk {

template <typename F>
class Assignment {
 public:
  using AssignCallback = base::OnceCallback<Value<math::RationalField<F>>()>;

  virtual ~Assignment() = default;

  // Creates a new region and enters into it.
  //
  // Panics if we are currently in a region (if |ExitRegion()| was not called).
  //
  // Not intended for downstream consumption; use |Layouter::AssignRegion()|
  // instead.
  virtual void EnterRegion(std::string_view name) {}

  // Allows the developer to include a |name| for a specific column
  // within a |Region|.
  //
  // This is usually useful for debugging circuit failures.
  virtual void NameColumn(std::string_view name, const AnyColumnKey& column) {}

  // Exits the current region.
  //
  // Panics if we are not currently in a region (if |EnterRegion()| was not
  // called).
  //
  // Not intended for downstream consumption; use |Layouter::AssignRegion()|
  // instead.
  virtual void ExitRegion() {}

  // Enables a selector at the given row.
  virtual void EnableSelector(std::string_view name, const Selector& selector,
                              RowIndex row) {}

  // Queries the cell of an instance column at a particular absolute row.
  virtual Value<F> QueryInstance(const InstanceColumnKey& column,
                                 RowIndex row) {
    return Value<F>();
  }

  // Assign an advice column value (witness).
  virtual void AssignAdvice(std::string_view name,
                            const AdviceColumnKey& column, RowIndex row,
                            AssignCallback assign) {}

  // Assign a fixed value.
  virtual void AssignFixed(std::string_view name, const FixedColumnKey& column,
                           RowIndex row, AssignCallback assign) {}

  // Assign two cells to have the same value
  virtual void Copy(const AnyColumnKey& left_column, RowIndex left_row,
                    const AnyColumnKey& right_column, RowIndex right_row) {}

  // Fills a fixed |column| starting from the given |row| with |value|.
  virtual void FillFromRow(const FixedColumnKey& column, RowIndex row,
                           const math::RationalField<F>& value) {}

  // Queries the value of the given challenge.
  //
  // Returns |Value<F>::Unknown()| if the current synthesis phase
  // is before the challenge can be queried.
  virtual Value<F> GetChallenge(const Challenge& challenge) {
    return Value<F>::Unknown();
  }

  // Creates a new (sub)namespace and enters into it.
  //
  // Not intended for downstream consumption; use |Layouter::Namespace()|
  // instead.
  virtual void PushNamespace(std::string_view name) {}

  // Exits out of the existing namespace.
  //
  // Not intended for downstream consumption; use |Layouter::Namespace()|
  // instead.
  virtual void PopNamespace(const std::optional<std::string>& gadget_name) {}
};

}  // namespace tachyon::zk

#endif  // TACHYON_ZK_PLONK_CIRCUIT_ASSIGNMENT_H_
