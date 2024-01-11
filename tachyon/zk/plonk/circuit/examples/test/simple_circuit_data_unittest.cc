#include "tachyon/zk/plonk/circuit/examples/test/simple_circuit_data.h"

#include <string>

#include "gtest/gtest.h"

#include "tachyon/zk/plonk/circuit/examples/circuit_test.h"

namespace tachyon::zk::halo2 {
namespace {

class SimpleCircuitDataTest : public CircuitTest {};

}  // namespace

TEST_F(SimpleCircuitDataTest, JsonValueConverter) {
  Commitment empty_commitment;

  SimpleCircuitData<PCS> simple_circuit_data;
  simple_circuit_data.circuit_name = "simple_circuit";
  simple_circuit_data.n = 16;
  simple_circuit_data.extended_n = 32;
  simple_circuit_data.fixed_columns = {
      // clang-format off
      {
        CreatePoly({"0x1b386c209eabea1777ad2736a8d8c1b4669d32a8c4784f51b62f1a2337000001"}),
        CreateColumn({"0x0000000000000000000000000000000000000000000000000000000000000007"}),
        empty_commitment
      },
      {
        CreatePoly({"0x1b386c209eabea1777ad2736a8d8c1b4669d32a8c4784f51b62f1a2337000001"}),
        CreateColumn({"0x0000000000000000000000000000000000000000000000000000000000000007"}),
        empty_commitment
      },
      // clang-format on
  };
  std::string json = base::WriteToJson(simple_circuit_data);
  std::cout << json << std::endl;
}

}  // namespace tachyon::zk::halo2
