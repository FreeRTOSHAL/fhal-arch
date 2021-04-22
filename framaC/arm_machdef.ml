(* SPDX-License-Identifier: MIT *)
(*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 *)
open Cil_types
(* Based on https://www.cs.dartmouth.edu/~trdata/reports/TR2018-863.pdf *)
let arm = {
  version = "arm machdep";
  compiler = "gcc";
  cpp_arch_flags = [];
  sizeof_short = 2;
  sizeof_int = 4;
  sizeof_long = 4;
  sizeof_longlong = 8;
  sizeof_ptr = 4;
  sizeof_float = 4;
  sizeof_double = 8;
  sizeof_longdouble = 8;
  sizeof_void = 4;
  sizeof_fun = 4;
  size_t = "unsigned int";
  wchar_t = "int";
  ptrdiff_t = "int";
  alignof_short = 2;
  alignof_int = 4;
  alignof_long = 4;
  alignof_longlong = 8;
  alignof_ptr = 4;
  alignof_float = 4;
  alignof_double = 8;
  alignof_longdouble = 8;
  alignof_str = 1;
  alignof_fun = 1;
  alignof_aligned = 8;
  char_is_unsigned = false;
  const_string_literals = false;
  little_endian = true;
  underscore_name = false;
  has__builtin_va_list = true;
  
}
(* __thread_is_keyword = true; not suppored any more? its documented in the manual *)
let () = File.new_machdep "arm" arm
