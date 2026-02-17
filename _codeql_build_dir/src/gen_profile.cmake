file(READ ${PROFILE_INPUT_FILE} JSON)
set(PROFILE "\
#include <string>\n\
namespace murxla {\n\
namespace ${SOLVER} {\n\
  static const std::string s_profile=R\"(${JSON})\";\n\
}\n}\n")
file(WRITE "${PROFILE_OUTPUT_FILE}" "${PROFILE}")