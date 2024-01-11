#ifndef TACHYON_ZK_PLONK_CIRCUIT_EXAMPLES_SIMPLE_CIRCUIT_DATA_H_
#define TACHYON_ZK_PLONK_CIRCUIT_EXAMPLES_SIMPLE_CIRCUIT_DATA_H_

#include <string>
#include <vector>

#include "tachyon/base/json/json.h"
// #include "tachyon/crypto/commitments/polynomial_openings.h"

namespace tachyon {

namespace zk::halo2 {
namespace {

template <typename PCS>
struct ColumnPackage {
  typename PCS::Poly poly;
  typename PCS::Evals evals;
  typename PCS::Commitment commitment;
};

template <typename PCS>
struct PermutationSet {
  ColumnPackage<PCS> product_poly;
};

template <typename PCS>
struct LookupPackage {
  typename PCS::Poly compressed_expr;
  typename PCS::Poly permuted_expr;
  typename PCS::Evals permuted_poly;
  typename PCS::Commitment commitment;
};

template <typename PCS>
struct SimpleCircuitData {
  using Poly = typename PCS::Poly;

  std::string circuit_name;
  int n = 0;
  int extended_n = 0;
  std::vector<ColumnPackage<PCS>> fixed_columns;
  Poly l_first;
  Poly l_last;
  Poly l_active_rows;
};

}  // namespace

}  // namespace zk::halo2

namespace base {

template <typename PCS>
class base::RapidJsonValueConverter<zk::halo2::ColumnPackage<PCS>> {
 public:
  template <typename Allocator>
  static rapidjson::Value From(const zk::halo2::ColumnPackage<PCS>& value,
                               Allocator& allocator) {
    rapidjson::Value object(rapidjson::kObjectType);
    AddJsonElement(object, "poly", value.poly, allocator);
    AddJsonElement(object, "evals", value.evals, allocator);
    AddJsonElement(object, "commitment", value.commitment, allocator);
    return object;
  }

  static bool To(const rapidjson::Value& json_value, std::string_view key,
                 zk::halo2::ColumnPackage<PCS>* value, std::string* error) {
    return true;
  }
};

template <typename PCS>
class base::RapidJsonValueConverter<zk::halo2::PermutationSet<PCS>> {
 public:
  template <typename Allocator>
  static rapidjson::Value From(const zk::halo2::PermutationSet<PCS>& value,
                               Allocator& allocator) {
    rapidjson::Value object(rapidjson::kObjectType);
    AddJsonElement(object, "product_poly", value.product_poly, allocator);
    return object;
  }

  static bool To(const rapidjson::Value& json_value, std::string_view key,
                 zk::halo2::PermutationSet<PCS>* value, std::string* error) {
    return true;
  }
};

template <typename PCS>
class base::RapidJsonValueConverter<zk::halo2::LookupPackage<PCS>> {
 public:
  template <typename Allocator>
  static rapidjson::Value From(const zk::halo2::LookupPackage<PCS>& value,
                               Allocator& allocator) {
    rapidjson::Value object(rapidjson::kObjectType);
    AddJsonElement(object, "compressed_expr", value.compressed_expr, allocator);
    AddJsonElement(object, "permuted_expr", value.permuted_expr, allocator);
    AddJsonElement(object, "permuted_poly", value.permuted_poly, allocator);
    AddJsonElement(object, "commitment", value.commitment, allocator);
    return object;
  }

  static bool To(const rapidjson::Value& json_value, std::string_view key,
                 zk::halo2::LookupPackage<PCS>* value, std::string* error) {
    return true;
  }
};

template <typename PCS>
class base::RapidJsonValueConverter<zk::halo2::SimpleCircuitData<PCS>> {
 public:
  template <typename Allocator>
  static rapidjson::Value From(const zk::halo2::SimpleCircuitData<PCS>& value,
                               Allocator& allocator) {
    rapidjson::Value object(rapidjson::kObjectType);
    AddJsonElement(object, "circuit_name", value.circuit_name, allocator);
    AddJsonElement(object, "n", value.n, allocator);
    AddJsonElement(object, "extended_n", value.extended_n, allocator);
    AddJsonElement(object, "fixed_columns", value.fixed_columns, allocator);
    AddJsonElement(object, "l_first", value.l_first, allocator);
    AddJsonElement(object, "l_last", value.l_last, allocator);
    AddJsonElement(object, "l_active_rows", value.l_active_rows, allocator);
    return object;
  }

  static bool To(const rapidjson::Value& json_value, std::string_view key,
                 zk::halo2::SimpleCircuitData<PCS>* value, std::string* error) {
    return true;
  }
};

}  // namespace base
}  // namespace tachyon

#endif  // TACHYON_ZK_PLONK_CIRCUIT_EXAMPLES_SIMPLE_CIRCUIT_DATA_H_
