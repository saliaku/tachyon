#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tachyon/zk/plonk/circuit/examples/circuit_test.h"
#include "tachyon/zk/plonk/circuit/examples/fibonacci1_circuit.h"
#include "tachyon/zk/plonk/circuit/floor_planner/v1/v1_floor_planner.h"
#include "tachyon/zk/plonk/halo2/pinned_verifying_key.h"
#include "tachyon/zk/plonk/keys/proving_key.h"

namespace tachyon::zk::halo2 {

namespace {

constexpr uint8_t kExpectedProof[] = {
    197, 90,  54,  138, 254, 187, 187, 144, 27,  39,  20,  39,  15,  181, 249,
    52,  63,  84,  95,  51,  45,  242, 210, 62,  170, 158, 176, 93,  60,  111,
    37,  2,   93,  88,  146, 144, 43,  179, 6,   218, 194, 31,  174, 181, 184,
    44,  226, 22,  117, 37,  50,  100, 239, 5,   38,  36,  177, 245, 5,   62,
    67,  52,  53,  46,  84,  190, 108, 245, 192, 106, 51,  107, 85,  215, 124,
    33,  178, 186, 27,  105, 218, 50,  59,  250, 73,  229, 251, 140, 191, 110,
    167, 71,  86,  11,  127, 25,  45,  2,   182, 108, 63,  131, 148, 230, 218,
    182, 207, 27,  85,  212, 148, 232, 122, 42,  105, 244, 149, 17,  102, 186,
    185, 52,  213, 124, 227, 121, 165, 160, 249, 91,  247, 169, 242, 205, 178,
    62,  126, 181, 61,  210, 1,   222, 80,  241, 65,  92,  49,  56,  58,  39,
    245, 16,  99,  17,  67,  81,  97,  183, 59,  136, 118, 224, 25,  117, 192,
    3,   43,  32,  194, 243, 249, 220, 15,  194, 233, 150, 207, 146, 148, 125,
    33,  221, 220, 102, 110, 105, 175, 188, 106, 29,  169, 145, 3,   128, 104,
    133, 179, 157, 63,  1,   77,  110, 141, 66,  96,  59,  38,  134, 63,  254,
    79,  106, 157, 247, 206, 64,  126, 143, 89,  189, 48,  106, 172, 39,  1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   105, 233, 126, 25,  199, 110, 55,  48,  104, 73,  69,  182, 10,  106,
    231, 187, 107, 215, 185, 253, 92,  179, 223, 123, 12,  67,  70,  89,  196,
    236, 235, 2,   116, 247, 138, 127, 23,  235, 140, 97,  179, 224, 212, 149,
    242, 11,  218, 82,  110, 33,  183, 208, 116, 10,  17,  119, 229, 246, 181,
    120, 72,  184, 122, 39,  43,  54,  12,  54,  154, 36,  140, 184, 41,  56,
    62,  45,  1,   244, 153, 157, 92,  141, 73,  187, 214, 189, 103, 228, 219,
    78,  50,  242, 24,  52,  181, 39,  183, 71,  181, 99,  169, 179, 69,  207,
    23,  126, 161, 155, 165, 194, 40,  91,  246, 223, 163, 151, 212, 157, 38,
    237, 27,  254, 80,  56,  84,  220, 204, 46,  224, 83,  39,  211, 161, 232,
    104, 189, 96,  183, 133, 178, 95,  99,  210, 222, 110, 128, 121, 104, 205,
    75,  110, 41,  245, 180, 27,  220, 180, 17,  46,  46,  128, 180, 58,  82,
    85,  184, 17,  79,  211, 37,  181, 240, 233, 185, 70,  75,  135, 123, 171,
    86,  38,  160, 53,  166, 158, 49,  92,  234, 37,  114, 197, 14,  1,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    230, 154, 5,   235, 93,  62,  76,  69,  239, 199, 31,  216, 11,  129, 144,
    236, 224, 106, 124, 75,  149, 103, 99,  204, 25,  198, 117, 24,  40,  7,
    200, 5,   150, 216, 181, 160, 57,  251, 85,  157, 227, 86,  180, 81,  151,
    19,  107, 133, 8,   55,  175, 32,  37,  140, 207, 208, 194, 9,   89,  131,
    173, 174, 10,  10,  228, 228, 197, 128, 106, 53,  237, 33,  130, 207, 167,
    249, 85,  123, 36,  117, 205, 23,  34,  151, 111, 221, 165, 218, 56,  97,
    231, 206, 123, 72,  49,  17,  64,  136, 244, 160, 109, 183, 234, 134, 223,
    198, 107, 6,   107, 16,  113, 210, 121, 129, 154, 173, 75,  186, 161, 45,
    151, 14,  229, 64,  95,  202, 40,  20,  56,  229, 39,  131, 8,   100, 147,
    239, 153, 67,  149, 30,  57,  34,  133, 245, 2,   132, 185, 10,  79,  24,
    136, 225, 44,  29,  11,  26,  249, 131, 39,  31,  223, 143, 131, 98,  57,
    3,   0,   61,  113, 143, 59,  109, 107, 132, 49,  171, 44,  76,  229, 15,
    182, 16,  135, 54,  24,  228, 44,  9,   56,  2,   203, 45,  91,  57,  171,
    199, 129, 188, 4,   235, 98,  194, 212, 67,  143, 116, 110, 195, 68,  241,
    217, 139, 75,  186, 8,   119, 12,  105, 223, 47,  75,  249, 170, 24,  215,
    19,  216, 141, 200, 235, 175, 167, 216, 79,  213, 76,  112, 158, 23,  143,
    76,  54,  205, 220, 22,  47,  112, 250, 10,  21,  173, 186, 25,  156, 65,
    10,  70,  166, 65,  157, 102, 98,  90,  46,  0,   49,  185, 7,   223, 136,
    71,  149, 184, 232, 151, 105, 232, 133, 163, 16,  155, 126, 231, 159, 127,
    167, 154, 18,  107, 182, 228, 64,  36,  251, 120, 235, 139, 90,  168, 252,
    154, 128, 151, 101, 84,  142, 252, 155, 120, 194, 31,  58,  113, 41,  93,
    44,  165, 155, 227, 6,   3,   184, 135, 167, 94,  105, 168, 215, 118, 73,
    0,   7,   188, 152, 42,  153, 107, 209, 67,  121, 253, 156, 171, 100, 244,
    82,  17,  130, 106, 62,  6,   2,   132, 239, 150, 250, 60,  183, 63,  106,
    65,  23,  116, 39,  12,  23,  197, 178, 26,  63,  157, 155, 178, 192, 53,
    40,  83,  164, 174, 177, 165, 154, 92,  15,  138, 131, 30,  250, 149, 64,
    56,  186, 12,  135, 233, 65,  14,  198, 167, 85,  202, 149, 34,  120, 79,
    117, 156, 142, 92,  147, 190, 146, 226, 94,  133, 41,  82,  76,  97,  38,
    27,  66,  82,  227, 113, 208, 226, 92,  57,  22,  21,  0,   5,   22,  139,
    226, 194, 165, 139, 157, 135, 229, 61,  82,  183, 38,  100, 19,  101, 138,
    129, 46,  21,  193, 125, 194, 35,  103, 94,  194, 196, 27,  70,  64,  95,
    245, 176, 164, 68,  181, 194, 66,  243, 188, 123, 31,  73,  182, 229, 4,
    119, 56,  243, 119, 188, 252, 158, 78,  27,  56,  61,  167, 91,  87,  179,
    128, 92,  114, 108, 196, 26,  117, 75,  117, 84,  222, 46,  3,   98,  189,
    249, 11,  21,  212, 80,  201, 106, 46,  77,  207, 175, 136, 82,  241, 78,
    246, 30,  107, 19,  133, 241, 99,  103, 205, 158, 43,  93,  2,   196, 120,
    114, 142, 140, 137};

class FibonacciV1CircuitTest : public CircuitTest {};

}  // namespace

TEST_F(FibonacciV1CircuitTest, Configure) {
  ConstraintSystem<F> constraint_system;
  Fibonacci1Config<F> config =
      Fibonacci1Circuit<F, V1FloorPlanner>::Configure(constraint_system);
  EXPECT_EQ(config.col_a, AdviceColumnKey(0));
  EXPECT_EQ(config.col_b, AdviceColumnKey(1));
  EXPECT_EQ(config.col_c, AdviceColumnKey(2));
  EXPECT_EQ(config.instance, InstanceColumnKey(0));
  EXPECT_EQ(config.selector, Selector::Simple(0));
  EXPECT_EQ(constraint_system.num_fixed_columns(), 0);
  EXPECT_EQ(constraint_system.num_advice_columns(), 3);
  EXPECT_EQ(constraint_system.num_instance_columns(), 1);
  EXPECT_EQ(constraint_system.num_selectors(), 1);
  EXPECT_EQ(constraint_system.num_challenges(), 0);
  std::vector<Phase> expected_advice_column_phases = {kFirstPhase, kFirstPhase,
                                                      kFirstPhase};
  EXPECT_EQ(constraint_system.advice_column_phases(),
            expected_advice_column_phases);
  EXPECT_TRUE(constraint_system.challenge_phases().empty());
  EXPECT_TRUE(constraint_system.selector_map().empty());
  std::vector<Gate<F>> expected_gates;
  std::vector<std::unique_ptr<Expression<F>>> polys;
  {
    std::unique_ptr<Expression<F>> poly = ExpressionFactory<F>::Product(
        ExpressionFactory<F>::Selector(config.selector),
        ExpressionFactory<F>::Sum(
            ExpressionFactory<F>::Sum(ExpressionFactory<F>::Advice(AdviceQuery(
                                          0, Rotation::Cur(), config.col_a)),
                                      ExpressionFactory<F>::Advice(AdviceQuery(
                                          1, Rotation::Cur(), config.col_b))),
            ExpressionFactory<F>::Negated(ExpressionFactory<F>::Advice(
                AdviceQuery(2, Rotation::Cur(), config.col_c)))));
    polys.push_back(std::move(poly));
  }
  expected_gates.push_back(Gate<F>("add", {""}, std::move(polys),
                                   {Selector::Simple(0)},
                                   {
                                       {AdviceColumnKey(0), Rotation::Cur()},
                                       {AdviceColumnKey(1), Rotation::Cur()},
                                       {AdviceColumnKey(2), Rotation::Cur()},
                                   }));
  EXPECT_EQ(constraint_system.gates(), expected_gates);
  std::vector<AdviceQueryData> expected_advice_queries = {
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(0)),
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(1)),
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(2)),
  };
  EXPECT_EQ(constraint_system.advice_queries(), expected_advice_queries);
  std::vector<size_t> expected_num_advice_queries = {1, 1, 1};
  EXPECT_EQ(constraint_system.num_advice_queries(),
            expected_num_advice_queries);
  std::vector<InstanceQueryData> expected_instance_queries = {
      InstanceQueryData(Rotation::Cur(), InstanceColumnKey(0)),
  };
  EXPECT_EQ(constraint_system.instance_queries(), expected_instance_queries);
  EXPECT_TRUE(constraint_system.fixed_queries().empty());
  std::vector<AnyColumnKey> expected_permutation_columns = {
      AdviceColumnKey(0),
      AdviceColumnKey(1),
      AdviceColumnKey(2),
      InstanceColumnKey(0),
  };
  EXPECT_EQ(constraint_system.permutation().columns(),
            expected_permutation_columns);
  EXPECT_TRUE(constraint_system.lookups().empty());
  EXPECT_TRUE(constraint_system.general_column_annotations().empty());
  EXPECT_FALSE(constraint_system.minimum_degree().has_value());
}

TEST_F(FibonacciV1CircuitTest, Synthesize) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));
  const Domain* domain = prover_->domain();

  ConstraintSystem<F> constraint_system;
  Fibonacci1Config config =
      Fibonacci1Circuit<F, V1FloorPlanner>::Configure(constraint_system);
  Assembly<PCS> assembly =
      VerifyingKey<PCS>::CreateAssembly(domain, constraint_system);

  Fibonacci1Circuit<F, V1FloorPlanner> circuit;
  typename Fibonacci1Circuit<F, V1FloorPlanner>::FloorPlanner floor_planner;
  floor_planner.Synthesize(&assembly, circuit, std::move(config),
                           constraint_system.constants());

  EXPECT_TRUE(assembly.fixed_columns().empty());

  std::vector<AnyColumnKey> expected_columns = {
      AdviceColumnKey(0),
      AdviceColumnKey(1),
      AdviceColumnKey(2),
      InstanceColumnKey(0),
  };
  EXPECT_EQ(assembly.permutation().columns(), expected_columns);

  const CycleStore& cycle_store = assembly.permutation().cycle_store();
  // clang-format off
  CycleStore::Table<Label> expected_mapping({
      {{1, 1}, {1, 2},  {1, 3},  {1, 4},  {1, 5},  {1, 6},  {3, 1},  {3, 0},
       {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}},
      {{2, 1}, {2, 2},  {2, 3},  {2, 4},  {2, 5},  {2, 6},  {2, 7},  {0, 6},
       {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 14}, {1, 15}},
      {{3, 2}, {1, 0},  {0, 0},  {0, 1},  {0, 2},  {0, 3},  {0, 4},  {0, 5},
       {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 12}, {2, 13}, {2, 14}, {2, 15}},
      {{0, 7}, {1, 7},  {2, 0},  {3, 3},  {3, 4},  {3, 5},  {3, 6},  {3, 7},
       {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {3, 14}, {3, 15}},
  });
  CycleStore::Table<Label> expected_aux({
      {{1, 1}, {1, 2},  {1, 3},  {1, 4},  {1, 5},  {1, 6},  {1, 7},  {0, 7},
       {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}},
      {{1, 0}, {1, 1},  {1, 2},  {1, 3},  {1, 4},  {1, 5},  {1, 6},  {1, 7},
       {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 14}, {1, 15}},
      {{2, 0}, {1, 0},  {1, 1},  {1, 2},  {1, 3},  {1, 4},  {1, 5},  {1, 6},
       {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 12}, {2, 13}, {2, 14}, {2, 15}},
      {{0, 7}, {1, 7},  {2, 0},  {3, 3},  {3, 4},  {3, 5},  {3, 6},  {3, 7},
       {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {3, 14}, {3, 15}},
  });
  CycleStore::Table<size_t> expected_sizes({
      {1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1},
      {2, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1},
      {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  });
  // clang-format on
  EXPECT_EQ(cycle_store.mapping(), expected_mapping);
  EXPECT_EQ(cycle_store.aux(), expected_aux);
  EXPECT_EQ(cycle_store.sizes(), expected_sizes);

  // clang-format off
  std::vector<std::vector<bool>> expected_selectors = {
      { true,  true,  true,  true,  true,  true,  true,  true,
       false, false, false, false, false, false, false, false}};
  // clang-format on
  EXPECT_EQ(assembly.selectors(), expected_selectors);
  EXPECT_EQ(assembly.usable_rows(), base::Range<size_t>::Until(10));
}

TEST_F(FibonacciV1CircuitTest, LoadVerifyingKey) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  Fibonacci1Circuit<F, V1FloorPlanner> circuit;

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  std::vector<Commitment> expected_permutation_verifying_key;
  {
    std::vector<Point> points = {
        {"0x0d753ca469cfea858cfa6ec91acd016399cd294f61ff0d43ebb968d2524a713c",
         "0x2550d6fb1e5c470ba3e6017989a6d380a4c0c6be14ba6e4275dae09e16a23284"},
        {"0x1357bd4a7abf61580c4d6830f369d242e2268747198bfe5e41cc960f5c475f77",
         "0x29ca42708ce07d2576298db193d3fd9bf87739177d4bf2b831841798d8b2b50d"},
        {"0x1eba9015d88cec13d423a6c37c5edd453a72e087fcfb36461bf75dd35ebe344a",
         "0x22c70d284caba78df7da1a1eb8607e5fa298a4e8ff21b8bfeb7b0772f4cb2c71"},
        {"0x1dd1199d21ff272e331fca7683198fed95caea81dd571af67753b96d29111b1d",
         "0x0274945484229b0f6dcff00e591c859ac06bdb68ccd0bf25425ccdc266df1329"},
    };
    expected_permutation_verifying_key = CreateCommitments(points);
  }
  EXPECT_EQ(vkey.permutation_verifying_key().commitments(),
            expected_permutation_verifying_key);

  std::vector<Commitment> expected_fixed_commitments;
  {
    std::vector<Point> points = {
        {"0x297ff8a661d1fa1196c065b6fb7df901fb16b5b83168ddca6c7749d963cc967a",
         "0x1a7b1f2b5f3e35fc4c706ece6b8f646e95904f9aa417f8a31fd37a41da167ec1"},
    };
    expected_fixed_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(vkey.fixed_commitments(), expected_fixed_commitments);

  F expected_transcript_repr = F::FromHexString(
      "0x0cf64676a870bcf55c113fb62e896e924807935cb63a2794b382539e995f8673");
  EXPECT_EQ(vkey.transcript_repr(), expected_transcript_repr);
}

TEST_F(FibonacciV1CircuitTest, LoadProvingKey) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  Fibonacci1Circuit<F, V1FloorPlanner> circuit;

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
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
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
          "0x183227397098d014dc2822db40c0ac2e9419f4243cdcb848a1f0fac9f8000001",
          "0x1db5b8e23dbf6d914e37d939eb6b037c619b3e5ea827cdb2b030fc247bdc9dcb",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x15612923e039d29887b96603ef6ab434a2c021b8674489fcfa35d3972944e837",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x186635fde662f3b15c5e1b5fc456d148c26786b9cda347e81f7f4c2f8c9761ed",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x14a1b21f8b9c269f87d74740cf8c84a3046338799106ae503c4dfb0f0b0b250e",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x15b61284f96f4584f96ef5bee1c4a8ae7eca32c5d97b942edf17bbd266f4daf3",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x11f18ea69ea8787324e8219fecfa5c08c0c5e4859cdefa96fbe66ab1e5689e14",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x14f69b80a4d1998bf98cd6fbc1e6791ce06d4987033db882212fe34a48bb17ca",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0ca20bc2474bfe93330e63c5c5e629d521922ce0c25a74cc6b34babcf6236236",
      }};
      // clang-format on
      expected_fixed_polys = CreatePolys(polys);
    }
    EXPECT_EQ(pkey.fixed_polys(), expected_fixed_polys);

    std::vector<Evals> expected_permutations_columns;
    {
      // clang-format off
      std::vector<std::vector<std::string_view>> evals = {{
          "0x133f51f46c2a51201fbb89dc1d6868acfef0f7ce0e572ccb98e04e99eac3ea36",
          "0x1240a374ee6f71e12df10a129946ca9ba824f58a50e3a58b4c68dd5590b74ad8",
          "0x009553089042fe83ab570b54e4e64d307d8e8b20d568e782ece3926981d0a96c",
          "0x14a6c152ace4b16a42e1377e400798e15ac60320c21d90277890dbdd551d1912",
          "0x035992598be4d2ae5334f24f30355a1fca7f0e28762a178d79e993dca70eaabf",
          "0x03b645319eb70d7a692ea8c87fbcab1f292cd502071d9ffb7291ebe28356aa07",
          "0x2f0e061e83e8b12c1bdf8df7cca02483295e89f78a462ce38859661e06501815",
          "0x18afdf23e9bd9302673fc1e076a492d4d65bd18ebc4d854ed189139bab313e52",
          "0x30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000",
          "0x0f5c21d0ca65e0db9be1f670eca407d08ec5ec67626a74f892ccebcd0cf9b9f6",
          "0x0530d09118705106cbb4a786ead16926d5d174e181a26686af5448492e42a181",
          "0x1fe9a328fad7382fddb3730a89f574d14e57caeac619eeb30d24fb38a4fc6fbe",
          "0x0000000000000000b3c4d79d41a91758cb49c3517c4604a520cff123608fc9cb",
          "0x0dd360411caed09700b52c71a6655715c4d558439e2d34f4307da9a4be13c42e",
          "0x130b17119778465cfb3acaee30f81dee20710ead41671f568b11d9ab07b95a9b",
          "0x02e40daf409556c02bfc85eb303402b774954d30aeb0337eb85a71e6373428de",
      },
      {
          "0x08e7cbfea108224b0777f0558503af41585b75ab8d4d807505158f4bc8c771de",
          "0x0b6f861977ce57ddb2e647048ed9b9433cac640ba0599e264e24446765d915d3",
          "0x26f93d99832c6285d9abf8c5b896ea753ed137516972c5ddcc464b56c488d600",
          "0x086398ace043cf0db4e99f2712f392ddbd9c7b9202b2fcadf26e9b145316df9f",
          "0x04765b5102d6627cfcc389436e96bbc9aa478dcb720b546c77063077a40910ce",
          "0x2e39c17d3e15071a9341cfcea233ad5f25a14dea28716de88ae369ac2c9944ac",
          "0x2a3d1fef5cb3ce1065d222dde5f9b16d48b42597868c1a49e15cb8007c8778a4",
          "0x1d59376149b959ccbd157ac850893a6f07c2d99b3852513ab8d01be8e846a566",
          "0x2741e304be6aaf5f53641f0bacb8e9ebccd45eba1b23316bbcd39ed80acc165f",
          "0x1d24fc7e75074f099894bbda6418efb02942f07a6b6243c5ab01a6fa053c15cb",
          "0x1e23aafdf2c22e488a5f3ba3e83a8dc1800ef2be28d5cb05f779183e5f48b529",
          "0x2fcefb6a50eea1a60cf93a619c9b0b2caaa55d27a450890e56fe632a6e2f5695",
          "0x1bbd8d20344ceebf756f0e384179bf7bcd6de527b79be069cb5119b69ae2e6ef",
          "0x2d0abc19554ccd7b651b5367514bfe3d5db4da20038f5903c9f861b748f15542",
          "0x2cae0941427a92af4f219cee01c4ad3dff071346729bd095d15009b16ca955fa",
          "0x0d4615fec1d5611cd5ffa9e358e64eb494829d350acf01c136f55acac8e3624c",
      },
      {
          "0x034183d253b6b250dae0a457797029523434d2b6bc41c09b6ef409bd970e4208",
          "0x09226b6e22c6f0ca64ec26aad4c86e715b5f898e5e963f25870e56bbe533e9a2",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x21082ca216cbbf4e1c6e4f4594dd508c996dfbe1174efb98b11509c6e306460b",
          "0x2b337de1c8c14f22ec9b9e2f96afef3652627366f8170a0a948dad4ac1bd5e80",
          "0x107aab49e65a67f9da9cd2abf78be38bd9dc1d5db39f81de36bcfa5b4b039043",
          "0x30644e72e131a029048b6e193fd841045cea24f6fd736bec231204708f703636",
          "0x2290ee31c482cf92b79b1944db1c0147635e9004db8c3b9d13644bef31ec3bd3",
          "0x1cb0ed9df901b713b97ee5357df1bf9b16f16cc0d737b31e07ba77d910efc8d6",
          "0x277c827440297ddeb0d85560fc7da91bcfd8729cec6bf01c3ecc664827388e23",
          "0x24f4c8596963484c0569feb1f2a79f19eb87843cd95fd26af5bdb12c8a26ea2e",
          "0x096b10d95e053da3dea44cf0c8ea6de7e962b0f71046aab3779baa3d2b772a01",
          "0x2800b5c600edd11c0366a68f6e8dc57f6a976cb6770673e351735a7f9ce92062",
          "0x2bedf321de5b3dacbb8cbc7312ea9c937dec5a7d07ae1c24ccdbc51c4bf6ef33",
          "0x022a8cf5a31c990f250e75e7df4daafe02929a5e514802a8b8fe8be7c366bb55",
          "0x06272e83847dd219527e22d89b87a6efdf7fc2b0f32d564762853d937378875d",
      },
      {
          "0x2d8040c3a09c49698c53bfcb514d55a5b39e9b17cb093d128b8783adb8cbd723",
          "0x231e38741f5c3f0ce2509bd3289b09a893b14b136eea6ed00cec9ac9271c9db5",
          "0x13b360d4e82fe915fed16081038f98c211427b87a281bd733c277dbadf10372b",
          "0x0f34fda7cd268bf095354ea6c2631826c349d7518bf094361bc91f61c519c7fb",
          "0x2f549305063b1803a77ba92486c5383e00468fc857ec66d944868c4a309e596a",
          "0x1e626bf9ef3c8920522383ff9be21287c3af8d47fe61ff9af6c6d8d0118154bc",
          "0x02898db49f7022cac0aeabf721a3c7ab96e14ad35e7d6b0410b868ff504b62fc",
          "0x2a150ee7450ad90cace368ea424c8fdf36b3821e79f5f1c617a5f8e74c19b4cf",
          "0x17b46f4ef7740d27511083d60adcc58851d816b9bd6beb427258e1f844cec1af",
          "0x015648545d48eefd9c70b7beb4e133d9fed55e50ef7343adbb888f75e9afe7ec",
          "0x2d22caa08d7aedd8dd6fa15f08112f0af3ff1591bd77aff5d4edebd658f1bdf9",
          "0x212f50cb140b1439231af70fbf1e403664ea10f6edc8dc5b2818d6322ae63806",
          "0x010fbb6ddaf6882610d49c91fabc201f27ed588021cd09b7ff5b6949bf61a697",
          "0x1201e278f1f51709662cc1b6e59f45d564845b007b5770f64d1b1cc3de7eab45",
          "0x2ddac0be41c17d5ef7a199bf5fdd90b191529d751b3c058d33298c949fb49d05",
          "0x064f3f8b9c26c71d0b6cdccc3f34c87df1806629ffc37ecb2c3bfcaca3e64b32",
      }};
      // clang-format on
      expected_permutations_columns = CreateColumns(evals);
    }
    EXPECT_EQ(pkey.permutation_proving_key().permutations(),
              expected_permutations_columns);

    std::vector<Poly> expected_permutations_polys;
    {
      // clang-format off
      std::vector<std::vector<std::string_view>> polys = {{
          "0x112272f91e39f0f3754b36d52fafaa32b73194639e9077aeeb8fd9eeb5c197f6",
          "0x0f6ca21bfbcb52a4aee917564277498d8adcd0ead26ba0df100a7286b752d9d4",
          "0x15a7e8359f35c766c29ed77f4d1d5119901d036e20c15142192a21e09041e827",
          "0x0b4bc964d379eeaa158eca75b6dbb6a7b86f896b10f25adc9b92d8d5475ed2f1",
          "0x25d115149ca21b94ee56a92ef0d8c8602163a397afc69182e9b4411789e2e60b",
          "0x1e4a361c58a1fc2e342bb1b5522c187cfc6134683e6fa0f85eacc9916574db2c",
          "0x18b8417cd7b83ee5b62a8acacf2af01e8ea1a740a58c55047d8c926b8368c14d",
          "0x102953096b3b2549bc2c5f8c310a262a6280534c7e1984c250448ba8b1ff20d1",
          "0x12deb8e349e34c7dbd7857f75ca3dc59a3258bfe82bc3d01f2c8be9df84e4b96",
          "0x14232df27fdebe635d88be19813451e8b574c2458d8ae3c1503dc0833e136502",
          "0x25333b49ba4fe6f17bfa2659b5254c7ab6805b8bb4711ee45163adcdeeafdded",
          "0x13519d493b4d53fdd4aa5e6bfbf50e5e459112c26c54b865e524adc12e3c211e",
          "0x28b5a8a4f3f4184b8747b9d14a086e6e83e878fb40ffc9529d9603cf20a5ce1b",
          "0x105c187bb3938b9863b8a65abb1e84eeff1390bb1f5b99a6f34e1b0f8286db1b",
          "0x1d47bb6d01831abc30218432bcd7f6ebdf7fcdfd9e1c3c42302bb8d9526ed60c",
          "0x019d94bb672f383811eabd489da66d2cc7b2f8ce7e58d88c72e5e354c865eb21",
      },
      {
          "0x141e39f89c516db1255ecee3825495dfb18b1bd87123d9257363ae894a291dd3",
          "0x08a4dfd16be05cd2087a54d987991410bba11d1548080afde146412d7de47930",
          "0x2f25daf535a35461b7c8d7f63bdbbefbd8339e1d3c294d9069a788bb901c525c",
          "0x0ec047746f9977d98dc53b31063cd1fe923a04dfa0e7b689138388a53ce7d701",
          "0x256defc2c8f2e2dad221ba4401452d4a6aa8dced60798b0cdc3618e76a4ee2e0",
          "0x22053aee890a5e747c94c48401983f3c2e9cf8aaf7c8c03afaa6c573ef6fa7f8",
          "0x15add4829e360738c4aa9bf9a1ada88926b9bd475b09577c34ac62ebefcea7f6",
          "0x028b81cb54900a1ac0a193c1d76a3a83742de52b5ceb991e475e98982a564c33",
          "0x0361f1609cfec0d84058dbc7fac6dc7f3dde7c674b560517b375a6775bc71d65",
          "0x00603be4f6072cb8adb7b6a6a54cfac89e3c6287ade9d4cf64cbb1d659193490",
          "0x1a13eebb730009c87bc2fb35f61a4f938809953b467d85a8d6c4db8fa2d8547d",
          "0x21a406fe719828502a8b0a857b44865e95f9e368d8d1ba08305e6ceeb3182900",
          "0x1fbfc06d752ce7940fe2ad597c54a9ff0974b915ce28b93da3d3a08a5c60c1e8",
          "0x0e5f1384582741b53bbb81327fe91920f996ef9d81f0b056493b302000905809",
          "0x05de7057059dbb0bf2947a408fca011bb4cf784db57515afb28f9cee22669553",
          "0x2dd8cca78ca1960ef7aeb1f4aa171dd9b406031d1ccdd772fc835cfbc5a9b3ce",
      },
      {
          "0x15a78e0e6085f1debac2f4ec73d13ceba739fee72ad97c78b52a98f544aeb8ff",
          "0x1812dc7c7a640429efb6c9f4517a6a26665d974c956fc4b205154578966cb7d5",
          "0x2d25b29b330ef94426a3147d0cfbeac4967b6ad86802c928957e0e84a21a5d87",
          "0x261f4d63c9fdebb6334bf99c16a2b16a6889dbe3db6d53c72bba22395b7b672f",
          "0x1e79eccbbbe54d40275ee2bfbec7ae64b7aebf6e5142e793b30a7ec56dde6aff",
          "0x2f0b5305526a51f04ed9e81f590d5dadf43a2488ebf665c4f85c9ae9edf419c9",
          "0x1a268af4032dced16da12867c46aa1da4b8cbd95767a0829326fe829756fdeda",
          "0x160198e6359a11c21826c7d41371a65a056720e7f3a4ef9036a118e182490fd4",
          "0x27f8f5e308e267a585e5f57c4f1d6a7221517bb415c7de49238e98966bc593b1",
          "0x0b78ca79f16a7d1dae622114e268f3c124081d6a39858811f478685428e2adfc",
          "0x163129eea9b2d7f90a9491c5b1b14501d8446f1f6c89d26f192946472f70d883",
          "0x1a716c12432e1e01a617216c047e29823135fc00fb4cfb292e18d574606f35c0",
          "0x2177d56ea2af7bf7e9b131d78074426092359e6ca2345b36d32c1d2f03566ac3",
          "0x11856670bac1b7c78a8932e8c2137d3ea585b35beac3e92b61765cc3cdf68326",
          "0x26df134ce46792b7f32f53ac7df54484a1da3922ab3cab65ceba02399b5ace1e",
          "0x2a8f208fd791f7f5c13c533407af34929458b6fce3155f602331decc39a18d1b",
      },
      {
          "0x25e061e5a751efd01b3390c7dd2d33bc4071216db8e513d573a5c9ba9b66913a",
          "0x2c70d8ce1e96bc83662ff9edf413c8b1ddb7308b30770b8b0363bb16b410966b",
          "0x0fdf631b3cbc1368612327cce61e043125889ca33418bf1b2795fa110bdf77bc",
          "0x233d0ce0f5d908c63505bfca1434d15fc248b560ea827e14ee07d9498ddc8588",
          "0x20741541c6dd548e731c08a36e05f26bcd37a4bf5bd8507b67748ff15b8eaa1e",
          "0x06ceaea47f22a3a3f3cb1fd1a920bad815977860c229146ce63786165b6bac01",
          "0x2516e1b23de252185b9b2c8753c52dad78073c4848830e0ad3b139d7e1043c7b",
          "0x24202f1572d431b51140eab5b4305fff78babdd7726b235a0e8360b97ff1780f",
          "0x275e5b9af3e37e1402656f0c71add7179326c94a3c508b97e3136325374d0086",
          "0x043a8f7d69c75e225994410c0bbbb1b0cdedc772dd29f8b02444d05ab3daaaa2",
          "0x2e7cc9c94b8356104b5c50ae98eec937238938d0b15fd25da84d80a7817a6bcb",
          "0x143def6354020635267fb1edc79f0f36da06d1904800c28b2be63348fd2f7606",
          "0x1c951d6176ed6399f36cb6b8ddece674c50fe1b2415ef397aaff428a29246a59",
          "0x0ab51b71f8d9ba71c7319ad8657f90942a9701c88b7d6f04a51216475f40f779",
          "0x26d4e18d12de6a7b7c071a2dc7fbe89ea4c65f6d647143fcc5984acae9088635",
          "0x2eacc4ca1c50e55c175b62c30ac9a31e50d52c60ed7e7f8640bb920f4c69329a",
      }};
      // clang-format on
      expected_permutations_polys = CreatePolys(polys);
    }
    EXPECT_EQ(pkey.permutation_proving_key().polys(),
              expected_permutations_polys);
  }
}

TEST_F(FibonacciV1CircuitTest, CreateProof) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  Fibonacci1Circuit<F, V1FloorPlanner> circuit;
  std::vector<Fibonacci1Circuit<F, V1FloorPlanner>> circuits = {
      std::move(circuit)};

  F a(1);
  F b(1);
  F out(55);
  std::vector<F> instance_column = {std::move(a), std::move(b), std::move(out)};
  std::vector<Evals> instance_columns = {Evals(std::move(instance_column))};
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

TEST_F(FibonacciV1CircuitTest, VerifyProof) {
  size_t n = 16;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  Fibonacci1Circuit<F, V1FloorPlanner> circuit;

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  std::vector<uint8_t> owned_proof(std::begin(kExpectedProof),
                                   std::end(kExpectedProof));
  std::unique_ptr<Verifier<PCS>> verifier =
      CreateVerifier(CreateBufferWithProof(absl::MakeSpan(owned_proof)));

  F a(1);
  F b(1);
  F out(55);
  std::vector<F> instance_column = {std::move(a), std::move(b), std::move(out)};
  std::vector<Evals> instance_columns = {Evals(std::move(instance_column))};
  std::vector<std::vector<Evals>> instance_columns_vec = {
      std::move(instance_columns)};

  Proof<F, Commitment> proof;
  F h_eval;
  ASSERT_TRUE(verifier->VerifyProofForTesting(vkey, instance_columns_vec,
                                              &proof, &h_eval));

  std::vector<std::vector<Commitment>> expected_advice_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x02256f3c5db09eaa3ed2f22d335f543f34f9b50f2714271b90bbbbfe8a365ac5",
         "0x1892bac12516e09b25b7acaa45dccc104fb325529df299fcb08b7fc250723b1a"},
        {"0x2e3534433e05f5b1242605ef6432257516e22cb8b5ae1fc2da06b32b9092585d",
         "0x30035ae0c7892f2d414997acee56077efc83c863be8eca1b40b563ef293b5f06"},
        {"0x197f0b5647a76ebf8cfbe549fa3b32da691bbab2217cd7556b336ac0f56cbe54",
         "0x125234fa5387f32f1959d867a9854afe20c9079047f05a483eca4d5535a4bb3c"},
    };
    expected_advice_commitments_vec.push_back(CreateCommitments(points));
  }
  EXPECT_EQ(proof.advices_commitments_vec, expected_advice_commitments_vec);

  EXPECT_TRUE(proof.challenges.empty());

  F expected_theta = F::FromHexString(
      "0x18a64f2ddc1afba4a10885729147924aee5566dedc5a60639d03faeda5987d55");
  EXPECT_EQ(proof.theta, expected_theta);

  ASSERT_EQ(proof.lookup_permuted_commitments_vec.size(), 1);
  EXPECT_TRUE(proof.lookup_permuted_commitments_vec[0].empty());

  F expected_beta = F::FromHexString(
      "0x0fad7877ce13703990abfb4da11774def34800d2ad6a67de343437f0879cca6e");
  EXPECT_EQ(proof.beta, expected_beta);

  F expected_gamma = F::FromHexString(
      "0x0603f87b63670e962930822ca8da4fd51cf203f717e2c6ff07cf1b6198b67e7d");
  EXPECT_EQ(proof.gamma, expected_gamma);

  std::vector<std::vector<Commitment>>
      expected_permutation_product_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x20a579e37cd534b9ba661195f4692a7ae894d4551bcfb6dae694833f6cb6022d",
         "0x1d06b37e36e0686cf277125f2149e1deb0f9373def884c6ffe3a393efb135409"},
        {"0x083bb7615143116310f5273a38315c41f150de01d23db57e3eb2cdf2a9f75bf9",
         "0x28f5f2a447884fb6541e9505858ca6e6564ce6921b0a8247411cd3ba059afcc3"},
        {"0x11a91d6abcaf696e66dcdd217d9492cf96e9c20fdcf9f3c2202b03c07519e076",
         "0x228fc8589c8e13db408e5730b0b71189c4db2715e6ae98c107e78d44f9f9e039"},
        {"0x27ac6a30bd598f7e40cef79d6a4ffe3f86263b60428d6e4d013f9db385688003",
         "0x0659560ad79308cb12b2d1d4b49c82972fae49d91a801352722723b7356d4722"},
    };
    expected_permutation_product_commitments_vec.push_back(
        CreateCommitments(points));
  }
  EXPECT_EQ(proof.permutation_product_commitments_vec,
            expected_permutation_product_commitments_vec);

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
      "0x2bae05c734517c500ad5a075952e4e129ddb521711381be2f0e80f7af06b6fec");
  EXPECT_EQ(proof.y, expected_y);

  std::vector<Commitment> expected_vanishing_h_poly_commitments;
  {
    std::vector<Point> points = {
        {"0x02ebecc45946430c7bdfb35cfdb9d76bbbe76a0ab645496830376ec7197ee969",
         "0x27790a5dbfb7915016b8eda65148bc70ca178b2a425195abf1c2352dfc4805e4"},
        {"0x277ab84878b5f6e577110a74d0b7216e52da0bf295d4e0b3618ceb177f8af774",
         "0x08d0f9762fc80f3f6f36581d96a89df1ca94460ac60f433e2b44befcc98897a4"},
    };
    expected_vanishing_h_poly_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(proof.vanishing_h_poly_commitments,
            expected_vanishing_h_poly_commitments);

  F expected_x = F::FromHexString(
      "0x2cce10dbad327c0d7a709658d40e2d2c78ed07211bd2ef6a74685ebe4201486e");
  EXPECT_EQ(proof.x, expected_x);

  std::vector<std::vector<F>> expected_advice_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x27b53418f2324edbe467bdd6bb498d5c9d99f4012d3e3829b88c249a360c362b",
        "0x2eccdc543850fe1bed269dd497a3dff65b28c2a59ba17e17cf45b3a963b547b7",
        "0x2e2e11b4dc1bb4f5296e4bcd6879806eded2635fb285b760bd68e8a1d32753e0",
    };
    expected_advice_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.advice_evals_vec, expected_advice_evals_vec);

  std::vector<F> expected_fixed_evals;
  {
    std::vector<std::string_view> evals = {
        "0x0ec57225ea5c319ea635a02656ab7b874b46b9e9f0b525d34f11b855523ab480",
    };
    expected_fixed_evals = CreateEvals(evals);
  }
  EXPECT_EQ(proof.fixed_evals, expected_fixed_evals);

  F expected_vanishing_random_eval = F::FromHexString(
      "0x0000000000000000000000000000000000000000000000000000000000000001");
  EXPECT_EQ(proof.vanishing_random_eval, expected_vanishing_random_eval);

  std::vector<F> expected_common_permutation_evals;
  {
    std::vector<std::string_view> evals = {
        "0x05c807281875c619cc6367954b7c6ae0ec90810bd81fc7ef454c3e5deb059ae6",
        "0x0a0aaead835909c2d0cf8c2520af3708856b139751b456e39d55fb39a0b5d896",
        "0x1131487bcee76138daa5dd6f972217cd75247b55f9a7cf8221ed356a80c5e4e4",
        "0x1428ca5f40e50e972da1ba4bad9a8179d271106b066bc6df86eab76da0f48840",
    };
    expected_common_permutation_evals = CreateEvals(evals);
  }
  EXPECT_EQ(proof.common_permutation_evals, expected_common_permutation_evals);

  std::vector<std::vector<F>> expected_permutation_product_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x1f2783f91a0b1d2ce188184f0ab98402f58522391e954399ef9364088327e538",
        "0x0a419c19baad150afa702f16dccd364c8f179e704cd54fd8a7afebc88dd813d7",
        "0x02063e6a821152f464ab9cfd7943d16b992a98bc07004976d7a8695ea787b803",
        "0x136426b7523de5879d8ba5c2e28b1605001516395ce2d071e352421b26614c52",
    };
    expected_permutation_product_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.permutation_product_evals_vec,
            expected_permutation_product_evals_vec);

  std::vector<std::vector<F>> expected_permutation_product_next_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x2dcb0238092ce418368710b60fe54c2cab31846b6d3b8f713d00033962838fdf",
        "0x129aa77f9fe77e9b10a385e86997e8b8954788df07b931002e5a62669d41a646",
        "0x0f5c9aa5b1aea4532835c0b29b9d3f1ab2c5170c277417416a3fb73cfa96ef84",
        "0x04e5b6491f7bbcf342c2b544a4b0f55f40461bc4c25e6723c27dc1152e818a65",
    };
    expected_permutation_product_next_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.permutation_product_next_evals_vec,
            expected_permutation_product_next_evals_vec);

  std::vector<std::vector<std::optional<F>>>
      expected_permutation_product_last_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x18aaf94b2fdf690c7708ba4b8bd9f144c36e748f43d4c262eb04bc81c7ab395b",
        "0x06e39ba52c5d29713a1fc2789bfc8e546597809afca85a8beb78fb2440e4b66b",
        "0x29855ee292be935c8e9c754f782295ca55a7c60e41e9870cba384095fa1e838a",
        "",
    };
    expected_permutation_product_last_evals_vec.push_back(
        CreateOptionalEvals(evals));
  }
  EXPECT_EQ(proof.permutation_product_last_evals_vec,
            expected_permutation_product_last_evals_vec);

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
