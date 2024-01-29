// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_BASE_TABLE_BASE_H_
#define TACHYON_ZK_PLONK_BASE_TABLE_BASE_H_

#include <vector>

#include "absl/types/span.h"

#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/logging.h"
#include "tachyon/base/ref.h"
#include "tachyon/base/template_util.h"
#include "tachyon/zk/plonk/base/column_key.h"

namespace tachyon::zk::plonk {

template <typename PolyOrEvals>
class TableBase {
 public:
  virtual ~TableBase() = default;

  virtual absl::Span<const PolyOrEvals> GetFixedColumns() const = 0;
  virtual absl::Span<const PolyOrEvals> GetAdviceColumns() const = 0;
  virtual absl::Span<const PolyOrEvals> GetInstanceColumns() const = 0;

  base::Ref<const PolyOrEvals> GetColumn(
      const ColumnKeyBase& column_key) const {
    switch (column_key.type()) {
      case ColumnType::kFixed:
        return base::Ref<const PolyOrEvals>(
            &GetFixedColumns()[column_key.index()]);
      case ColumnType::kAdvice:
        return base::Ref<const PolyOrEvals>(
            &GetAdviceColumns()[column_key.index()]);
      case ColumnType::kInstance:
        return base::Ref<const PolyOrEvals>(
            &GetInstanceColumns()[column_key.index()]);
      case ColumnType::kAny:
        break;
    }
    NOTREACHED();
    return base::Ref<const PolyOrEvals>();
  }

  template <typename Container>
  std::vector<base::Ref<const PolyOrEvals>> GetColumns(
      const Container& column_keys) const {
    return base::Map(
        column_keys,
        [this](const base::container_value_t<Container>& column_key) {
          return GetColumn(column_key);
        });
  }
};

}  // namespace tachyon::zk::plonk

#endif  // TACHYON_ZK_PLONK_BASE_TABLE_BASE_H_
