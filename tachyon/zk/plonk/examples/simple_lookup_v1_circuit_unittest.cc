#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tachyon/zk/plonk/examples/circuit_test.h"
#include "tachyon/zk/plonk/examples/simple_lookup_circuit.h"
#include "tachyon/zk/plonk/halo2/pinned_verifying_key.h"
#include "tachyon/zk/plonk/keys/proving_key.h"
#include "tachyon/zk/plonk/layout/floor_planner/v1/v1_floor_planner.h"

namespace tachyon::zk::halo2 {

namespace {

constexpr size_t kBits = 3;

constexpr uint8_t kExpectedProof[] = {
    47,  182, 87,  188, 243, 30,  63,  165, 203, 25,  53,  75,  159, 66,  202,
    142, 241, 131, 243, 14,  210, 25,  158, 80,  248, 54,  227, 231, 128, 14,
    188, 33,  135, 89,  21,  76,  201, 3,   235, 80,  0,   226, 252, 125, 123,
    57,  32,  116, 77,  132, 74,  28,  129, 87,  84,  88,  98,  197, 114, 110,
    128, 182, 153, 19,  181, 198, 105, 109, 162, 166, 183, 84,  67,  244, 177,
    201, 49,  196, 14,  0,   219, 248, 92,  5,   235, 57,  222, 34,  192, 113,
    234, 166, 141, 6,   20,  44,  163, 197, 58,  201, 108, 119, 98,  206, 182,
    224, 163, 217, 204, 19,  116, 126, 186, 254, 5,   145, 243, 143, 77,  148,
    12,  156, 51,  163, 253, 50,  169, 174, 1,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   252, 201, 198, 216, 151,
    252, 234, 109, 125, 227, 44,  215, 90,  19,  227, 211, 16,  132, 218, 47,
    20,  14,  103, 227, 190, 48,  56,  18,  141, 245, 147, 155, 111, 69,  183,
    230, 171, 201, 152, 96,  96,  50,  231, 189, 239, 146, 228, 77,  195, 44,
    115, 52,  100, 50,  26,  190, 251, 125, 237, 62,  51,  129, 206, 41,  213,
    73,  24,  105, 24,  52,  90,  88,  74,  250, 189, 72,  232, 182, 82,  66,
    236, 149, 14,  82,  230, 247, 77,  156, 203, 208, 59,  229, 232, 91,  174,
    16,  185, 190, 49,  196, 79,  222, 232, 164, 156, 122, 177, 104, 52,  223,
    34,  13,  85,  36,  146, 51,  96,  212, 188, 159, 26,  113, 139, 225, 166,
    134, 23,  32,  124, 225, 54,  77,  154, 74,  58,  114, 7,   152, 224, 110,
    164, 15,  197, 176, 210, 102, 181, 255, 137, 174, 192, 151, 54,  27,  201,
    157, 161, 51,  146, 0,   206, 113, 69,  204, 199, 117, 182, 68,  68,  29,
    252, 62,  114, 155, 151, 16,  34,  113, 23,  153, 119, 62,  100, 62,  166,
    56,  197, 14,  18,  113, 57,  38,  12,  174, 198, 230, 163, 123, 21,  249,
    53,  24,  174, 72,  9,   196, 231, 43,  117, 53,  121, 116, 113, 120, 244,
    212, 71,  250, 245, 200, 183, 177, 200, 46,  1,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   74,  233, 48,  131,
    22,  89,  250, 105, 203, 216, 104, 232, 153, 60,  228, 53,  103, 220, 115,
    5,   204, 55,  219, 211, 190, 108, 68,  54,  163, 239, 67,  27,  110, 95,
    81,  83,  107, 43,  251, 249, 233, 48,  243, 174, 235, 248, 127, 71,  163,
    105, 128, 254, 55,  87,  239, 219, 221, 85,  82,  48,  34,  172, 26,  19,
    44,  27,  229, 55,  53,  174, 87,  94,  5,   41,  104, 180, 183, 3,   108,
    207, 242, 98,  41,  2,   174, 175, 254, 144, 50,  246, 136, 187, 62,  224,
    212, 25,  16,  255, 92,  32,  104, 219, 15,  248, 16,  49,  250, 136, 75,
    83,  112, 23,  116, 159, 191, 103, 244, 34,  61,  35,  192, 60,  93,  212,
    133, 187, 243, 28,  111, 98,  50,  77,  146, 192, 142, 77,  44,  183, 174,
    206, 135, 161, 1,   134, 241, 9,   121, 230, 187, 75,  196, 164, 56,  0,
    174, 101, 190, 81,  190, 12,  74,  166, 34,  101, 37,  83,  187, 10,  167,
    168, 129, 126, 12,  132, 0,   130, 183, 115, 192, 14,  219, 226, 9,   51,
    208, 151, 250, 198, 0,   173, 91,  30,  189, 194, 22,  141, 13,  114, 113,
    162, 210, 171, 230, 111, 50,  248, 118, 206, 52,  121, 43,  116, 34,  115,
    77,  174, 137, 208, 21,  143, 232, 236, 188, 160};

constexpr uint8_t kExpectedMultiProof[] = {
    47,  182, 87,  188, 243, 30,  63,  165, 203, 25,  53,  75,  159, 66,  202,
    142, 241, 131, 243, 14,  210, 25,  158, 80,  248, 54,  227, 231, 128, 14,
    188, 33,  47,  182, 87,  188, 243, 30,  63,  165, 203, 25,  53,  75,  159,
    66,  202, 142, 241, 131, 243, 14,  210, 25,  158, 80,  248, 54,  227, 231,
    128, 14,  188, 33,  161, 99,  130, 124, 10,  20,  160, 177, 184, 226, 59,
    230, 41,  56,  59,  50,  77,  199, 118, 186, 205, 135, 239, 134, 240, 216,
    178, 143, 224, 186, 112, 133, 110, 18,  165, 210, 171, 209, 142, 38,  49,
    39,  167, 176, 66,  2,   157, 206, 154, 213, 99,  116, 249, 18,  192, 71,
    196, 104, 232, 248, 155, 157, 34,  159, 89,  158, 214, 71,  187, 172, 132,
    250, 244, 246, 27,  26,  161, 145, 46,  212, 131, 213, 197, 195, 198, 107,
    243, 182, 186, 151, 97,  0,   50,  34,  16,  153, 109, 145, 193, 251, 94,
    145, 202, 240, 196, 236, 95,  31,  161, 173, 90,  245, 183, 206, 42,  94,
    187, 113, 14,  9,   16,  180, 18,  53,  240, 184, 230, 160, 141, 34,  61,
    50,  56,  77,  136, 29,  85,  118, 10,  11,  246, 144, 45,  247, 193, 24,
    159, 171, 151, 79,  99,  231, 4,   196, 85,  14,  155, 140, 73,  154, 13,
    51,  1,   183, 129, 90,  188, 60,  1,   201, 92,  29,  18,  150, 103, 47,
    167, 207, 151, 155, 254, 184, 236, 190, 146, 119, 107, 159, 165, 160, 64,
    165, 1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   47,  172, 222, 13,  13,  12,  224, 97,  93,  200, 160, 75,
    5,   163, 114, 90,  192, 141, 254, 13,  100, 184, 101, 163, 233, 159, 78,
    5,   241, 39,  149, 5,   40,  11,  98,  1,   131, 105, 31,  119, 211, 35,
    208, 92,  16,  141, 54,  186, 85,  51,  48,  147, 26,  212, 124, 16,  28,
    2,   89,  147, 218, 21,  81,  0,   108, 96,  172, 102, 157, 115, 78,  132,
    158, 105, 84,  190, 71,  232, 56,  178, 31,  177, 102, 86,  121, 78,  135,
    139, 195, 51,  89,  225, 105, 42,  121, 156, 82,  179, 44,  221, 114, 243,
    65,  222, 7,   178, 43,  232, 233, 163, 39,  248, 255, 223, 71,  229, 151,
    12,  34,  80,  73,  103, 57,  110, 1,   139, 223, 171, 204, 73,  3,   168,
    27,  182, 207, 72,  36,  191, 169, 49,  218, 64,  166, 147, 146, 6,   242,
    47,  237, 168, 252, 122, 135, 62,  131, 186, 247, 123, 158, 4,   204, 73,
    3,   168, 27,  182, 207, 72,  36,  191, 169, 49,  218, 64,  166, 147, 146,
    6,   242, 47,  237, 168, 252, 122, 135, 62,  131, 186, 247, 123, 158, 4,
    139, 30,  224, 135, 52,  150, 157, 241, 211, 151, 197, 1,   41,  166, 172,
    233, 28,  128, 103, 213, 108, 62,  199, 22,  27,  44,  107, 62,  220, 198,
    172, 24,  202, 35,  32,  107, 51,  158, 42,  192, 112, 163, 235, 241, 13,
    21,  31,  138, 195, 38,  74,  235, 102, 118, 197, 233, 145, 137, 92,  237,
    50,  169, 218, 31,  1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   147, 9,   27,  191, 53,  255, 133, 161, 207,
    13,  48,  194, 151, 1,   51,  177, 200, 1,   10,  177, 231, 20,  173, 104,
    17,  223, 68,  229, 202, 28,  92,  7,   202, 190, 62,  194, 225, 115, 241,
    77,  54,  107, 222, 211, 213, 40,  221, 124, 236, 132, 45,  28,  50,  124,
    187, 14,  225, 140, 170, 71,  229, 214, 157, 21,  210, 74,  235, 46,  221,
    76,  167, 90,  61,  36,  164, 163, 205, 203, 121, 48,  38,  204, 205, 60,
    50,  87,  40,  8,   134, 231, 171, 226, 150, 94,  48,  7,   229, 121, 5,
    139, 205, 183, 36,  150, 127, 201, 6,   223, 16,  249, 167, 247, 41,  251,
    50,  61,  37,  71,  102, 218, 56,  197, 9,   162, 69,  159, 96,  10,  83,
    156, 93,  126, 48,  239, 157, 14,  151, 244, 182, 207, 214, 114, 206, 66,
    125, 143, 9,   39,  58,  125, 31,  207, 247, 184, 221, 159, 199, 221, 1,
    5,   1,   160, 63,  77,  223, 129, 195, 86,  34,  53,  60,  89,  38,  147,
    91,  250, 196, 77,  154, 203, 20,  87,  74,  15,  23,  14,  193, 179, 251,
    70,  128, 17,  27,  172, 155, 236, 201, 208, 65,  160, 226, 93,  16,  146,
    53,  179, 114, 131, 165, 122, 137, 68,  149, 154, 38,  109, 61,  21,  151,
    184, 239, 157, 122, 41,  149, 180, 63,  151, 95,  120, 63,  5,   163, 232,
    209, 202, 25,  166, 247, 195, 41,  238, 217, 8,   170, 185, 191, 187, 91,
    119, 200, 233, 235, 168, 156, 9,   172, 76,  239, 215, 3,   217, 222, 86,
    109, 249, 0,   195, 134, 197, 87,  64,  151, 214, 250, 23,  241, 132, 6,
    4,   12,  33,  156, 12,  166, 249, 207, 26,  161, 82,  172, 32,  172, 188,
    238, 122, 176, 68,  121, 246, 202, 25,  9,   65,  144, 93,  19,  17,  67,
    179, 86,  103, 217, 182, 214, 217, 248, 12,  119, 39,  188, 160, 17,  210,
    52,  34,  134, 158, 145, 158, 123, 236, 212, 139, 45,  173, 239, 186, 235,
    196, 183, 190, 66,  121, 8,   119, 95,  12,  151, 160, 172, 18,  145, 117,
    33,  138, 63,  122, 32,  15,  217, 123, 11,  246, 172, 91,  51,  159, 113,
    109, 205, 185, 29,  253, 28,  11,  35,  238, 106, 172, 44,  253, 192, 158};

class SimpleLookupV1CircuitTest : public CircuitTest {};

}  // namespace

TEST_F(SimpleLookupV1CircuitTest, Configure) {
  ConstraintSystem<F> constraint_system;
  SimpleLookupConfig<F, kBits> config =
      SimpleLookupCircuit<F, kBits, V1FloorPlanner>::Configure(
          constraint_system);
  EXPECT_EQ(config.selector(), Selector::Complex(0));
  EXPECT_EQ(config.table(), LookupTableColumn(FixedColumnKey(0)));
  EXPECT_EQ(config.advice(), AdviceColumnKey(0));
  EXPECT_EQ(constraint_system.num_fixed_columns(), 1);
  EXPECT_EQ(constraint_system.num_advice_columns(), 1);
  EXPECT_EQ(constraint_system.num_instance_columns(), 0);
  EXPECT_EQ(constraint_system.num_selectors(), 1);
  EXPECT_EQ(constraint_system.num_challenges(), 0);
  std::vector<Phase> expected_advice_column_phases = {kFirstPhase};
  EXPECT_EQ(constraint_system.advice_column_phases(),
            expected_advice_column_phases);
  EXPECT_TRUE(constraint_system.challenge_phases().empty());
  EXPECT_TRUE(constraint_system.selector_map().empty());
  EXPECT_TRUE(constraint_system.gates().empty());
  std::vector<AdviceQueryData> expected_advice_queries = {
      AdviceQueryData(Rotation::Cur(), AdviceColumnKey(0)),
  };
  EXPECT_EQ(constraint_system.advice_queries(), expected_advice_queries);
  std::vector<RowIndex> expected_num_advice_queries = {1};
  EXPECT_EQ(constraint_system.num_advice_queries(),
            expected_num_advice_queries);
  EXPECT_TRUE(constraint_system.instance_queries().empty());
  std::vector<FixedQueryData> expected_fixed_queries = {
      FixedQueryData(Rotation::Cur(), FixedColumnKey(0)),
  };
  EXPECT_EQ(constraint_system.fixed_queries(), expected_fixed_queries);
  EXPECT_TRUE(constraint_system.permutation().columns().empty());
  std::vector<LookupArgument<F>> expected_lookups;
  {
    LookupPairs<std::unique_ptr<Expression<F>>> pairs;
    pairs.emplace_back(
        ExpressionFactory<F>::Sum(
            ExpressionFactory<F>::Product(
                ExpressionFactory<F>::Selector(config.selector()),
                ExpressionFactory<F>::Advice(
                    AdviceQuery(0, Rotation::Cur(), config.advice()))),
            ExpressionFactory<F>::Sum(
                ExpressionFactory<F>::Constant(F::One()),
                ExpressionFactory<F>::Negated(
                    ExpressionFactory<F>::Selector(config.selector())))),
        ExpressionFactory<F>::Fixed(
            FixedQuery(0, Rotation::Cur(), config.table().column())));
    expected_lookups.emplace_back("lookup", std::move(pairs));
  }
  EXPECT_EQ(constraint_system.lookups(), expected_lookups);
  EXPECT_TRUE(constraint_system.general_column_annotations().empty());
  EXPECT_FALSE(constraint_system.minimum_degree().has_value());
}

TEST_F(SimpleLookupV1CircuitTest, Synthesize) {
  size_t n = 32;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));
  const Domain* domain = prover_->domain();

  ConstraintSystem<F> constraint_system;
  SimpleLookupConfig<F, kBits> config =
      SimpleLookupCircuit<F, kBits, V1FloorPlanner>::Configure(
          constraint_system);
  Assembly<PCS> assembly =
      VerifyingKey<PCS>::CreateAssembly(domain, constraint_system);

  SimpleLookupCircuit<F, kBits, V1FloorPlanner> circuit(4);
  typename SimpleLookupCircuit<F, kBits, V1FloorPlanner>::FloorPlanner
      floor_planner;
  floor_planner.Synthesize(&assembly, circuit, std::move(config),
                           constraint_system.constants());

  std::vector<RationalEvals> expected_fixed_columns;
  {
    std::vector<std::vector<std::string_view>> evals = {{
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000002",
        "0x0000000000000000000000000000000000000000000000000000000000000003",
        "0x0000000000000000000000000000000000000000000000000000000000000004",
        "0x0000000000000000000000000000000000000000000000000000000000000005",
        "0x0000000000000000000000000000000000000000000000000000000000000006",
        "0x0000000000000000000000000000000000000000000000000000000000000007",
        "0x0000000000000000000000000000000000000000000000000000000000000008",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
        "0x0000000000000000000000000000000000000000000000000000000000000001",
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
    }};
    expected_fixed_columns = CreateRationalColumns(evals);
  }
  EXPECT_EQ(assembly.fixed_columns(), expected_fixed_columns);

  EXPECT_TRUE(assembly.permutation().columns().empty());

  const CycleStore& cycle_store = assembly.permutation().cycle_store();
  EXPECT_TRUE(cycle_store.mapping().IsEmpty());
  EXPECT_TRUE(cycle_store.aux().IsEmpty());
  EXPECT_TRUE(cycle_store.sizes().IsEmpty());

  std::vector<std::vector<bool>> expected_selectors = {{
      true,  true,  true,  true,  true,  true,  true,  true,
      true,  true,  true,  true,  true,  true,  true,  true,
      false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false,
  }};
  EXPECT_EQ(assembly.selectors(), expected_selectors);
  EXPECT_EQ(assembly.usable_rows(), base::Range<RowIndex>::Until(26));
}

TEST_F(SimpleLookupV1CircuitTest, LoadVerifyingKey) {
  size_t n = 32;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  SimpleLookupCircuit<F, kBits, V1FloorPlanner> circuit(4);

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  EXPECT_TRUE(vkey.permutation_verifying_key().commitments().empty());

  std::vector<Commitment> expected_fixed_commitments;
  {
    std::vector<Point> points = {
        {"0x2491b553ef715ba083a05cf556402967f4eb0d63faac61e1011382f579e92a25",
         "0x21797eb7fe8cdbb4abbaf6b4a38d22f61555af4ffebd38547f1b0a978c8a1ddf"},
        {"0x06301defe1b3e0ee7910835b9b40d15107294b715b16753d4b9b34aafa4e1418",
         "0x2d03286f3624c40418f311b994f7c2c7257e0d92d487704983720fce4a99fbe7"},
    };
    expected_fixed_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(vkey.fixed_commitments(), expected_fixed_commitments);

  F expected_transcript_repr = F::FromHexString(
      "0x01522b814675ed8fa132994e824c46042f5130b140ac340c760eb426c7253ae8");
  EXPECT_EQ(vkey.transcript_repr(), expected_transcript_repr);
}

TEST_F(SimpleLookupV1CircuitTest, LoadProvingKey) {
  size_t n = 32;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  SimpleLookupCircuit<F, kBits, V1FloorPlanner> circuit(4);

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
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
      };
      expected_l_first = CreatePoly(poly);
    }
    EXPECT_EQ(pkey.l_first(), expected_l_first);

    Poly expected_l_last;
    {
      std::vector<std::string_view> poly = {
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2c5ebc726b47cc659ddda5c2c5199730cb3deb6c9bed0a12973aae60ebd81c83",
          "0x283c497861461cf04ba9e47a8bbd619f28e84387bca93dffdcee18677a42352c",
          "0x0f9a39926cb2b113e678257c8fdd8bdba10667f9e11d46d529cd041ca367cdd0",
          "0x1fc1d37b63c8911b6692d47caf0a2ab7e8bc5e8a1bc3e2047d92c8b230847e4f",
          "0x031d6554a817c4b89ce4e88ac199b59b8e27e8ee0d1118a50a00f2e870b9a147",
          "0x01599bef0e460a791764dcf17cb57f79b293139b37c0b85054a46d6a560deaf4",
          "0x14bd475039a03f8d889d751c4b756bf013700add984fb797ec2eee23930f61df",
          "0x0183227397098d014dc2822db40c0ac2e9419f4243cdcb848a1f0fac9f800000",
          "0x0405920075e9d3c41a729ff3bc67c12c5cf5fcdbddcc667eaca747330427e37e",
          "0x082804fa7feb83396ca6613bf5c3f6bdff4ba4c0bd10329166f3dd2c75bdcad5",
          "0x20ca14e0747eef15d1d82039f1a3cc81872d804e989c29bc1a14f1774c983231",
          "0x10a27af77d690f0e51bd7139d2772da53f7789be5df58e8cc64f2ce1bf7b81b2",
          "0x2d46e91e3919db711b6b5d2bbfe7a2c19a0bff5a6ca857ec39e102ab7f465eba",
          "0x2f0ab283d2eb95b0a0eb68c504cbd8e375a0d4ad41f8b840ef3d882999f2150d",
          "0x1ba70722a791609c2fb2d09a360bec6d14c3dd6ae169b8f957b307705cf09e22",
          "0x2ee12bff4a2813286a8dc388cd754d9a3ef2490635eba50cb9c2e5e750800001",
          "0x2c5ebc726b47cc659ddda5c2c5199730cb3deb6c9bed0a12973aae60ebd81c83",
          "0x283c497861461cf04ba9e47a8bbd619f28e84387bca93dffdcee18677a42352c",
          "0x0f9a39926cb2b113e678257c8fdd8bdba10667f9e11d46d529cd041ca367cdd0",
          "0x1fc1d37b63c8911b6692d47caf0a2ab7e8bc5e8a1bc3e2047d92c8b230847e4f",
          "0x031d6554a817c4b89ce4e88ac199b59b8e27e8ee0d1118a50a00f2e870b9a147",
          "0x01599bef0e460a791764dcf17cb57f79b293139b37c0b85054a46d6a560deaf4",
          "0x14bd475039a03f8d889d751c4b756bf013700add984fb797ec2eee23930f61df",
          "0x0183227397098d014dc2822db40c0ac2e9419f4243cdcb848a1f0fac9f800000",
          "0x0405920075e9d3c41a729ff3bc67c12c5cf5fcdbddcc667eaca747330427e37e",
          "0x082804fa7feb83396ca6613bf5c3f6bdff4ba4c0bd10329166f3dd2c75bdcad5",
          "0x20ca14e0747eef15d1d82039f1a3cc81872d804e989c29bc1a14f1774c983231",
          "0x10a27af77d690f0e51bd7139d2772da53f7789be5df58e8cc64f2ce1bf7b81b2",
          "0x2d46e91e3919db711b6b5d2bbfe7a2c19a0bff5a6ca857ec39e102ab7f465eba",
          "0x2f0ab283d2eb95b0a0eb68c504cbd8e375a0d4ad41f8b840ef3d882999f2150d",
          "0x1ba70722a791609c2fb2d09a360bec6d14c3dd6ae169b8f957b307705cf09e22",
      };
      expected_l_last = CreatePoly(poly);
    }
    EXPECT_EQ(pkey.l_last(), expected_l_last);

    Poly expected_l_active_row;
    {
      std::vector<std::string_view> poly = {
          "0x0912ceb58a394e07d28f0d12384840917789bb8d96d2c51b3cba5e0bbd000001",
          "0x1d82943c3cbe2c2860423646e2978e190877fb3428206267de5d42802c5469a4",
          "0x2572c0c97e6a4f263421a91b28e88314200149a7c7ad52017a2795a064b4ad9f",
          "0x0539c553022d8a6706aa7d43b916f826a446cc385baacfb3d7b66a3da2a4a71c",
          "0x06a4e286e8e1f6381ee3df0e41b7ebfb160a057e7942670cdcd4cd7fd63dcad5",
          "0x101a5290a87c4a11d6aecbae4d1ea244bc7ba09f7480d2706e8603b77eef7690",
          "0x08bed71c502de5c63e148770b676249eb9f7c6d7d2e30fd25177ecd1cf31d5fa",
          "0x2b2849fbd4207ee991f8adc3272599de4bb1d799fe540523d10bd7be68f2c494",
          "0x1e3eb107ccbf041a18d0524efafe1ff4ff7abf47d7f6167ff373b90591047e4f",
          "0x265aafd2d283df1150400782a1674fd58cc747651b688d032c0c2600007ccbde",
          "0x21e2d2ab84ea032abda712c51a905e69819959e789afc1428f28695b59a3cd79",
          "0x298173f6c450ff5d1aa710238c4b9be7989078b00b571433f92c41427a5c1b4a",
          "0x2d8790103be208af5328e69750bfce208c5f356afe2aecbc651e787cfa72150d",
          "0x2ed37d6ed1d5abd50a2c5c3b6a13736bbbc5cb637165b93eb9e192e192da55bd",
          "0x2c9e1063d1568f66e38646080d761995f2a0bbcda4ed4c68c25afeab719dc4b8",
          "0x0ad668e43820f188510890c9e5906e54af9aa65550f601e79a28e49dd4c59972",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0709cb0b6beec89572c99ab6bc880d038e5aa740e631879f2542794a76af2310",
          "0x27adb8442c71c491438708732748c0e44ae895b77a796689a59cdb483bb40ec1",
          "0x245143f0f1496195f2e77fd90d06b853fa638f55918d45533fc7cbdbfa5719eb",
          "0x26b92704ca3c8feefde7624cd7b156dc3fa6a44578db727b52cf08badac2352c",
          "0x23ac28e9730302a77db6be539530ca51b5e26f90a3b1ded8d627002d059a590e",
          "0x2622f0e98def29d70fe96397e09525345e686eeff3a701c95fc54c75ef24ed2c",
          "0x021084fdcb5ac54d2dea020edc9dbcb6fb8e6b96648138f62474629a46e62494",
          "0x0f1f5883e65f820d03faef0c1e6b22e25635ea7c1a27c3083c301d351ffb81b2",
          "0x107b1e0f0e1a429fefc13f26cfa5c7e162819849eab588d7058653b3e447fba0",
          "0x100402345f5c2d8b64fb8d9f5167087db66e40e1a3390e44475e4884c8056f63",
          "0x2e5c33950a4961df0f01031c87a93f50165b8f55ef24822d12e8e87d9f9f4f05",
          "0x303ac7ee586e1da181f2a07a4a2acd13f1855ca16dac5d5d0e675351a68deaf5",
          "0x039513b90e2671a96fa218f5a975ce9eec90436a48dd57dbdbc70a0b20d38677",
          "0x176e61e727623d5ecdc1d98d26dcab891b111dac86184cbfe9867127bdf97eeb",
          "0x0819511deb18fdaead1bc5e1429f10d85c5e54084b66d6db5c4b577f846a5214",
      };
      expected_l_active_row = CreatePoly(poly);
    }
    EXPECT_EQ(pkey.l_active_row(), expected_l_active_row);

    std::vector<Evals> expected_fixed_columns;
    {
      // clang-format off
      std::vector<std::vector<std::string_view>> evals = {{
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000002",
          "0x0000000000000000000000000000000000000000000000000000000000000003",
          "0x0000000000000000000000000000000000000000000000000000000000000004",
          "0x0000000000000000000000000000000000000000000000000000000000000005",
          "0x0000000000000000000000000000000000000000000000000000000000000006",
          "0x0000000000000000000000000000000000000000000000000000000000000007",
          "0x0000000000000000000000000000000000000000000000000000000000000008",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
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
      },
      {
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
          "0x0000000000000000000000000000000000000000000000000000000000000001",
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
          "0x0f1f5883e65f820d099915c908786b9d1c903896a609f32d65369cbe3b000002",
          "0x096ac2c44881b041a21b6f1f1bb2d4d7f9c270a7f29e76fffbd15325b95e3ce2",
          "0x00f3e53a93723acf565de2acd9206f6e54e69eb9c2a69ca8b869bfa5a2184898",
          "0x2fa31a7ef4e264272a22782ae9092cd46d84c489b2f0fedf1abdac57aef4f71e",
          "0x039d659292e4e34b8f8d55cbf609982d5dd87ca2715875bc916e0b1ce36c5218",
          "0x18773605c3eb631e5718d4464a2c4cbe1de8c1ff4c6edf5930a8619786a5b9a9",
          "0x29940fd410cae7a7bba44f5b90bfa4ddf627bf99351a729308830b87ea4ab586",
          "0x1573ce82875ad48fa671878c5dbbb4eb60e7d3bc107d840733b81340ae861628",
          "0x060c89ce5c263405262f9480120850db51f792b96b909da2ad68c0172cf28515",
          "0x12fa4fd45cb064c711bb17be2cbd831cfd36440a94099878d242b461acab0d3c",
          "0x2df352948035ec5e15e6cc580f0d6760d0d8bcfb8f8728d4c31d46caf5725957",
          "0x2d516d066b9262f6a9d0ec33b676176e1ef20545f36477c946a1d98016fcdee0",
          "0x0634d845bcffbda3a6875ef3745abde328d92f2cfa0768286f023a33ebc48ec2",
          "0x27f51fe49fe915f62d04ff477b37032163ef864cb34493e535c70e940a1a1a9d",
          "0x0ab166942af71ce2a25e95002523b19b93aeb434fa4f58b0f408355b3edad84a",
          "0x0a30fa7e9d8e734d75881684f1faacf89c76c5758c1c32f87b19cfa4468e7d36",
          "0x060c89ce5c263405370a08b6d0302b0ba5067d090f372e12287c3eb27e000000",
          "0x0124ed3feff85704cb4661b932f64c795f68320181e920bb20e3d4bb45c19525",
          "0x07e9cf3f61b88fd7e58eb4676366a100e0c482b760c53e8f1cdd937ec0c48e1d",
          "0x07f21d6c685addacc008b3973ae25de51d899c64eb619f3cfa7be3b778a65b7c",
          "0x29c0a3f92039a2db604cb5a7d2f564d3c505bc4d21b3e2a25601ced4f56fbb76",
          "0x20e03b037d76f2c1cdd3c664a4c69a200aa6c54e2db3b837ecd0704167b7a78a",
          "0x254a2223f9c61c8df93d29ab265943abaa648d9b97f5ceb94c8866ea3c95f158",
          "0x0010b2e6207c947382e7540d1b82d1e2e1f6cdaa8d3b14265a511885a1400bc5",
          "0x030644e72e131a02ac5f7892263fefb6259228d42b4227788f519df4900d7aeb",
          "0x2f2b91a04f43fd7b356571ac6c81712dabd92ea26fb5a70fe9d24b4767da223c",
          "0x30041fa5a928bd53c6323779d0a8128d6df889403d62ce0f4c5d86a1220aae15",
          "0x06e84ced14e100dd18f0294f3458f1ef7a64c3abd9f1107d7d711fa517804a9c",
          "0x0ef70a0c8585f86ec70cf573b4b81e9bcb8ff7471a4b3a4066e49d85a55f63b1",
          "0x138f1764bf0cab47dacd22a4b3f361d912167e314137bf8ae37dce58b3e382b5",
          "0x012c7a8b30b4eb3571fb6ded0d8c3cf1f818380b2f30562be1b20df1dfeaa2bb",
          "0x05a87d92807e5e84dd239dc00a90413974e1281cd7af602ae9365c1ce392e4ca",
      },
      {
          "0x183227397098d014dc2822db40c0ac2e9419f4243cdcb848a1f0fac9f8000001",
          "0x2676094caa55c6970e879bd3e4e0c2b266852e1dd23eecf207aa65c27cef1718",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x2fa0a1411a6bdca4430e8a31f6b95622bcf52f1221d69b3a87741ee75ff60861",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0dcdcf1a8592c176d828c4f1bc010641f8d5daaeb024a3b0572614bb8d4b76b6",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0c25fbe8f815a5f0ad1f1da5444aff35fceba50167f818fd44fa151200736e7f",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x24e253551f980578421317bf8e27bd6a4e39164bc90f4435384ff03da6684972",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x22c5a2a8524e53ec9895eaed111377d08c1efdb8d49f3550674a9ec6b1b414ca",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x173933360c1ef0091fd01fd29f368c6847b1ced532f67a3479002bf6f7b1202a",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x05ba0b833e833f0324cabe3e915da9b03266b13aa27b8836433b4a44c212794d",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x27a3fe08749b4723f800831c880b99272349f8894fa25151ec688bf5eeed86b4",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x1624d655a6ff961dfcfb21887a32b66f0dfedaeebf275f53b6a3aa43b94edfd7",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x0a9866e360d0323a8435566e0855cb06c991ac0b1d7ea437c8593773ff4beb37",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x087bb636938680aedab8299b8b41856d07779378290e9552f753e5fd0a97b68f",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x21380da2bb08e0366fac23b5d51e43a158c504c28a25c08aeaa9c128b08c9182",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x1f903a712d8bc4b044a27c695d683c955cdacf1541f935d7d87dc17f23b4894b",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x2e21b6bd79e449ac920cfcdfa4314511c0ef62fa4a00aedeec11ace74109f7a1",
          "0x0000000000000000000000000000000000000000000000000000000000000000",
          "0x06e8003f08c8bf900e43a58734888024ef2b7ba61fdeec9627f970783410e8e9",
      }};
      // clang-format on
      expected_fixed_polys = CreatePolys(polys);
    }
    EXPECT_EQ(pkey.fixed_polys(), expected_fixed_polys);

    EXPECT_TRUE(pkey.permutation_proving_key().permutations().empty());
    EXPECT_TRUE(pkey.permutation_proving_key().polys().empty());
  }
}

TEST_F(SimpleLookupV1CircuitTest, CreateProof) {
  size_t n = 32;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  SimpleLookupCircuit<F, kBits, V1FloorPlanner> circuit(4);
  std::vector<SimpleLookupCircuit<F, kBits, V1FloorPlanner>> circuits = {
      circuit, std::move(circuit)};

  std::vector<Evals> instance_columns;
  std::vector<std::vector<Evals>> instance_columns_vec = {
      instance_columns, std::move(instance_columns)};

  ProvingKey<PCS> pkey;
  ASSERT_TRUE(pkey.Load(prover_.get(), circuit));
  prover_->CreateProof(pkey, std::move(instance_columns_vec), circuits);

  std::vector<uint8_t> proof = prover_->GetWriter()->buffer().owned_buffer();
  std::vector<uint8_t> expected_proof(std::begin(kExpectedMultiProof),
                                      std::end(kExpectedMultiProof));
  EXPECT_THAT(proof, testing::ContainerEq(expected_proof));
}

TEST_F(SimpleLookupV1CircuitTest, Verify) {
  size_t n = 32;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  SimpleLookupCircuit<F, kBits, V1FloorPlanner> circuit(4);

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  std::vector<uint8_t> owned_proof(std::begin(kExpectedProof),
                                   std::end(kExpectedProof));
  Verifier<PCS> verifier =
      CreateVerifier(CreateBufferWithProof(absl::MakeSpan(owned_proof)));
  std::vector<Evals> instance_columns;
  std::vector<std::vector<Evals>> instance_columns_vec = {
      std::move(instance_columns)};

  size_t num_circuits = instance_columns_vec.size();
  Proof<F, Commitment> proof;
  F h_eval;
  ASSERT_TRUE(verifier.VerifyProofForTesting(vkey, instance_columns_vec, &proof,
                                             &h_eval));

  std::vector<std::vector<Commitment>> expected_advice_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x21bc0e80e7e336f8509e19d20ef383f18eca429f4b3519cba53f1ef3bc57b62f",
         "0x20f77de309a3f81a09883e451c96e1b091f7fc7799e27905444c4759806fc6d2"},
    };
    expected_advice_commitments_vec.push_back(CreateCommitments(points));
  }
  EXPECT_EQ(proof.advices_commitments_vec, expected_advice_commitments_vec);

  EXPECT_TRUE(proof.challenges.empty());

  F expected_theta = F::FromHexString(
      "0x290f20708b8b7e7985088e9221743049fba8e682c6d7488e5213494da7649505");
  EXPECT_EQ(proof.theta, expected_theta);

  std::vector<std::vector<LookupPair<Commitment>>>
      expected_lookup_permuted_commitments_vec;
  {
    std::vector<Point> input_points = {
        {"0x1399b6806e72c562585457811c4a844d7420397b7dfce20050eb03c94c155987",
         "0x0e2f049e4b617f6d07f7cfbd90d8cead3b4368a413f2c71cc4e73f1612c412f6"},
    };
    std::vector<Point> table_points = {
        {"0x2c14068da6ea71c022de39eb055cf8db000ec431c9b1f44354b7a6a26d69c6b5",
         "0x1bb517ae1ce20941980e210163591fc2b6c48892105d68b9ad8f1d48dd9aa59c"},
    };
    expected_lookup_permuted_commitments_vec.push_back(
        CreateLookupPermutedCommitments(input_points, table_points));
  }
  EXPECT_EQ(proof.lookup_permuted_commitments_vec,
            expected_lookup_permuted_commitments_vec);

  F expected_beta = F::FromHexString(
      "0x12183c0dc15c5e6071d6beecad709fa4fb11c14ad6089f1b39027775560e190c");
  EXPECT_EQ(proof.beta, expected_beta);

  F expected_gamma = F::FromHexString(
      "0x1083f5b6c86390dfb35fbaeca4bf3fa53a05277508abc025bf7b05c4305b29a0");
  EXPECT_EQ(proof.gamma, expected_gamma);

  ASSERT_EQ(proof.permutation_product_commitments_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_commitments_vec[0].empty());

  std::vector<std::vector<Commitment>> expected_lookup_product_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x2ea932fda3339c0c944d8ff39105feba7e7413ccd9a3e0b6ce62776cc93ac5a3",
         "0x1b0446e5fd9752be88503dba0cfde03e808ce6114ed5d2766996b0a3e5717671"},
    };
    expected_lookup_product_commitments_vec.push_back(
        CreateCommitments(points));
  }
  EXPECT_EQ(proof.lookup_product_commitments_vec,
            expected_lookup_product_commitments_vec);

  Commitment expected_vanishing_random_poly_commitment;
  {
    expected_vanishing_random_poly_commitment = CreateCommitment(
        {"0x0000000000000000000000000000000000000000000000000000000000000001",
         "0x0000000000000000000000000000000000000000000000000000000000000002"});
  }
  EXPECT_EQ(proof.vanishing_random_poly_commitment,
            expected_vanishing_random_poly_commitment);

  F expected_y = F::FromHexString(
      "0x0ed52b381813f71a5b8c931fd9696534b96d49f2d21fa71a73c0ed19c5843953");
  EXPECT_EQ(proof.y, expected_y);

  std::vector<Commitment> expected_vanishing_h_poly_commitments;
  {
    std::vector<Point> points = {
        {"0x1b93f58d123830bee3670e142fda8410d3e3135ad72ce37d6deafc97d8c6c9fc",
         "0x2f5101e174e83a1e04f79bf48381b7369c256ae64bc5fefd0d499c6b81711abf"},
        {"0x29ce81333eed7dfbbe1a326434732cc34de492efbde732606098c9abe6b7456f",
         "0x2ad970331aee6de0a9464e6d4e2f928284c076c53b75f68280b9fe0c71590f9a"},
        {"0x10ae5be8e53bd0cb9c4df7e6520e95ec4252b6e848bdfa4a585a3418691849d5",
         "0x227b2836dd30acb1eb885302cd2ccb7092bccfe4fffe155eaa64da910044c8d6"},
        {"0x201786a6e18b711a9fbcd460339224550d22df3468b17a9ca4e8de4fc431beb9",
         "0x15aa249466f9008be03bbdaf7b79dc2dfcc3ba4aa6b7d3e6f7d8d172e4880e24"},
    };
    expected_vanishing_h_poly_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(proof.vanishing_h_poly_commitments,
            expected_vanishing_h_poly_commitments);

  F expected_x = F::FromHexString(
      "0x053a454eca02741827b727febbe275db5436ebd46105f169c4c82fdc6386f6c0");
  EXPECT_EQ(proof.x, expected_x);

  std::vector<std::vector<F>> expected_advice_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x009233a19dc91b3697c0ae89ffb566d2b0c50fa46ee09807723a4a9a4d36e17c",
    };
    expected_advice_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.advice_evals_vec, expected_advice_evals_vec);

  std::vector<F> expected_fixed_evals;
  {
    std::vector<std::string_view> evals = {
        "0x263971120ec538a63e643e779917712210979b723efc1d4444b675c7cc4571ce",
        "0x2ec8b1b7c8f5fa47d4f47871747935752be7c40948ae1835f9157ba3e6c6ae0c",
    };
    expected_fixed_evals = CreateEvals(evals);
  }
  EXPECT_EQ(proof.fixed_evals, expected_fixed_evals);

  F expected_vanishing_random_eval = F::FromHexString(
      "0x0000000000000000000000000000000000000000000000000000000000000001");
  EXPECT_EQ(proof.vanishing_random_eval, expected_vanishing_random_eval);

  EXPECT_TRUE(proof.common_permutation_evals.empty());

  ASSERT_EQ(proof.permutation_product_evals_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_evals_vec[0].empty());

  ASSERT_EQ(proof.permutation_product_next_evals_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_next_evals_vec[0].empty());

  ASSERT_EQ(proof.permutation_product_last_evals_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_last_evals_vec[0].empty());

  std::vector<std::vector<F>> expected_lookup_product_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x1b43efa336446cbed3db37cc0573dc6735e43c99e868d8cb69fa59168330e94a",
    };
    expected_lookup_product_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_product_evals_vec, expected_lookup_product_evals_vec);

  std::vector<std::vector<F>> expected_lookup_product_next_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x131aac22305255dddbef5737fe8069a3477ff8ebaef330e9f9fb2b6b53515f6e",
    };
    expected_lookup_product_next_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_product_next_evals_vec,
            expected_lookup_product_next_evals_vec);

  std::vector<std::vector<F>> expected_lookup_permuted_input_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x19d4e03ebb88f63290feafae022962f2cf6c03b7b46829055e57ae3537e51b2c",
    };
    expected_lookup_permuted_input_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_permuted_input_evals_vec,
            expected_lookup_permuted_input_evals_vec);

  std::vector<std::vector<F>> expected_lookup_permuted_input_inv_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x1cf3bb85d45d3cc0233d22f467bf9f741770534b88fa3110f80fdb68205cff10",
    };
    expected_lookup_permuted_input_inv_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_permuted_input_inv_evals_vec,
            expected_lookup_permuted_input_inv_evals_vec);

  std::vector<std::vector<F>> expected_lookup_permuted_table_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x0cbe51be65ae0038a4c44bbbe67909f18601a187ceaeb72c4d8ec0924d32626f",
    };
    expected_lookup_permuted_table_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_permuted_table_evals_vec,
            expected_lookup_permuted_table_evals_vec);

  F expected_h_eval = F::FromHexString(
      "0x01d8f3b1cc0f65fe4a711b22ac8843f20b8f0ca5cf6f01757c018ef1a9a44c7f");
  EXPECT_EQ(h_eval, expected_h_eval);
}

TEST_F(SimpleLookupV1CircuitTest, VerifyMultiProof) {
  size_t n = 32;
  CHECK(prover_->pcs().UnsafeSetup(n, F(2)));
  prover_->set_domain(Domain::Create(n));

  SimpleLookupCircuit<F, kBits, V1FloorPlanner> circuit(4);

  VerifyingKey<PCS> vkey;
  ASSERT_TRUE(vkey.Load(prover_.get(), circuit));

  std::vector<uint8_t> owned_proof(std::begin(kExpectedMultiProof),
                                   std::end(kExpectedMultiProof));
  Verifier<PCS> verifier =
      CreateVerifier(CreateBufferWithProof(absl::MakeSpan(owned_proof)));
  std::vector<Evals> instance_columns;
  std::vector<std::vector<Evals>> instance_columns_vec = {
      instance_columns, std::move(instance_columns)};

  size_t num_circuits = instance_columns_vec.size();
  Proof<F, Commitment> proof;
  F h_eval;
  ASSERT_TRUE(verifier.VerifyProofForTesting(vkey, instance_columns_vec, &proof,
                                             &h_eval));

  std::vector<std::vector<Commitment>> expected_advice_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x21bc0e80e7e336f8509e19d20ef383f18eca429f4b3519cba53f1ef3bc57b62f",
         "0x20f77de309a3f81a09883e451c96e1b091f7fc7799e27905444c4759806fc6d2"},
    };
    expected_advice_commitments_vec.push_back(CreateCommitments(points));

    points = {
        {"0x21bc0e80e7e336f8509e19d20ef383f18eca429f4b3519cba53f1ef3bc57b62f",
         "0x20f77de309a3f81a09883e451c96e1b091f7fc7799e27905444c4759806fc6d2"},
    };
    expected_advice_commitments_vec.push_back(CreateCommitments(points));
  }
  EXPECT_EQ(proof.advices_commitments_vec, expected_advice_commitments_vec);

  EXPECT_TRUE(proof.challenges.empty());

  F expected_theta = F::FromHexString(
      "0x19f2e881193a280d2d77a8bc0f9edf053732ec86f93279a580830fb2befeccd7");
  EXPECT_EQ(proof.theta, expected_theta);

  std::vector<std::vector<LookupPair<Commitment>>>
      expected_lookup_permuted_commitments_vec;
  {
    std::vector<Point> input_points = {
        {"0x0570bae08fb2d8f086ef87cdba76c74d323b3829e63be2b8b1a0140a7c8263a1",
         "0x0eb968ee1f6663417be8e37d714a7dec22a717d74bc3c8bc1f4a4bd58da49eed"},
    };
    std::vector<Point> table_points = {
        {"0x1f229d9bf8e868c447c012f97463d59ace9d0242b0a72731268ed1abd2a5126e",
         "0x29edba749b5dc16811d78ee18bce88c5c61cd56230ad68e73c578361062a317d"},
    };
    expected_lookup_permuted_commitments_vec.push_back(
        CreateLookupPermutedCommitments(input_points, table_points));

    input_points = {
        {"0x19102232006197bab6f36bc6c3c5d583d42e91a11a1bf6f4fa84acbb47d69e59",
         "0x1bfa3165ed3f9e4941b37e1c8f4d4e05e1c45c82b55168f5b1adc4af96cd81fb"},
    };
    table_points = {
        {"0x20e6b8f03512b410090e71bb5e2aceb7f55aada11f5fecc4f0ca915efbc1916d",
         "0x0386bd00ee929fd8e19e80f18f372aca85b7243d28457923d49c511058e3cd55"},
    };
    expected_lookup_permuted_commitments_vec.push_back(
        CreateLookupPermutedCommitments(input_points, table_points));
  }
  EXPECT_EQ(proof.lookup_permuted_commitments_vec,
            expected_lookup_permuted_commitments_vec);

  F expected_beta = F::FromHexString(
      "0x006a00a3a227c15c50d662d1a7d34e8a1dca3445c79fe17bd0bba1bd324d513e");
  EXPECT_EQ(proof.beta, expected_beta);

  F expected_gamma = F::FromHexString(
      "0x08aacbb21c2c304fa7a8f64d8554dba8e3ad16dba09ab0cba0b711c28210802c");
  EXPECT_EQ(proof.gamma, expected_gamma);

  ASSERT_EQ(proof.permutation_product_commitments_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_commitments_vec[0].empty());

  std::vector<std::vector<Commitment>> expected_lookup_product_commitments_vec;
  {
    std::vector<Point> points = {
        {"0x1a498c9b0e55c404e7634f97ab9f18c1f72d90f60b0a76551d884d38323d228d",
         "0x158baeb0a976e042a02134658e53c74585234c9f27ed3a9ec40454bca1ae124b"},
    };
    expected_lookup_product_commitments_vec.push_back(
        CreateCommitments(points));

    points = {
        {"0x2540a0a59f6b7792beecb8fe9b97cfa72f6796121d5cc9013cbc5a81b701330d",
         "0x00a3fb614af06a4322b1c0fdd2c9c1b5e47f873c8d4ab47aa217a1fec5e39cd1"},
    };
    expected_lookup_product_commitments_vec.push_back(
        CreateCommitments(points));
  }
  EXPECT_EQ(proof.lookup_product_commitments_vec,
            expected_lookup_product_commitments_vec);

  Commitment expected_vanishing_random_poly_commitment;
  {
    expected_vanishing_random_poly_commitment = CreateCommitment(
        {"0x0000000000000000000000000000000000000000000000000000000000000001",
         "0x0000000000000000000000000000000000000000000000000000000000000002"});
  }
  EXPECT_EQ(proof.vanishing_random_poly_commitment,
            expected_vanishing_random_poly_commitment);

  F expected_y = F::FromHexString(
      "0x081da6a34bb45df66fa1bbda6850c3b1a5aad71cbc13926a54439251cc884074");
  EXPECT_EQ(proof.y, expected_y);

  std::vector<Commitment> expected_vanishing_h_poly_commitments;
  {
    std::vector<Point> points = {
        {"0x059527f1054e9fe9a365b8640dfe8dc05a72a3054ba0c85d61e00c0d0ddeac2f",
         "0x244c3a6aead08117b86d1f46cb5dc9175ba3a3ae928955a158345f0a5d62ce4e"},
        {"0x005115da9359021c107cd41a93303355ba368d105cd023d3771f698301620b28",
         "0x21e75a51fb0bf9aeb422e3682af875aeede51f33cca14b4fa4fd8abf569623d2"},
        {"0x1c792a69e15933c38b874e795666b11fb238e847be54699e844e739d66ac606c",
         "0x2e2d4868337c81d3a8557f15231d79cdaa609ec6778d2bf386e04aacace2619f"},
        {"0x2bdf8b016e39674950220c97e547dffff827a3e9e82bb207de41f372dd2cb352",
         "0x1b37ee7442c7c4072fc090ac2b46cc81e7f222306b35d400b0f94db864c82f81"},
    };
    expected_vanishing_h_poly_commitments = CreateCommitments(points);
  }
  EXPECT_EQ(proof.vanishing_h_poly_commitments,
            expected_vanishing_h_poly_commitments);

  F expected_x = F::FromHexString(
      "0x0a7f0b32529da1d83a2e86cdd521aa1b38b73f08d834762a1e17ba859e38f6c3");
  EXPECT_EQ(proof.x, expected_x);

  std::vector<std::vector<F>> expected_advice_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x049e7bf7ba833e877afca8ed2ff2069293a640da31a9bf2448cfb61ba80349cc",
    };
    expected_advice_evals_vec.push_back(CreateEvals(evals));

    evals = {
        "0x049e7bf7ba833e877afca8ed2ff2069293a640da31a9bf2448cfb61ba80349cc",
    };
    expected_advice_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.advice_evals_vec, expected_advice_evals_vec);

  std::vector<F> expected_fixed_evals;
  {
    std::vector<std::string_view> evals = {
        "0x18acc6dc3e6b2c1b16c73e6cd567801ce9aca62901c597d3f19d963487e01e8b",
        "0x1fdaa932ed5c8991e9c57666eb4a26c38a1f150df1eba370c02a9e336b2023ca",
    };
    expected_fixed_evals = CreateEvals(evals);
  }
  EXPECT_EQ(proof.fixed_evals, expected_fixed_evals);

  F expected_vanishing_random_eval = F::FromHexString(
      "0x0000000000000000000000000000000000000000000000000000000000000001");
  EXPECT_EQ(proof.vanishing_random_eval, expected_vanishing_random_eval);

  EXPECT_TRUE(proof.common_permutation_evals.empty());

  ASSERT_EQ(proof.permutation_product_evals_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_evals_vec[0].empty());

  ASSERT_EQ(proof.permutation_product_next_evals_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_next_evals_vec[0].empty());

  ASSERT_EQ(proof.permutation_product_last_evals_vec.size(), num_circuits);
  EXPECT_TRUE(proof.permutation_product_last_evals_vec[0].empty());

  std::vector<std::vector<F>> expected_lookup_product_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x075c1ccae544df1168ad14e7b10a01c8b1330197c2300dcfa185ff35bf1b0993"};
    expected_lookup_product_evals_vec.push_back(CreateEvals(evals));

    evals = {
        "0x118046fbb3c10e170f4a5714cb9a4dc4fa5b9326593c352256c381df4d3fa001"};
    expected_lookup_product_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_product_evals_vec, expected_lookup_product_evals_vec);

  std::vector<std::vector<F>> expected_lookup_product_next_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x159dd6e547aa8ce10ebb7c321c2d84ec7cdd28d5d3de6b364df173e1c23ebeca"};
    expected_lookup_product_next_evals_vec.push_back(CreateEvals(evals));

    evals = {
        "0x297a9defb897153d6d269a9544897aa58372b33592105de2a041d0c9ec9bac1b"};
    expected_lookup_product_next_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_product_next_evals_vec,
            expected_lookup_product_next_evals_vec);

  std::vector<std::vector<F>> expected_lookup_permuted_input_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x07305e96e2abe786082857323ccdcc263079cbcda3a4243d5aa74cdd2eeb4ad2"};
    expected_lookup_permuted_input_evals_vec.push_back(CreateEvals(evals));

    evals = {
        "0x099ca8ebe9c8775bbbbfb9aa08d9ee29c3f7a619cad1e8a3053f785f973fb495"};
    expected_lookup_permuted_input_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_permuted_input_evals_vec,
            expected_lookup_permuted_input_evals_vec);

  std::vector<std::vector<F>> expected_lookup_permuted_input_inv_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x0a609f45a209c538da6647253d32fb29f7a7f910df06c97f9624b7cd8b0579e5"};
    expected_lookup_permuted_input_inv_evals_vec.push_back(CreateEvals(evals));

    evals = {
        "0x1acff9a60c9c210c040684f117fad6974057c586c300f96d56ded903d7ef4cac"};
    expected_lookup_permuted_input_inv_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_permuted_input_inv_evals_vec,
            expected_lookup_permuted_input_inv_evals_vec);

  std::vector<std::vector<F>> expected_lookup_permuted_table_evals_vec;
  {
    std::vector<std::string_view> evals = {
        "0x0501ddc79fddb8f7cf1f7d3a27098f7d42ce72d6cfb6f4970e9def307e5d9c53"};
    expected_lookup_permuted_table_evals_vec.push_back(CreateEvals(evals));

    evals = {
        "0x27770cf8d9d6b6d96756b34311135d90410919caf67944b07aeebcac20ac52a1"};
    expected_lookup_permuted_table_evals_vec.push_back(CreateEvals(evals));
  }
  EXPECT_EQ(proof.lookup_permuted_table_evals_vec,
            expected_lookup_permuted_table_evals_vec);

  F expected_h_eval = F::FromHexString(
      "0x22000b7c7cb0b3aee0dc8eae939b3dfc5c5482b0e33231fa54cb1ed46e24eab8");
  EXPECT_EQ(h_eval, expected_h_eval);
}

}  // namespace tachyon::zk::halo2
