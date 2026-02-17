#include <string>
namespace murxla {
namespace z3 {
  static const std::string s_profile=R"({
  "theories": {
    "include": [
      "THEORY_ARRAY",
      "THEORY_BOOL",
      "THEORY_BV",
      "THEORY_DT",
      "THEORY_FP",
      "THEORY_INT",
      "THEORY_QUANT",
      "THEORY_REAL",
      "THEORY_STRING",
      "THEORY_UF"
    ]
  },

  "sorts": {
    "exclude": [
      "SORT_REGLAN",
      "SORT_BAG",
      "SORT_SET"
    ]
  }
}
)";
}
}
