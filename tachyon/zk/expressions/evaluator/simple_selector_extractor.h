// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_EXPRESSIONS_EVALUATOR_SIMPLE_SELECTOR_EXTRACTOR_H_
#define TACHYON_ZK_EXPRESSIONS_EVALUATOR_SIMPLE_SELECTOR_EXTRACTOR_H_

#include <optional>

#include "tachyon/zk/expressions/evaluator.h"
#include "tachyon/zk/expressions/selector_expression.h"

namespace tachyon::zk {

template <typename F>
class SimpleSelectorExtractor
    : public Evaluator<F, std::optional<plonk::Selector>> {
 public:
  // Evaluator methods
  std::optional<plonk::Selector> Evaluate(const Expression<F>* input) override {
    auto op = [](const std::optional<plonk::Selector>& left,
                 const std::optional<plonk::Selector>& right)
        -> std::optional<plonk::Selector> {
      CHECK(!(left.has_value() && right.has_value()))
          << "two simple selectors cannot be in the same expression";
      if (left.has_value()) return left;
      if (right.has_value()) return right;
      return std::nullopt;
    };

    switch (input->type()) {
      case ExpressionType::kConstant:
        return std::nullopt;
      case ExpressionType::kSelector: {
        const plonk::Selector& selector = input->ToSelector()->selector();
        if (selector.is_simple()) {
          return selector;
        }
        return std::nullopt;
      }
      case ExpressionType::kFixed:
        return std::nullopt;
      case ExpressionType::kAdvice:
        return std::nullopt;
      case ExpressionType::kInstance:
        return std::nullopt;
      case ExpressionType::kChallenge:
        return std::nullopt;
      case ExpressionType::kNegated: {
        return Evaluate(input->ToNegated()->expr());
      }
      case ExpressionType::kSum: {
        const SumExpression<F>* sum = input->ToSum();
        return op(Evaluate(sum->left()), Evaluate(sum->right()));
      }
      case ExpressionType::kProduct: {
        const ProductExpression<F>* product = input->ToProduct();
        return op(Evaluate(product->left()), Evaluate(product->right()));
      }
      case ExpressionType::kScaled: {
        const ScaledExpression<F>* scaled = input->ToScaled();
        return Evaluate(scaled->expr());
      }
    }
    NOTREACHED();
    return std::nullopt;
  }
};

template <typename F>
std::optional<plonk::Selector> Expression<F>::ExtractSimpleSelector() const {
  SimpleSelectorExtractor<F> extractor;
  return Evaluate(&extractor);
}

}  // namespace tachyon::zk

#endif  // TACHYON_ZK_EXPRESSIONS_EVALUATOR_SIMPLE_SELECTOR_EXTRACTOR_H_
