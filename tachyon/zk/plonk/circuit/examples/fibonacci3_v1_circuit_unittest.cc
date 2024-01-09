#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tachyon/zk/plonk/circuit/examples/circuit_test.h"
#include "tachyon/zk/plonk/circuit/examples/fibonacci3_circuit.h"
#include "tachyon/zk/plonk/circuit/floor_planner/v1/v1_floor_planner.h"
#include "tachyon/zk/plonk/halo2/pinned_verifying_key.h"
#include "tachyon/zk/plonk/keys/proving_key.h"

namespace tachyon::zk::halo2 {

namespace {

constexpr uint8_t kExpectedProof[] = {
    33,  231, 248, 121, 132, 226, 31,  179, 86,  67,  213, 109, 201, 53,  147,
    93,  41,  30,  16,  172, 115, 107, 0,   172, 225, 38,  5,   143, 136, 68,
    3,   47,  222, 110, 120, 246, 167, 202, 30,  64,  192, 113, 187, 20,  209,
    5,   19,  35,  161, 55,  99,  167, 153, 242, 153, 84,  169, 135, 224, 148,
    140, 22,  18,  148, 5,   145, 196, 201, 20,  216, 6,   183, 82,  73,  62,
    87,  36,  44,  76,  29,  120, 103, 124, 253, 87,  100, 218, 122, 245, 84,
    26,  140, 240, 97,  130, 134, 92,  104, 200, 22,  60,  26,  130, 227, 203,
    220, 139, 80,  219, 111, 129, 53,  194, 0,   185, 59,  50,  147, 22,  2,
    25,  58,  255, 70,  194, 103, 39,  5,   143, 199, 14,  63,  41,  37,  111,
    200, 181, 194, 119, 182, 175, 185, 182, 254, 90,  205, 250, 56,  35,  47,
    214, 38,  249, 166, 235, 34,  66,  177, 210, 132, 1,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   150, 107, 12,
    124, 196, 77,  15,  79,  155, 216, 235, 209, 100, 122, 149, 249, 157, 89,
    67,  210, 148, 56,  142, 81,  28,  163, 148, 208, 127, 138, 114, 131, 23,
    164, 155, 40,  100, 94,  242, 163, 88,  183, 181, 36,  19,  103, 5,   63,
    60,  235, 84,  80,  196, 17,  56,  251, 100, 214, 7,   143, 210, 12,  216,
    133, 119, 11,  133, 168, 250, 105, 120, 203, 125, 206, 178, 6,   227, 250,
    106, 23,  147, 20,  33,  100, 193, 139, 236, 71,  123, 126, 3,   82,  117,
    176, 47,  8,   74,  59,  111, 112, 95,  65,  253, 25,  129, 46,  119, 141,
    194, 161, 69,  95,  77,  169, 167, 232, 64,  188, 94,  92,  88,  131, 225,
    32,  248, 11,  226, 22,  29,  178, 49,  93,  246, 21,  2,   57,  104, 185,
    162, 71,  192, 133, 30,  192, 181, 133, 235, 240, 44,  84,  146, 30,  220,
    199, 56,  4,   123, 81,  147, 1,   116, 75,  114, 57,  186, 124, 103, 222,
    252, 87,  49,  106, 43,  42,  94,  200, 87,  93,  224, 25,  207, 65,  27,
    194, 70,  179, 167, 124, 134, 174, 221, 25,  91,  100, 85,  168, 54,  69,
    92,  205, 156, 50,  122, 149, 41,  184, 177, 13,  222, 84,  147, 191, 160,
    32,  88,  86,  224, 30,  6,   171, 107, 175, 51,  42,  85,  114, 224, 19,
    26,  101, 58,  148, 98,  155, 69,  195, 246, 144, 148, 161, 195, 117, 140,
    50,  149, 91,  56,  166, 196, 7,   65,  41,  142, 127, 141, 5,   106, 59,
    97,  110, 21,  101, 115, 49,  188, 125, 242, 25,  35,  102, 134, 196, 98,
    154, 226, 68,  209, 238, 65,  189, 86,  114, 68,  226, 250, 201, 138, 13,
    1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   194, 190, 145, 230, 207, 201, 92,  213, 120, 15,  23,  70,  11,
    109, 176, 69,  68,  7,   167, 163, 188, 71,  92,  5,   161, 253, 74,  20,
    162, 75,  100, 165, 194, 190, 145, 230, 207, 201, 92,  213, 120, 15,  23,
    70,  11,  109, 176, 69,  68,  7,   167, 163, 188, 71,  92,  5,   161, 253,
    74,  20,  162, 75,  100, 165};

class Fibonacci3V1CircuitTest : public CircuitTest {};

}  // namespace

TEST_F(Fibonacci3V1CircuitTest, Configure) {
  ConstraintSystem<F> constraint_system;
  Fibonacci3Config<F> config =
      Fibonacci3Circuit<F, V1FloorPlanner>::Configure(constraint_system);
  EXPECT_EQ(config.selector, Selector::Simple(0));
  EXPECT_EQ(config.a, AdviceColumnKey(0));
  EXPECT_EQ(config.b, AdviceColumnKey(1));
  EXPECT_EQ(config.c, AdviceColumnKey(2));
  EXPECT_EQ(config.a_equals_b.value_inv(), AdviceColumnKey(4));
  EXPECT_EQ(
      *config.a_equals_b.expr(),
      *ExpressionFactory<F>::Sum(
          ExpressionFactory<F>::Constant(F::One()),
          ExpressionFactory<F>::Negated(ExpressionFactory<F>::Product(
              ExpressionFactory<F>::Sum(
                  ExpressionFactory<F>::Advice(
                      AdviceQuery(0, Rotation::Cur(), config.a)),
                  ExpressionFactory<F>::Negated(ExpressionFactory<F>::Advice(
                      AdviceQuery(1, Rotation::Cur(), config.b)))),
              ExpressionFactory<F>::Advice(AdviceQuery(
                  2, Rotation::Cur(), config.a_equals_b.value_inv()))))));
  EXPECT_EQ(config.output, AdviceColumnKey(3));
  EXPECT_EQ(constraint_system.num_fixed_columns(), 0);
  EXPECT_EQ(constraint_system.num_advice_columns(), 5);
  EXPECT_EQ(constraint_system.num_instance_columns(), 0);
  EXPECT_EQ(constraint_system.num_selectors(), 1);
  EXPECT_EQ(constraint_system.num_challenges(), 0);
  std::vector<Phase> expected_advice_column_phases = {
      kFirstPhase, kFirstPhase, kFirstPhase, kFirstPhase, kFirstPhase};
  EXPECT_EQ(constraint_system.advice_column_phases(),
            expected_advice_column_phases);
  EXPECT_TRUE(constraint_system.challenge_phases().empty());
  EXPECT_TRUE(constraint_system.selector_map().empty());
  std::vector<Gate<F>> expected_gates;
  std::vector<std::unique_ptr<Expression<F>>> is_zero_polys;
  {
    std::unique_ptr<Expression<F>> poly = ExpressionFactory<F>::Product(
        ExpressionFactory<F>::Product(
            ExpressionFactory<F>::Selector(Selector::Simple(0)),
            ExpressionFactory<F>::Sum(
                ExpressionFactory<F>::Advice(
                    AdviceQuery(0, Rotation::Cur(), AdviceColumnKey(0))),
                ExpressionFactory<F>::Negated(ExpressionFactory<F>::Advice(
                    AdviceQuery(1, Rotation::Cur(), AdviceColumnKey(1)))))),
        ExpressionFactory<F>::Sum(
            ExpressionFactory<F>::Constant(F::One()),
            ExpressionFactory<F>::Negated(ExpressionFactory<F>::Product(
                ExpressionFactory<F>::Sum(
                    ExpressionFactory<F>::Advice(
                        AdviceQuery(0, Rotation::Cur(), AdviceColumnKey(0))),
                    ExpressionFactory<F>::Negated(ExpressionFactory<F>::Advice(
                        AdviceQuery(1, Rotation::Cur(), AdviceColumnKey(1))))),
                ExpressionFactory<F>::Advice(
                    AdviceQuery(2, Rotation::Cur(), AdviceColumnKey(4)))))));
    is_zero_polys.push_back(std::move(poly));
  }
  expected_gates.push_back(Gate<F>("is_zero", {""}, std::move(is_zero_polys),
                                   {Selector::Simple(0)},
                                   {
                                       {AdviceColumnKey(0), Rotation::Cur()},
                                       {AdviceColumnKey(1), Rotation::Cur()},
                                       {AdviceColumnKey(4), Rotation::Cur()},
                                   }));

  std::vector<std::unique_ptr<Expression<F>>> function_polys;
  function_polys.emplace_back(ExpressionFactory<F>::Product(
      ExpressionFactory<F>::Selector(Selector::Simple(0)),
      ExpressionFactory<F>::Product(
          ExpressionFactory<F>::Sum(
              ExpressionFactory<F>::Constant(F::One()),
              ExpressionFactory<F>::Negated(ExpressionFactory<F>::Product(
                  ExpressionFactory<F>::Sum(
                      ExpressionFactory<F>::Advice(
                          AdviceQuery(0, Rotation::Cur(), AdviceColumnKey(0))),
                      ExpressionFactory<F>::Negated(
                          ExpressionFactory<F>::Advice(AdviceQuery(
                              1, Rotation::Cur(), AdviceColumnKey(1))))),
                  ExpressionFactory<F>::Advice(
                      AdviceQuery(2, Rotation::Cur(), AdviceColumnKey(4)))))),
          ExpressionFactory<F>::Sum(
              ExpressionFactory<F>::Advice(
                  AdviceQuery(4, Rotation::Cur(), AdviceColumnKey(3))),
              ExpressionFactory<F>::Negated(ExpressionFactory<F>::Advice(
                  AdviceQuery(3, Rotation::Cur(), AdviceColumnKey(2))))))));
  function_polys.emplace_back(ExpressionFactory<F>::Product(
      ExpressionFactory<F>::Product(
          ExpressionFactory<F>::Selector(Selector::Simple(0)),
          ExpressionFactory<F>::Sum(
              ExpressionFactory<F>::Constant(F::One()),
              ExpressionFactory<F>::Negated(ExpressionFactory<F>::Sum(
                  ExpressionFactory<F>::Constant(F::One()),
                  ExpressionFactory<F>::Negated(ExpressionFactory<F>::Product(
                      ExpressionFactory<F>::Sum(
                          ExpressionFactory<F>::Advice(AdviceQuery(
                              0, Rotation::Cur(), AdviceColumnKey(0))),
                          ExpressionFactory<F>::Negated(
                              ExpressionFactory<F>::Advice(AdviceQuery(
                                  1, Rotation::Cur(), AdviceColumnKey(1))))),
                      ExpressionFactory<F>::Advice(AdviceQuery(
                          2, Rotation::Cur(), AdviceColumnKey(4))))))))),
      ExpressionFactory<F>::Sum(
          ExpressionFactory<F>::Advice(
              AdviceQuery(4, Rotation::Cur(), AdviceColumnKey(3))),
          ExpressionFactory<F>::Negated(ExpressionFactory<F>::Sum(
              ExpressionFactory<F>::Advice(
                  AdviceQuery(0, Rotation::Cur(), AdviceColumnKey(0))),
              ExpressionFactory<F>::Negated(ExpressionFactory<F>::Advice(
                  AdviceQuery(1, Rotation::Cur(), AdviceColumnKey(1)))))))));
  expected_gates.push_back(Gate<F>("f(a, b, c) = if a == b {c} else {a - b}",
                                   {"", ""}, std::move(function_polys),
                                   {Selector::Simple(0)},
                                   {
                                       {AdviceColumnKey(0), Rotation::Cur()},
                                       {AdviceColumnKey(1), Rotation::Cur()},
                                       {AdviceColumnKey(2), Rotation::Cur()},
                                       {AdviceColumnKey(3), Rotation::Cur()},
                                   }));
  EXPECT_EQ(constraint_system.gates(), expected_gates);
  std::vector<AdviceQueryData> expected_advice_queries = {
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(0)),
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(1)),
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(4)),
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(2)),
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(3)),
  };
  EXPECT_EQ(constraint_system.advice_queries(), expected_advice_queries);
  std::vector<size_t> expected_num_advice_queries = {1, 1, 1, 1, 1};
  EXPECT_EQ(constraint_system.num_advice_queries(),
            expected_num_advice_queries);
  EXPECT_TRUE(constraint_system.instance_queries().empty());
  EXPECT_TRUE(constraint_system.fixed_queries().empty());
  EXPECT_TRUE(constraint_system.permutation().columns().empty());
  EXPECT_TRUE(constraint_system.lookups().empty());
  EXPECT_TRUE(constraint_system.general_column_annotations().empty());
  EXPECT_FALSE(constraint_system.minimum_degree().has_value());
}

TEST_F(Fibonacci3V1CircuitTest, Synthesize) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));
  const Domain* domain = prover_->domain();

  ConstraintSystem<F> constraint_system;
  Fibonacci3Config config =
      Fibonacci3Circuit<F, V1FloorPlanner>::Configure(constraint_system);
  Assembly<PCS> assembly =
      VerifyingKey<PCS>::CreateAssembly(domain, constraint_system);

  F a(10);
  F b(12);
  F c(15);
  Fibonacci3Circuit<F, V1FloorPlanner> circuit(std::move(a), std::move(b),
                                               std::move(c));
  typename Fibonacci3Circuit<F, V1FloorPlanner>::FloorPlanner floor_planner;
  floor_planner.Synthesize(&assembly, circuit, std::move(config),
                           constraint_system.constants());

  EXPECT_TRUE(assembly.fixed_columns().empty());

  EXPECT_TRUE(assembly.permutation().columns().empty());

  const CycleStore& cycle_store = assembly.permutation().cycle_store();
  EXPECT_TRUE(cycle_store.mapping().IsEmpty());
  EXPECT_TRUE(cycle_store.aux().IsEmpty());
  EXPECT_TRUE(cycle_store.sizes().IsEmpty());

  // clang-format off
  std::vector<std::vector<bool>> expected_selectors = {
      { true, false, false, false, false, false, false, false,
       false, false, false, false, false, false, false, false}};
  // clang-format on
  EXPECT_EQ(assembly.selectors(), expected_selectors);
  EXPECT_EQ(assembly.usable_rows(), base::Range<size_t>::Until(10));
}

TEST_F(Fibonacci3V1CircuitTest, LoadVerifyingKey) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  F a(10);
  F b(12);
  F c(15);
  Fibonacci3Circuit<F, V1FloorPlanner> circuit(std::move(a), std::move(b),
                                               std::move(c));

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  EXPECT_TRUE(vkey.permutation_verifying_key().commitments().empty());

  std::vector<Commitment> expected_fixed_commitments;
  {
    std::vector<Point> points = {
        {"0x0006d246b1045b5cf7ef706abdca51d7f88992335199a85f594360f81b3435a9",
         "0x0ce5093fe91a56ef9d54ef73d457aa726be43905202132cd98cb1893cfee96a6"},
    };
    expected_fixed_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(vkey.fixed_commitments(), expected_fixed_commitments);

  F expected_transcript_repr = F::FromHexString(
      "0x015bd699cd46cf1250b4b2b2553645628551f6c76bd3e5cc995a355972340be4");
  EXPECT_EQ(vkey.transcript_repr(), expected_transcript_repr);
}

TEST_F(Fibonacci3V1CircuitTest, LoadProvingKey) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  F a(10);
  F b(12);
  F c(15);
  Fibonacci3Circuit<F, V1FloorPlanner> circuit(std::move(a), std::move(b),
                                               std::move(c));

  for (size_t i = 0; i < 2; ++i) {
    ProvingKey<PCS> pkey;
    bool load_verifying_key = i == 0;
    SCOPED_TRACE(
        absl::Substitute("load_verifying_key: $0", load_verifying_key));
    if (load_verifying_key) {
      VerifyingKey<PCS> vkey;
      ASSERT_TRUE(vkey.Load(prover_.get(), circuit));
      ASSERT_TRUE(
          pkey.LoadWithVerifyingKey(prover_.get(), circuit, std::move(vkey)));
    } else {
      ASSERT_TRUE(pkey.Load(prover_.get(), circuit));
    }

    Poly expected_l_first;
    {
      std::vector<std::string_view> poly = {
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
      };
      expected_l_first = CreatePoly(poly);
    }
    EXPECT_EQ(pkey.l_first(), expected_l_first);

    Poly expected_l_last;
    {
      std::vector<std::string_view> poly = {
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2014447de15a99b6df03833e95f96ae1299c9ec6ff990b6e75fa3b3b04846a57",
          "0x0f1f5883e65f820d14d56342dc92fd12a944d4cbbdce5377b7439bd07108fc9d",
          "0x02b337de1c8c14f22ec9b9e2f96afef3652627366f8170a0a948dad4ac1bd5e8",
          "0x030644e72e131a029b85045b68181585d2833e84879b9709143e1f593f000000",
          "0x105009f4ffd70672d94cc277eb87ed7bfe9749817a206522cde7ba58eb7b95aa",
          "0x2144f5eefad21e1ca37ae273a4ee5b4a7eef137cbbeb1d198c9e59c37ef70364",
          "0x2db11694c4a58b3789868bd388165969c30dc1120a37fff09a991abf43e42a19",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2014447de15a99b6df03833e95f96ae1299c9ec6ff990b6e75fa3b3b04846a57",
          "0x0f1f5883e65f820d14d56342dc92fd12a944d4cbbdce5377b7439bd07108fc9d",
          "0x02b337de1c8c14f22ec9b9e2f96afef3652627366f8170a0a948dad4ac1bd5e8",
          "0x030644e72e131a029b85045b68181585d2833e84879b9709143e1f593f000000",
          "0x105009f4ffd70672d94cc277eb87ed7bfe9749817a206522cde7ba58eb7b95aa",
          "0x2144f5eefad21e1ca37ae273a4ee5b4a7eef137cbbeb1d198c9e59c37ef70364",
          "0x2db11694c4a58b3789868bd388165969c30dc1120a37fff09a991abf43e42a19",
      };
      expected_l_last = CreatePoly(poly);
    }
    EXPECT_EQ(pkey.l_last(), expected_l_last);

    Poly expected_l_active_row;
    {
      std::vector<std::string_view> poly = {
          "0x12259d6b14729c0fa51e1a2470908122ef13771b2da58a367974bc177a000001",
          "0x1a8133201ba2fe22aff30c7fd04fadcb17ceab0715a13371b06d35acd9695b3d",
          "0x0d49c50dd1c3ec703dc7be1c836fd7f62c140afcf284ce19b9a99affac7b95aa",
          "0x117dae38a05bcb8c7c290ee16cec493d73ef8dafa5c61fa4a2efd9a39e63abf4",
          "0x0c19139cb84c680a79505ee7747ae78cd6c196473632bc6ea3057c773208fc9d",
          "0x136156e428a2662bc2fddfd3b39f6475dafecb8699a611f3da6edd22c3479af1",
          "0x2aaad1ad96927134ee0187781ffe43e3f08a828d829c68e7865afb6604e42a19",
          "0x28d7d254c17b7ea40ebc4659996adacebd0d8f52d021284040d407c2f33b896f",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x1ef7221577b1e8f8cebdcb2fcd10296b6d9d43267b395c820757c0fc87681d81",
          "0x1d0dff96b3477fb4437e7ee32de1555b5719604277fd746561bc1be1c5846a57",
          "0x1be193603aacb384678281793fa8f20b949cf5976d9493017ba8a357ee49da57",
          "0x1e3eb107ccbf041a07f5de183cd645c4ac6bd4f8344f861078603a6a3ff70364",
          "0x20080468beb85b16c9f71b3ea2ce10fb6cdc81c346721c888ebc9109900adec6",
          "0x30114169cfaa9b194b94fb3e12d441cabad6d0fa619f4a28d8ecb10f5d1bd5e9",
          "0x2edcc3ce4ec47abd9b83b31a4db9571236223b590c30997fd30ce24f7bf2fdd6",
      };
      expected_l_active_row = CreatePoly(poly);
    }
    EXPECT_EQ(pkey.l_active_row(), expected_l_active_row);

    std::vector<Evals> expected_fixed_columns;
    {
      // clang-format off
      std::vector<std::vector<std::string_view>> evals = {{
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
      }};
      // clang-format on
      expected_fixed_columns = CreateColumns(evals);
    }
    EXPECT_EQ(pkey.fixed_columns(), expected_fixed_columns);

    std::vector<Poly> expected_fixed_polys;
    {
      // clang-format off
      std::vector<std::vector<std::string_view>> polys = {{
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
          "0x2d5e098bb31e86271ccb415b196942d755b0a9c3f21dd9882fa3d63ab1000001",
      }};
      // clang-format on
      expected_fixed_polys = CreatePolys(polys);
    }
    EXPECT_EQ(pkey.fixed_polys(), expected_fixed_polys);

    EXPECT_TRUE(pkey.permutation_proving_key().permutations().empty());

    EXPECT_TRUE(pkey.permutation_proving_key().polys().empty());
  }
}

TEST_F(Fibonacci3V1CircuitTest, CreateProof) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  F a(10);
  F b(12);
  F c(15);
  Fibonacci3Circuit<F, V1FloorPlanner> circuit(std::move(a), std::move(b),
                                               std::move(c));
  std::vector<Fibonacci3Circuit<F, V1FloorPlanner>> circuits = {
      std::move(circuit)};

  std::vector<Evals> instance_columns;
  std::vector<std::vector<Evals>> instance_columns_vec = {
      std::move(instance_columns)};

  ProvingKey<PCS> pkey;
  ASSERT_TRUE(pkey.Load(prover_.get(), circuit));
  prover_->CreateProof(pkey, std::move(instance_columns_vec), circuits);

  std::vector<uint8_t> proof = prover_->GetWriter()->buffer().owned_buffer();
  std::vector<uint8_t> expected_proof(std::begin(kExpectedProof),
                                      std::end(kExpectedProof));
  EXPECT_THAT(proof, testing::ContainerEq(expected_proof));
}

TEST_F(Fibonacci3V1CircuitTest, VerifyProof) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  F a(10);
  F b(12);
  F c(15);
  Fibonacci3Circuit<F, V1FloorPlanner> circuit(std::move(a), std::move(b),
                                               std::move(c));

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  std::vector<uint8_t> owned_proof(std::begin(kExpectedProof),
                                   std::end(kExpectedProof));
  std::unique_ptr<Verifier<PCS>> verifier =
      CreateVerifier(CreateBufferWithProof(absl::MakeSpan(owned_proof)));

  std::vector<Evals> instance_columns;
  std::vector<std::vector<Evals>> instance_columns_vec = {
      std::move(instance_columns)};

  Proof<F, Commitment> proof;
  F h_eval;
  ASSERT_TRUE(verifier->VerifyProofForTesting(vkey, instance_columns_vec,
                                              &proof, &h_eval));

  std::vector<std::vector<Commitment>> expected_advice_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x2f0344888f0526e1ac006b73ac101e295d9335c96dd54356b31fe28479f8e721",
         "0x03d9c8b82e3fdc536e0dc94e102b13f6f02b68a87483ee77ff9d315b2c7d60de"},
        {"0x1412168c94e087a95499f299a76337a1231305d114bb71c0401ecaa7f6786ede",
         "0x13c23c6582942c390a5a788602da1db2a55a2424d0da56357809ac5b2e441f65"},
        {"0x068261f08c1a54f57ada6457fd7c67781d4c2c24573e4952b706d814c9c49105",
         "0x145e88698efe8890d0ac0148f7323474c9a32b7ee6eca0ad95915643c1bd689d"},
        {"0x052767c246ff3a19021693323bb900c235816fdb508bdccbe3821a3c16c8685c",
         "0x1993a757677c0957fb51ca2cccfcef84c924a13fdf4d91786ce1a1c6f733199c"},
        {"0x04d2b14222eba6f926d62f2338facd5afeb6b9afb677c2b5c86f25293f0ec78f",
         "0x22c64d91d4b0dc41516f1f8c3828a535c7918b49936ac04e5b57e466d0face1f"},
    };
    expected_advice_commitments_vec.push_back(CreateCommitments(points));
  }
  EXPECT_EQ(proof.advices_commitments_vec, expected_advice_commitments_vec);

  EXPECT_TRUE(proof.challenges.empty());

  F expected_theta = F::FromHexString(
      "0x1d236783892b7620458f6e9be6c14506f6742ec8f87d97140ac435b4ed3f427f");
  EXPECT_EQ(proof.theta, expected_theta);

  ASSERT_EQ(proof.lookup_permuted_commitments_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_permuted_commitments_vec[0].empty());

  F expected_beta = F::FromHexString(
      "0x07b96df44c3764c3e8dc57ab4940ad391d2efeec1975dd96a079a01b4da75ea8");
  EXPECT_EQ(proof.beta, expected_beta);

  F expected_gamma = F::FromHexString(
      "0x06ebbecadc3b038340ae9085093f30d92f1f14e74994803cc8cd5d9666710471");
  EXPECT_EQ(proof.gamma, expected_gamma);

  ASSERT_EQ(proof.permutation_product_commitments_vec.size(), 1);
  EXPECT_TRUE(proof.permutation_product_commitments_vec[0].empty());

  ASSERT_EQ(proof.lookup_product_commitments_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_product_commitments_vec[0].empty());

  Commitment expected_vanishing_random_poly_commitment;
  {
    expected_vanishing_random_poly_commitment = CreateCommitment(
        {"0x0000000000000000000000000000000000000000000000000000000000000001",
         "0x0000000000000000000000000000000000000000000000000000000000000002"});
  }
  EXPECT_EQ(proof.vanishing_random_poly_commitment,
            expected_vanishing_random_poly_commitment);

  F expected_y = F::FromHexString(
      "0x13335bf2343d72549f81053d7b3bfb8f96e841f4f808481bee7a57535f73a9b1");
  EXPECT_EQ(proof.y, expected_y);

  std::vector<Commitment> expected_vanishing_h_poly_commitments;
  {
    std::vector<Point> points = {
        {"0x03728a7fd094a31c518e3894d243599df9957a64d1ebd89b4f0f4dc47c0c6b96",
         "0x0681af821b554ce49a9294c74df720c22ef43019481d7ae95002da903997157d"},
        {"0x05d80cd28f07d664fb3811c45054eb3c3f05671324b5b758a3f25e64289ba417",
         "0x038264bf052f5d010ed073b24bd2515f939d3ec581a5d7b7adca5a1ae3bed591"},
        {"0x082fb07552037e7b47ec8bc164211493176afae306b2ce7dcb7869faa8850b77",
         "0x0a5630351a4ebabb9f86911190333a8174d347b7d265cb06f70bbacc63daf954"},
    };
    expected_vanishing_h_poly_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(proof.vanishing_h_poly_commitments,
            expected_vanishing_h_poly_commitments);

  F expected_x = F::FromHexString(
      "0x2112e9bcdae699637ec8e70595c2713af32bd8aa684111af55354988104f8ce9");
  EXPECT_EQ(proof.x, expected_x);

  std::vector<std::vector<F>> expected_advice_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x16e20bf820e183585c5ebc40e8a7a94d5f45a1c28d772e8119fd415f706f3b4a",
        "0x0193517b0438c7dc1e92542cf0eb85b5c01e85c047a2b968390215f65d31b21d",
        "0x19ddae867ca7b346c21b41cf19e05d57c85e2a2b6a3157fcde677cba39724b74",
        "0x2a33af6bab061ee0565820a0bf9354de0db1b829957a329ccd5c4536a855645b",
        "0x058d7f8e294107c4a6385b95328c75c3a19490f6c3459b62943a651a13e07255",
    };
    expected_advice_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.advice_evals_vec, expected_advice_evals_vec);

  std::vector<F> expected_fixed_evals;
  {
    std::vector<std::string_view> evals = {
        "0x0d8ac9fae2447256bd41eed144e29a62c486662319f27dbc317365156e613b6a",
    };
    expected_fixed_evals = CreateEvals(evals);
  }
  EXPECT_EQ(proof.fixed_evals, expected_fixed_evals);

  F expected_vanishing_random_eval = F::FromHexString(
      "0x0000000000000000000000000000000000000000000000000000000000000001");
  EXPECT_EQ(proof.vanishing_random_eval, expected_vanishing_random_eval);

  EXPECT_TRUE(proof.common_permutation_evals.empty());

  ASSERT_EQ(proof.permutation_product_evals_vec.size(), 1);
  EXPECT_TRUE(proof.permutation_product_evals_vec[0].empty());

  ASSERT_EQ(proof.permutation_product_next_evals_vec.size(), 1);
  EXPECT_TRUE(proof.permutation_product_next_evals_vec[0].empty());

  ASSERT_EQ(proof.permutation_product_last_evals_vec.size(), 1);
  EXPECT_TRUE(proof.permutation_product_last_evals_vec[0].empty());

  ASSERT_EQ(proof.lookup_product_evals_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_product_evals_vec[0].empty());

  ASSERT_EQ(proof.lookup_product_next_evals_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_product_next_evals_vec[0].empty());

  ASSERT_EQ(proof.lookup_permuted_input_evals_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_permuted_input_evals_vec[0].empty());

  ASSERT_EQ(proof.lookup_permuted_input_inv_evals_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_permuted_input_inv_evals_vec[0].empty());

  ASSERT_EQ(proof.lookup_permuted_table_evals_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_permuted_table_evals_vec[0].empty());
}

}  // namespace tachyon::zk::halo2
