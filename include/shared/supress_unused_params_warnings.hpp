#ifndef SUPRESS_PARAMS_WARNING_H
#define SUPRESS_PARAMS_WARNING_H

#define UNUSED_STRINGIFY(macro_arg_string_literal)                         \
#macro_arg_string_literal
#define UNUSED_PARAMS(macro_arg_parameter)                                \
  _Pragma(UNUSED_STRINGIFY(unused(macro_arg_parameter)))

#endif // !SUPRESS_PARAMS_WARNING_H
