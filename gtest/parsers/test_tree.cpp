#include <parsers/fixtures.hpp>

namespace json_parser::test_tree {

struct TreeNode {
  std::strong_ordering operator<=>(const TreeNode&) const;
  bool operator==(const TreeNode&) const;

  std::string tree_value;
  tgbm::api::optional<std::vector<tgbm::box<TreeNode>>> children;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return name == "tree_value";
  }
};

std::strong_ordering TreeNode::operator<=>(const TreeNode&) const = default;
bool TreeNode::operator==(const TreeNode&) const = default;

JSON_PARSE_TEST(SimpleTree, TreeNode) {
  TreeNode expected;
  expected.tree_value = "root";
  expected.children = std::vector<tgbm::box<TreeNode>>{
      tgbm::box<TreeNode>{TreeNode{"child1", std::nullopt}},
      tgbm::box<TreeNode>{TreeNode{"child2", std::vector<tgbm::box<TreeNode>>{tgbm::box<TreeNode>{
                                                 TreeNode{"grandchild1", std::nullopt}}}}}};

  auto json = R"(
            {
                "tree_value": "root",
                "children": [
                    {
                        "tree_value": "child1"
                    },
                    {
                        "tree_value": "child2",
                        "children": [
                            {
                                "tree_value": "grandchild1"
                            }
                        ]
                    }
                ]
            }
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(LeafOnly, TreeNode) {
  TreeNode expected;
  expected.tree_value = "single";

  auto json = R"(
            {
                "tree_value": "single"
            }
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

}  // namespace json_parser::test_tree
