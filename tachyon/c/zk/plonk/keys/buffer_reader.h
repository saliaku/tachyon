#ifndef TACHYON_C_ZK_PLONK_KEYS_BUFFER_READER_H_
#define TACHYON_C_ZK_PLONK_KEYS_BUFFER_READER_H_

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "tachyon/base/buffer/buffer.h"
#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/logging.h"
#include "tachyon/c/zk/plonk/keys/buffer_reader.h"
#include "tachyon/c/zk/plonk/keys/endian_auto_reset.h"
#include "tachyon/math/elliptic_curves/affine_point.h"
#include "tachyon/math/finite_fields/prime_field_base.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluations.h"
#include "tachyon/math/polynomials/univariate/univariate_polynomial.h"
#include "tachyon/zk/expressions/expression_factory.h"
#include "tachyon/zk/lookup/lookup_argument.h"
#include "tachyon/zk/plonk/base/column_key.h"
#include "tachyon/zk/plonk/base/phase.h"
#include "tachyon/zk/plonk/constraint_system/gate.h"
#include "tachyon/zk/plonk/permutation/permutation_argument.h"
#include "tachyon/zk/plonk/permutation/permutation_proving_key.h"

namespace tachyon::c::zk {

template <typename T, typename SFINAE = void>
class BufferReader;

template <typename T>
void ReadBuffer(base::Buffer& buffer, T& value) {
  value = BufferReader<T>::Read(buffer);
}

template <typename T>
class BufferReader<T, std::enable_if_t<std::is_integral_v<T>>> {
 public:
  static T Read(base::Buffer& buffer) {
    EndianAutoReset resetter(buffer, base::Endian::kBig);
    T v;
    CHECK(buffer.Read(&v));
    return v;
  }
};

inline bool ReadU8AsBool(base::Buffer& buffer) {
  return BufferReader<uint8_t>::Read(buffer) != 0;
}

inline size_t ReadU32AsSizeT(base::Buffer& buffer) {
  return size_t{BufferReader<uint32_t>::Read(buffer)};
}

template <typename T>
class BufferReader<std::vector<T>> {
 public:
  static std::vector<T> Read(base::Buffer& buffer) {
    return base::CreateVector(ReadU32AsSizeT(buffer), [&buffer]() {
      return BufferReader<T>::Read(buffer);
    });
  }
};

template <typename Curve>
class BufferReader<math::AffinePoint<Curve>> {
 public:
  using BaseField = typename math::AffinePoint<Curve>::BaseField;

  static math::AffinePoint<Curve> Read(base::Buffer& buffer) {
    BaseField x = BufferReader<BaseField>::Read(buffer);
    BaseField y = BufferReader<BaseField>::Read(buffer);
    return {std::move(x), std::move(y)};
  }
};

template <typename T>
class BufferReader<
    T, std::enable_if_t<std::is_base_of_v<math::PrimeFieldBase<T>, T>>> {
 public:
  using BigInt = typename T::BigIntTy;

  static T Read(base::Buffer& buffer) {
    EndianAutoReset resetter(buffer, base::Endian::kLittle);
    BigInt montgomery;
    CHECK(buffer.Read(montgomery.limbs));
    return T::FromMontgomery(montgomery);
  }
};

template <typename F, size_t MaxDegree>
class BufferReader<math::UnivariateDensePolynomial<F, MaxDegree>> {
 public:
  static math::UnivariateDensePolynomial<F, MaxDegree> Read(
      base::Buffer& buffer) {
    std::vector<F> coeffs;
    ReadBuffer(buffer, coeffs);
    return math::UnivariateDensePolynomial<F, MaxDegree>(
        math::UnivariateDenseCoefficients<F, MaxDegree>(std::move(coeffs)));
  }
};

template <typename F, size_t MaxDegree>
class BufferReader<math::UnivariateEvaluations<F, MaxDegree>> {
 public:
  static math::UnivariateEvaluations<F, MaxDegree> Read(base::Buffer& buffer) {
    std::vector<F> evals;
    ReadBuffer(buffer, evals);
    return math::UnivariateEvaluations<F, MaxDegree>(std::move(evals));
  }
};

template <>
class BufferReader<tachyon::zk::Phase> {
 public:
  static tachyon::zk::Phase Read(base::Buffer& buffer) {
    return tachyon::zk::Phase(BufferReader<uint8_t>::Read(buffer));
  }
};

template <>
class BufferReader<tachyon::zk::Challenge> {
 public:
  static tachyon::zk::Challenge Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    tachyon::zk::Phase phase = BufferReader<tachyon::zk::Phase>::Read(buffer);
    return {index, phase};
  }
};

template <>
class BufferReader<tachyon::zk::Rotation> {
 public:
  static tachyon::zk::Rotation Read(base::Buffer& buffer) {
    return tachyon::zk::Rotation(BufferReader<int32_t>::Read(buffer));
  }
};

template <>
class BufferReader<tachyon::zk::Selector> {
 public:
  static tachyon::zk::Selector Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    bool is_simple = ReadU8AsBool(buffer);
    return is_simple ? tachyon::zk::Selector::Simple(index)
                     : tachyon::zk::Selector::Complex(index);
  }
};

template <tachyon::zk::ColumnType C>
class BufferReader<tachyon::zk::ColumnKey<C>> {
 public:
  static tachyon::zk::ColumnKey<C> Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    uint8_t kind = BufferReader<uint8_t>::Read(buffer);
    if constexpr (C == tachyon::zk::ColumnType::kAdvice) {
      CHECK_EQ(kind, static_cast<int8_t>(C));
      return tachyon::zk::ColumnKey<C>(
          index, BufferReader<tachyon::zk::Phase>::Read(buffer));
    } else {
      if constexpr (C == tachyon::zk::ColumnType::kInstance ||
                    C == tachyon::zk::ColumnType::kFixed) {
        CHECK_EQ(kind, static_cast<int8_t>(C));
        return tachyon::zk::ColumnKey<C>(index);
      } else {
        tachyon::zk::Phase phase =
            BufferReader<tachyon::zk::Phase>::Read(buffer);
        switch (static_cast<tachyon::zk::ColumnType>(kind)) {
          case tachyon::zk::ColumnType::kAdvice:
            return tachyon::zk::AdviceColumnKey(index, phase);
          case tachyon::zk::ColumnType::kInstance:
            return tachyon::zk::InstanceColumnKey(index);
          case tachyon::zk::ColumnType::kFixed:
            return tachyon::zk::FixedColumnKey(index);
          case tachyon::zk::ColumnType::kAny:
            break;
        }
        NOTREACHED();
        return tachyon::zk::AnyColumnKey();
      }
    }
  }
};

template <>
class BufferReader<tachyon::zk::VirtualCell> {
 public:
  static tachyon::zk::VirtualCell Read(base::Buffer& buffer) {
    tachyon::zk::AnyColumnKey column =
        BufferReader<tachyon::zk::AnyColumnKey>::Read(buffer);
    tachyon::zk::Rotation rotation =
        BufferReader<tachyon::zk::Rotation>::Read(buffer);
    return {column, rotation};
  }
};

template <tachyon::zk::ColumnType C>
class BufferReader<tachyon::zk::QueryData<C>> {
 public:
  static tachyon::zk::QueryData<C> Read(base::Buffer& buffer) {
    if constexpr (C == tachyon::zk::ColumnType::kAny) {
      NOTREACHED();
    } else {
      tachyon::zk::ColumnKey<C> column =
          BufferReader<tachyon::zk::ColumnKey<C>>::Read(buffer);
      tachyon::zk::Rotation rotation =
          BufferReader<tachyon::zk::Rotation>::Read(buffer);
      return {rotation, column};
    }
  }
};

template <tachyon::zk::ColumnType C>
class BufferReader<tachyon::zk::Query<C>> {
 public:
  static tachyon::zk::Query<C> Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    size_t column_index = ReadU32AsSizeT(buffer);
    tachyon::zk::Rotation rotation =
        BufferReader<tachyon::zk::Rotation>::Read(buffer);
    if constexpr (C == tachyon::zk::ColumnType::kAdvice) {
      tachyon::zk::Phase phase = BufferReader<tachyon::zk::Phase>::Read(buffer);
      return {
          index,
          rotation,
          tachyon::zk::ColumnKey<C>(column_index, phase),
      };
    } else {
      if constexpr (C == tachyon::zk::ColumnType::kInstance ||
                    C == tachyon::zk::ColumnType::kFixed) {
        return {
            index,
            rotation,
            tachyon::zk::ColumnKey<C>(column_index),
        };
      } else {
        NOTREACHED();
      }
    }
  }
};

template <typename F>
class BufferReader<std::unique_ptr<tachyon::zk::Expression<F>>> {
 public:
  static std::unique_ptr<tachyon::zk::Expression<F>> Read(
      base::Buffer& buffer) {
    uint8_t kind = BufferReader<uint8_t>::Read(buffer);
    switch (kind) {
      case 0:
        return tachyon::zk::ExpressionFactory<F>::Constant(
            BufferReader<F>::Read(buffer));
      case 1:
        return tachyon::zk::ExpressionFactory<F>::Selector(
            BufferReader<tachyon::zk::Selector>::Read(buffer));
      case 2:
        return tachyon::zk::ExpressionFactory<F>::Fixed(
            BufferReader<tachyon::zk::FixedQuery>::Read(buffer));
      case 3:
        return tachyon::zk::ExpressionFactory<F>::Advice(
            BufferReader<tachyon::zk::AdviceQuery>::Read(buffer));
      case 4:
        return tachyon::zk::ExpressionFactory<F>::Instance(
            BufferReader<tachyon::zk::InstanceQuery>::Read(buffer));
      case 5:
        return tachyon::zk::ExpressionFactory<F>::Challenge(
            BufferReader<tachyon::zk::Challenge>::Read(buffer));
      case 6:
        return tachyon::zk::ExpressionFactory<F>::Negated(
            BufferReader<std::unique_ptr<tachyon::zk::Expression<F>>>::Read(
                buffer));
      case 7: {
        std::unique_ptr<tachyon::zk::Expression<F>> left;
        ReadBuffer(buffer, left);
        std::unique_ptr<tachyon::zk::Expression<F>> right;
        ReadBuffer(buffer, right);
        return tachyon::zk::ExpressionFactory<F>::Sum(std::move(left),
                                                      std::move(right));
      }
      case 8: {
        std::unique_ptr<tachyon::zk::Expression<F>> left;
        ReadBuffer(buffer, left);
        std::unique_ptr<tachyon::zk::Expression<F>> right;
        ReadBuffer(buffer, right);
        return tachyon::zk::ExpressionFactory<F>::Product(std::move(left),
                                                          std::move(right));
      }
      case 9: {
        std::unique_ptr<tachyon::zk::Expression<F>> expr;
        ReadBuffer(buffer, expr);
        F scale = BufferReader<F>::Read(buffer);
        return tachyon::zk::ExpressionFactory<F>::Scaled(std::move(expr),
                                                         std::move(scale));
      }
    }
    NOTREACHED();
    return nullptr;
  }
};

template <typename F>
class BufferReader<tachyon::zk::Gate<F>> {
 public:
  static tachyon::zk::Gate<F> Read(base::Buffer& buffer) {
    std::vector<std::unique_ptr<tachyon::zk::Expression<F>>> polys;
    ReadBuffer(buffer, polys);
    std::vector<tachyon::zk::Selector> queried_selectors;
    ReadBuffer(buffer, queried_selectors);
    std::vector<tachyon::zk::VirtualCell> queried_cells;
    ReadBuffer(buffer, queried_cells);
    return tachyon::zk::Gate<F>("", {}, std::move(polys),
                                std::move(queried_selectors),
                                std::move(queried_cells));
  }
};

template <>
class BufferReader<tachyon::zk::PermutationArgument> {
 public:
  static tachyon::zk::PermutationArgument Read(base::Buffer& buffer) {
    std::vector<tachyon::zk::AnyColumnKey> column_keys;
    ReadBuffer(buffer, column_keys);
    return tachyon::zk::PermutationArgument(std::move(column_keys));
  }
};

template <typename F>
class BufferReader<tachyon::zk::LookupArgument<F>> {
 public:
  static tachyon::zk::LookupArgument<F> Read(base::Buffer& buffer) {
    std::vector<std::unique_ptr<tachyon::zk::Expression<F>>> input_expressions;
    ReadBuffer(buffer, input_expressions);
    std::vector<std::unique_ptr<tachyon::zk::Expression<F>>> table_expressions;
    ReadBuffer(buffer, table_expressions);
    return tachyon::zk::LookupArgument<F>("", std::move(input_expressions),
                                          std::move(table_expressions));
  }
};

template <typename Poly, typename Evals>
class BufferReader<tachyon::zk::PermutationProvingKey<Poly, Evals>> {
 public:
  static tachyon::zk::PermutationProvingKey<Poly, Evals> Read(
      base::Buffer& buffer) {
    std::vector<Evals> permutations;
    ReadBuffer(buffer, permutations);
    std::vector<Poly> polys;
    ReadBuffer(buffer, polys);
    return tachyon::zk::PermutationProvingKey<Poly, Evals>(
        std::move(permutations), std::move(polys));
  }
};

}  // namespace tachyon::c::zk

#endif  // TACHYON_C_ZK_PLONK_KEYS_BUFFER_READER_H_
