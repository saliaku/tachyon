#ifndef VENDORS_HALO2_SRC_BUFFER_READER_H_
#define VENDORS_HALO2_SRC_BUFFER_READER_H_

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "tachyon/base/buffer/buffer.h"
#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/logging.h"
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
#include "vendors/halo2/src/endian_auto_reset.h"

namespace tachyon::halo2_api {

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
class BufferReader<zk::plonk::Phase> {
 public:
  static zk::plonk::Phase Read(base::Buffer& buffer) {
    return zk::plonk::Phase(BufferReader<uint8_t>::Read(buffer));
  }
};

template <>
class BufferReader<zk::plonk::Challenge> {
 public:
  static zk::plonk::Challenge Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    zk::plonk::Phase phase = BufferReader<zk::plonk::Phase>::Read(buffer);
    return {index, phase};
  }
};

template <>
class BufferReader<zk::plonk::Rotation> {
 public:
  static zk::plonk::Rotation Read(base::Buffer& buffer) {
    return zk::plonk::Rotation(BufferReader<int32_t>::Read(buffer));
  }
};

template <>
class BufferReader<zk::plonk::Selector> {
 public:
  static zk::plonk::Selector Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    bool is_simple = ReadU8AsBool(buffer);
    return is_simple ? zk::plonk::Selector::Simple(index)
                     : zk::plonk::Selector::Complex(index);
  }
};

template <zk::plonk::ColumnType C>
class BufferReader<zk::plonk::ColumnKey<C>> {
 public:
  static zk::plonk::ColumnKey<C> Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    uint8_t kind = BufferReader<uint8_t>::Read(buffer);
    if constexpr (C == zk::plonk::ColumnType::kAdvice) {
      CHECK_EQ(kind, static_cast<int8_t>(C));
      return zk::plonk::ColumnKey<C>(
          index, BufferReader<zk::plonk::Phase>::Read(buffer));
    } else {
      if constexpr (C == zk::plonk::ColumnType::kInstance ||
                    C == zk::plonk::ColumnType::kFixed) {
        CHECK_EQ(kind, static_cast<int8_t>(C));
        return zk::plonk::ColumnKey<C>(index);
      } else {
        zk::plonk::Phase phase = BufferReader<zk::plonk::Phase>::Read(buffer);
        switch (static_cast<zk::plonk::ColumnType>(kind)) {
          case zk::plonk::ColumnType::kAdvice:
            return zk::plonk::AdviceColumnKey(index, phase);
          case zk::plonk::ColumnType::kInstance:
            return zk::plonk::InstanceColumnKey(index);
          case zk::plonk::ColumnType::kFixed:
            return zk::plonk::FixedColumnKey(index);
          case zk::plonk::ColumnType::kAny:
            break;
        }
        NOTREACHED();
        return zk::plonk::AnyColumnKey();
      }
    }
  }
};

template <>
class BufferReader<zk::plonk::VirtualCell> {
 public:
  static zk::plonk::VirtualCell Read(base::Buffer& buffer) {
    zk::plonk::AnyColumnKey column =
        BufferReader<zk::plonk::AnyColumnKey>::Read(buffer);
    zk::plonk::Rotation rotation =
        BufferReader<zk::plonk::Rotation>::Read(buffer);
    return {column, rotation};
  }
};

template <zk::plonk::ColumnType C>
class BufferReader<zk::plonk::QueryData<C>> {
 public:
  static zk::plonk::QueryData<C> Read(base::Buffer& buffer) {
    if constexpr (C == zk::plonk::ColumnType::kAny) {
      NOTREACHED();
    } else {
      zk::plonk::ColumnKey<C> column =
          BufferReader<zk::plonk::ColumnKey<C>>::Read(buffer);
      zk::plonk::Rotation rotation =
          BufferReader<zk::plonk::Rotation>::Read(buffer);
      return {rotation, column};
    }
  }
};

template <zk::plonk::ColumnType C>
class BufferReader<zk::plonk::Query<C>> {
 public:
  static zk::plonk::Query<C> Read(base::Buffer& buffer) {
    size_t index = ReadU32AsSizeT(buffer);
    size_t column_index = ReadU32AsSizeT(buffer);
    zk::plonk::Rotation rotation =
        BufferReader<zk::plonk::Rotation>::Read(buffer);
    if constexpr (C == zk::plonk::ColumnType::kAdvice) {
      zk::plonk::Phase phase = BufferReader<zk::plonk::Phase>::Read(buffer);
      return {
          index,
          rotation,
          zk::plonk::ColumnKey<C>(column_index, phase),
      };
    } else {
      if constexpr (C == zk::plonk::ColumnType::kInstance ||
                    C == zk::plonk::ColumnType::kFixed) {
        return {
            index,
            rotation,
            zk::plonk::ColumnKey<C>(column_index),
        };
      } else {
        NOTREACHED();
      }
    }
  }
};

template <typename F>
class BufferReader<std::unique_ptr<zk::plonk::Expression<F>>> {
 public:
  static std::unique_ptr<zk::plonk::Expression<F>> Read(base::Buffer& buffer) {
    uint8_t kind = BufferReader<uint8_t>::Read(buffer);
    switch (kind) {
      case 0:
        return zk::plonk::ExpressionFactory<F>::Constant(
            BufferReader<F>::Read(buffer));
      case 1:
        return zk::plonk::ExpressionFactory<F>::Selector(
            BufferReader<zk::plonk::Selector>::Read(buffer));
      case 2:
        return zk::plonk::ExpressionFactory<F>::Fixed(
            BufferReader<zk::plonk::FixedQuery>::Read(buffer));
      case 3:
        return zk::plonk::ExpressionFactory<F>::Advice(
            BufferReader<zk::plonk::AdviceQuery>::Read(buffer));
      case 4:
        return zk::plonk::ExpressionFactory<F>::Instance(
            BufferReader<zk::plonk::InstanceQuery>::Read(buffer));
      case 5:
        return zk::plonk::ExpressionFactory<F>::Challenge(
            BufferReader<zk::plonk::Challenge>::Read(buffer));
      case 6:
        return zk::plonk::ExpressionFactory<F>::Negated(
            BufferReader<std::unique_ptr<zk::plonk::Expression<F>>>::Read(
                buffer));
      case 7: {
        std::unique_ptr<zk::plonk::Expression<F>> left;
        ReadBuffer(buffer, left);
        std::unique_ptr<zk::plonk::Expression<F>> right;
        ReadBuffer(buffer, right);
        return zk::plonk::ExpressionFactory<F>::Sum(std::move(left),
                                                    std::move(right));
      }
      case 8: {
        std::unique_ptr<zk::plonk::Expression<F>> left;
        ReadBuffer(buffer, left);
        std::unique_ptr<zk::plonk::Expression<F>> right;
        ReadBuffer(buffer, right);
        return zk::plonk::ExpressionFactory<F>::Product(std::move(left),
                                                        std::move(right));
      }
      case 9: {
        std::unique_ptr<zk::plonk::Expression<F>> expr;
        ReadBuffer(buffer, expr);
        F scale = BufferReader<F>::Read(buffer);
        return zk::plonk::ExpressionFactory<F>::Scaled(std::move(expr),
                                                       std::move(scale));
      }
    }
    NOTREACHED();
    return nullptr;
  }
};

template <typename F>
class BufferReader<zk::plonk::Gate<F>> {
 public:
  static zk::plonk::Gate<F> Read(base::Buffer& buffer) {
    std::vector<std::unique_ptr<zk::plonk::Expression<F>>> polys;
    ReadBuffer(buffer, polys);
    std::vector<zk::plonk::Selector> queried_selectors;
    ReadBuffer(buffer, queried_selectors);
    std::vector<zk::plonk::VirtualCell> queried_cells;
    ReadBuffer(buffer, queried_cells);
    return zk::plonk::Gate<F>("", {}, std::move(polys),
                              std::move(queried_selectors),
                              std::move(queried_cells));
  }
};

template <>
class BufferReader<zk::plonk::PermutationArgument> {
 public:
  static zk::plonk::PermutationArgument Read(base::Buffer& buffer) {
    std::vector<zk::plonk::AnyColumnKey> column_keys;
    ReadBuffer(buffer, column_keys);
    return zk::plonk::PermutationArgument(std::move(column_keys));
  }
};

template <typename F>
class BufferReader<zk::plonk::LookupArgument<F>> {
 public:
  static zk::plonk::LookupArgument<F> Read(base::Buffer& buffer) {
    std::vector<std::unique_ptr<zk::plonk::Expression<F>>> input_expressions;
    ReadBuffer(buffer, input_expressions);
    std::vector<std::unique_ptr<zk::plonk::Expression<F>>> table_expressions;
    ReadBuffer(buffer, table_expressions);
    return zk::plonk::LookupArgument<F>("", std::move(input_expressions),
                                        std::move(table_expressions));
  }
};

template <typename Poly, typename Evals>
class BufferReader<zk::plonk::PermutationProvingKey<Poly, Evals>> {
 public:
  static zk::plonk::PermutationProvingKey<Poly, Evals> Read(
      base::Buffer& buffer) {
    std::vector<Evals> permutations;
    ReadBuffer(buffer, permutations);
    std::vector<Poly> polys;
    ReadBuffer(buffer, polys);
    return zk::plonk::PermutationProvingKey<Poly, Evals>(
        std::move(permutations), std::move(polys));
  }
};

}  // namespace tachyon::halo2_api

#endif  // VENDORS_HALO2_SRC_BUFFER_READER_H_
