// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Tests for [requires] using requires.emb.
#include <gtest/gtest.h>
#include <stdint.h>

#include <vector>

#include "testdata/requires.emb.h"

namespace emboss {
namespace test {
namespace {

TEST(RequiresIntegers, Ok) {
  ::std::array</**/ ::std::uint8_t, 3> buffer = {0, 0, 0};
  auto view = MakeRequiresIntegersView(&buffer);
  EXPECT_TRUE(view.zero_through_nine().Ok());
  EXPECT_FALSE(view.ten_through_twenty().Ok());
  EXPECT_TRUE(view.disjoint().Ok());
  EXPECT_FALSE(view.ztn_plus_ttt().Ok());
  EXPECT_TRUE(view.zero_through_nine_plus_five().Ok());
  EXPECT_FALSE(view.alias_of_zero_through_nine().Ok());

  buffer[1] = 10;
  EXPECT_TRUE(view.zero_through_nine().Ok());
  EXPECT_TRUE(view.ten_through_twenty().Ok());
  EXPECT_TRUE(view.disjoint().Ok());
  EXPECT_TRUE(view.ztn_plus_ttt().Ok());
  EXPECT_TRUE(view.zero_through_nine_plus_five().Ok());
  EXPECT_FALSE(view.alias_of_zero_through_nine().Ok());

  buffer[0] = 2;
  EXPECT_TRUE(view.zero_through_nine().Ok());
  EXPECT_TRUE(view.ten_through_twenty().Ok());
  EXPECT_TRUE(view.disjoint().Ok());
  EXPECT_TRUE(view.ztn_plus_ttt().Ok());
  EXPECT_TRUE(view.zero_through_nine_plus_five().Ok());
  EXPECT_TRUE(view.alias_of_zero_through_nine().Ok());
  EXPECT_FALSE(view.Ok());

  buffer[1] = 12;
  EXPECT_TRUE(view.zero_through_nine().Ok());
  EXPECT_TRUE(view.ten_through_twenty().Ok());
  EXPECT_TRUE(view.disjoint().Ok());
  EXPECT_TRUE(view.ztn_plus_ttt().Ok());
  EXPECT_TRUE(view.zero_through_nine_plus_five().Ok());
  EXPECT_TRUE(view.alias_of_zero_through_nine().Ok());
  EXPECT_TRUE(view.Ok());
}

TEST(RequiresIntegers, CouldWriteValue) {
  ::std::array</**/ ::std::uint8_t, 3> buffer = {0, 0, 0};
  auto view = MakeRequiresIntegersView(&buffer);

  EXPECT_TRUE(view.zero_through_nine().CouldWriteValue(0));
  EXPECT_TRUE(view.zero_through_nine().CouldWriteValue(9));
  EXPECT_FALSE(view.zero_through_nine().CouldWriteValue(-1));
  EXPECT_FALSE(view.zero_through_nine().CouldWriteValue(10));

  EXPECT_TRUE(view.ten_through_twenty().CouldWriteValue(10));
  EXPECT_TRUE(view.ten_through_twenty().CouldWriteValue(20));
  EXPECT_FALSE(view.ten_through_twenty().CouldWriteValue(-1));
  EXPECT_FALSE(view.ten_through_twenty().CouldWriteValue(9));
  EXPECT_FALSE(view.ten_through_twenty().CouldWriteValue(21));

  EXPECT_TRUE(view.disjoint().CouldWriteValue(0));
  EXPECT_TRUE(view.disjoint().CouldWriteValue(5));
  EXPECT_TRUE(view.disjoint().CouldWriteValue(15));
  EXPECT_TRUE(view.disjoint().CouldWriteValue(20));
  EXPECT_FALSE(view.disjoint().CouldWriteValue(-1));
  EXPECT_FALSE(view.disjoint().CouldWriteValue(6));
  EXPECT_FALSE(view.disjoint().CouldWriteValue(14));
  EXPECT_FALSE(view.disjoint().CouldWriteValue(21));
}

TEST(RequiresBools, Ok) {
  ::std::array</**/ ::std::uint8_t, 1> buffer = {0};
  auto view = MakeRequiresBoolsView(&buffer);
  EXPECT_TRUE(view.must_be_false().Ok());
  EXPECT_FALSE(view.must_be_true().Ok());
  EXPECT_TRUE(view.b_must_be_false().Ok());
  EXPECT_FALSE(view.alias_of_a_must_be_true().Ok());
  EXPECT_FALSE(view.Ok());

  view.a().Write(true);
  view.must_be_true().Write(true);
  EXPECT_TRUE(view.must_be_false().Ok());
  EXPECT_TRUE(view.must_be_true().Ok());
  EXPECT_TRUE(view.b_must_be_false().Ok());
  EXPECT_TRUE(view.alias_of_a_must_be_true().Ok());
  EXPECT_TRUE(view.Ok());
}

TEST(RequiresBools, CouldWriteValue) {
  ::std::array</**/ ::std::uint8_t, 1> buffer = {0};
  auto view = MakeRequiresBoolsView(&buffer);

  EXPECT_TRUE(view.a().CouldWriteValue(true));
  EXPECT_TRUE(view.a().CouldWriteValue(false));
  EXPECT_TRUE(view.b().CouldWriteValue(true));
  EXPECT_TRUE(view.b().CouldWriteValue(false));
  EXPECT_TRUE(view.must_be_true().CouldWriteValue(true));
  EXPECT_FALSE(view.must_be_true().CouldWriteValue(false));
  EXPECT_FALSE(view.must_be_false().CouldWriteValue(true));
  EXPECT_TRUE(view.must_be_false().CouldWriteValue(false));
  EXPECT_TRUE(view.alias_of_a_must_be_true().CouldWriteValue(true));
  EXPECT_FALSE(view.alias_of_a_must_be_true().CouldWriteValue(false));
  EXPECT_DEATH(view.alias_of_a_must_be_true().Write(false), "");
}

TEST(RequiresEnums, Ok) {
  ::std::array</**/ ::std::uint8_t, 3> buffer = {0, 0, 0};
  auto view = MakeRequiresEnumsView(&buffer);
  EXPECT_TRUE(view.a().Ok());
  EXPECT_TRUE(view.b().Ok());
  EXPECT_TRUE(view.c().Ok());
  EXPECT_TRUE(view.filtered_a().Ok());
  EXPECT_FALSE(view.alias_of_a().Ok());
  EXPECT_FALSE(view.Ok());

  view.a().Write(RequiresEnums::Enum::EN1);
  EXPECT_TRUE(view.a().Ok());
  EXPECT_TRUE(view.b().Ok());
  EXPECT_TRUE(view.c().Ok());
  EXPECT_TRUE(view.filtered_a().Ok());
  EXPECT_TRUE(view.alias_of_a().Ok());
  EXPECT_TRUE(view.Ok());

  view.b().Write(RequiresEnums::Enum::EN1);
  EXPECT_TRUE(view.a().Ok());
  EXPECT_TRUE(view.b().Ok());
  EXPECT_TRUE(view.c().Ok());
  EXPECT_TRUE(view.filtered_a().Ok());
  EXPECT_TRUE(view.alias_of_a().Ok());
  EXPECT_FALSE(view.Ok());

  buffer[2] = 2;
  EXPECT_FALSE(view.c().Ok());
}

TEST(RequiresEnums, CouldWriteValue) {
  ::std::array</**/ ::std::uint8_t, 3> buffer = {0, 0, 0};
  auto view = MakeRequiresEnumsView(&buffer);

  EXPECT_TRUE(view.a().CouldWriteValue(RequiresEnums::Enum::EN0));
  EXPECT_TRUE(view.a().CouldWriteValue(RequiresEnums::Enum::EN3));
  EXPECT_TRUE(view.b().CouldWriteValue(RequiresEnums::Enum::EN0));
  EXPECT_TRUE(view.b().CouldWriteValue(RequiresEnums::Enum::EN3));
  EXPECT_TRUE(view.c().CouldWriteValue(RequiresEnums::Enum::EN0));
  EXPECT_TRUE(view.c().CouldWriteValue(RequiresEnums::Enum::EN1));
  EXPECT_FALSE(view.c().CouldWriteValue(RequiresEnums::Enum::EN2));
  EXPECT_FALSE(view.c().CouldWriteValue(RequiresEnums::Enum::EN3));
  EXPECT_FALSE(view.alias_of_a().CouldWriteValue(RequiresEnums::Enum::EN0));
  EXPECT_TRUE(view.alias_of_a().CouldWriteValue(RequiresEnums::Enum::EN1));
  EXPECT_FALSE(view.alias_of_a().CouldWriteValue(RequiresEnums::Enum::EN2));
  EXPECT_FALSE(view.alias_of_a().CouldWriteValue(RequiresEnums::Enum::EN3));
}

TEST(RequiresWithOptionalFields, Ok) {
  ::std::array</**/ ::std::uint8_t, 1> buffer = {0};
  auto view = MakeRequiresWithOptionalFieldsView(&buffer);
  EXPECT_FALSE(view.Ok());
  view.a().Write(true);
  EXPECT_TRUE(view.Ok());
  EXPECT_FALSE(view.has_b().Value());
  view.b_exists().Write(true);
  EXPECT_TRUE(view.b().Ok());
  EXPECT_FALSE(view.b_true().Ok());
  view.b_true().Write(true);
  EXPECT_TRUE(view.b_true().Ok());
  EXPECT_TRUE(view.Ok());
  view.a().Write(false);
  EXPECT_TRUE(view.Ok());
}

TEST(RequiresWithOptionalFields, CouldWriteValue) {
  ::std::array</**/ ::std::uint8_t, 1> buffer = {0};
  auto view = MakeRequiresWithOptionalFieldsView(&buffer);
  view.b_exists().Write(true);

  EXPECT_TRUE(view.a().CouldWriteValue(true));
  EXPECT_TRUE(view.a().CouldWriteValue(false));
  EXPECT_TRUE(view.b().CouldWriteValue(true));
  EXPECT_TRUE(view.b().CouldWriteValue(false));
  EXPECT_TRUE(view.b_true().CouldWriteValue(true));
  EXPECT_FALSE(view.b_true().CouldWriteValue(false));
}

}  // namespace
}  // namespace test
}  // namespace emboss
