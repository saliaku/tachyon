// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_BASE_VALUE_H_
#define TACHYON_ZK_BASE_VALUE_H_

#include <optional>
#include <string>
#include <utility>

#include "tachyon/base/functional/callback.h"
#include "tachyon/math/base/rational_field.h"

namespace tachyon::zk {

template <typename T>
class Value : public math::Field<Value<T>> {
 public:
  constexpr Value() = default;

  constexpr static Value Unknown() { return Value(); }

  constexpr static Value Known(const T& value) { return Value(value); }

  constexpr static Value Known(T&& value) { return Value(std::move(value)); }

  constexpr static Value Zero() { return Value(T::Zero()); }

  constexpr static Value One() { return Value(T::One()); }

  static Value Random() { return Value(T::Random()); }

  constexpr bool IsZero() const {
    if (IsNone()) return false;
    return value_->IsZero();
  }

  constexpr bool IsOne() const {
    if (IsNone()) return false;
    return value_->IsOne();
  }

  constexpr bool IsNone() const { return !value_.has_value(); }

  // Returns |Value::UnKnown()| if the value is |Value::Unknown()|, otherwise
  // calls |f| with the wrapped value and returns the result.
  template <typename W>
  constexpr Value<W> AndThen(base::OnceCallback<Value<W>(T)> f) const {
    if (IsNone()) return Unknown();
    return std::move(f).Run(value_.value());
  }

  std::string ToString() const {
    if (IsNone()) return "None";
    return value_->ToString();
  }

  constexpr bool operator==(const Value& other) const {
    if (IsNone()) return other.IsNone();
    if (other.IsNone()) return false;
    return *value_ == *other.value_;
  }
  constexpr bool operator!=(const Value& other) const {
    return operator!=(other);
  }

  const T& value() const { return *value_; }
  T& value() { return *value_; }

  const T* operator->() const { return value_.operator->(); }
  T* operator->() { return value_.operator->(); }

  // AdditiveSemigroup methods
  constexpr Value Add(const Value& other) const {
    if (IsNone() || other.IsNone()) return Unknown();
    return Value::Known(*value_ + *other.value_);
  }

  constexpr Value Add(const T& other) const {
    if (IsNone()) return Unknown();
    return Value::Known(*value_ + other);
  }

  constexpr Value& DoubleInPlace() {
    if (IsNone()) return *this;
    value_->DoubleInPlace();
    return *this;
  }

  // AdditiveGroup methods
  constexpr Value Sub(const Value& other) const {
    if (IsNone() || other.IsNone()) return Unknown();
    return Value::Known(*value_ - *other.value_);
  }

  constexpr Value Sub(const T& other) const {
    if (IsNone()) return Unknown();
    return Value::Known(*value_ - other);
  }

  constexpr Value& NegInPlace() {
    if (IsNone()) return *this;
    value_->NegInPlace();
    return *this;
  }

  // MultiplicativeSemigroup methods
  constexpr Value Mul(const Value& other) const {
    if (IsNone() || other.IsNone()) return Unknown();
    return Value::Known(*value_ * *other.value_);
  }

  constexpr Value Mul(const T& other) const {
    if (IsNone()) return Unknown();
    return Value::Known(*value_ * other);
  }

  constexpr Value& SquareInPlace() {
    if (IsNone()) return *this;
    value_->SquareInPlace();
    return *this;
  }

  // MultiplicativeGroup methods
  constexpr Value Div(const Value& other) const {
    if (IsNone() || other.IsNone()) return Unknown();
    return Value::Known(*value_ / *other.value_);
  }

  constexpr Value Div(const T& other) const {
    if (IsNone()) return Unknown();
    return Value::Known(*value_ / other);
  }

  constexpr Value& InverseInPlace() {
    if (IsNone()) return *this;
    value_->InverseInPlace();
    return *this;
  }

  Value<math::RationalField<T>> ToRationalFieldValue() const {
    if (IsNone()) return Value<math::RationalField<T>>::Unknown();
    return Value<math::RationalField<T>>::Known(
        math::RationalField<T>(value_.value()));
  }

 private:
  constexpr explicit Value(const T& value) : value_(value) {}
  constexpr explicit Value(T&& value) : value_(std::move(value)) {}

  std::optional<T> value_;
};

}  // namespace tachyon::zk

#endif  // TACHYON_ZK_BASE_VALUE_H_
