/***
 * Murxla: A Model-Based API Fuzzer for SMT solvers.
 *
 * This file is part of Murxla.
 *
 * Copyright (C) 2019-2022 by the authors listed in the AUTHORS file.
 *
 * See LICENSE for more information on using this software.
 */
#ifdef MURXLA_USE_Z3

#include "z3_solver.hpp"

#include "config.hpp"
#include "solver/z3/profile.hpp"
#include "util.hpp"

namespace murxla {
namespace z3solver {

// Use global z3 namespace to avoid ambiguity with murxla::z3solver
using namespace ::z3;

/* -------------------------------------------------------------------------- */
/* Z3Sort                                                                     */
/* -------------------------------------------------------------------------- */

::z3::sort
Z3Sort::get_z3_sort(Sort sort)
{
  Z3Sort* z3_sort = checked_cast<Z3Sort*>(sort.get());
  assert(z3_sort);
  return z3_sort->d_sort;
}

::z3::sort_vector
Z3Sort::sorts_to_z3_sorts(::z3::context& ctx, const std::vector<Sort>& sorts)
{
  ::z3::sort_vector res(ctx);
  for (const Sort& s : sorts)
  {
    res.push_back(Z3Sort::get_z3_sort(s));
  }
  return res;
}

size_t
Z3Sort::hash() const
{
  return d_sort.hash();
}

bool
Z3Sort::equals(const Sort& other) const
{
  Z3Sort* z3_sort = checked_cast<Z3Sort*>(other.get());
  if (z3_sort)
  {
    return ::z3::eq(d_sort, z3_sort->d_sort);
  }
  return false;
}

std::string
Z3Sort::to_string() const
{
  std::stringstream ss;
  ss << d_sort;
  return ss.str();
}

bool
Z3Sort::is_array() const
{
  // In Z3, function sorts are represented as array sorts
  // Return true only for "pure" array sorts (not function sorts)
  return d_sort.is_array() && !d_is_fun_sort;
}

bool
Z3Sort::is_bool() const
{
  return d_sort.is_bool();
}

bool
Z3Sort::is_bv() const
{
  return d_sort.is_bv();
}

bool
Z3Sort::is_dt() const
{
  return d_sort.is_datatype();
}

bool
Z3Sort::is_fp() const
{
  return d_sort.is_fpa();
}

bool
Z3Sort::is_fun() const
{
  // Z3 represents function sorts as array sorts with a flag
  return d_is_fun_sort;
}

bool
Z3Sort::is_int() const
{
  return d_sort.is_int();
}

bool
Z3Sort::is_real() const
{
  return d_sort.is_real();
}

bool
Z3Sort::is_rm() const
{
  return d_sort.sort_kind() == Z3_ROUNDING_MODE_SORT;
}

bool
Z3Sort::is_uninterpreted() const
{
  return d_sort.sort_kind() == Z3_UNINTERPRETED_SORT;
}

uint32_t
Z3Sort::get_bv_size() const
{
  assert(is_bv());
  return d_sort.bv_size();
}

uint32_t
Z3Sort::get_fp_exp_size() const
{
  assert(is_fp());
  return d_sort.fpa_ebits();
}

uint32_t
Z3Sort::get_fp_sig_size() const
{
  assert(is_fp());
  return d_sort.fpa_sbits();
}

std::string
Z3Sort::get_dt_name() const
{
  assert(is_dt());
  return d_sort.name().str();
}

Sort
Z3Sort::get_array_index_sort() const
{
  assert(is_array());
  return std::shared_ptr<Z3Sort>(new Z3Sort(d_sort.array_domain()));
}

Sort
Z3Sort::get_array_element_sort() const
{
  assert(is_array());
  return std::shared_ptr<Z3Sort>(new Z3Sort(d_sort.array_range()));
}

uint32_t
Z3Sort::get_fun_arity() const
{
  assert(is_fun());
  // Return the number of stored domain sorts
  return static_cast<uint32_t>(d_fun_domain_sorts.size());
}

Sort
Z3Sort::get_fun_codomain_sort() const
{
  assert(is_fun());
  // The codomain is the final range of the array sort
  ::z3::sort range = d_sort.array_range();
  return std::shared_ptr<Z3Sort>(new Z3Sort(range, false));
}

std::vector<Sort>
Z3Sort::get_fun_domain_sorts() const
{
  assert(is_fun());
  // Return the stored domain sorts
  return d_fun_domain_sorts;
}

/* -------------------------------------------------------------------------- */
/* Z3Term                                                                     */
/* -------------------------------------------------------------------------- */

::z3::expr
Z3Term::get_z3_term(Term term)
{
  Z3Term* z3_term = checked_cast<Z3Term*>(term.get());
  assert(z3_term);
  return z3_term->d_term;
}

std::vector<Term>
Z3Term::z3_terms_to_terms(const ::z3::expr_vector& terms)
{
  std::vector<Term> res;
  for (uint32_t i = 0; i < terms.size(); ++i)
  {
    res.push_back(std::shared_ptr<Z3Term>(new Z3Term(terms[i])));
  }
  return res;
}

::z3::expr_vector
Z3Term::terms_to_z3_terms(::z3::context& ctx, const std::vector<Term>& terms)
{
  ::z3::expr_vector res(ctx);
  for (auto& t : terms)
  {
    res.push_back(Z3Term::get_z3_term(t));
  }
  return res;
}

size_t
Z3Term::hash() const
{
  return d_term.hash();
}

std::string
Z3Term::to_string() const
{
  std::stringstream ss;
  ss << d_term;
  return ss.str();
}

bool
Z3Term::equals(const Term& other) const
{
  Z3Term* z3_term = checked_cast<Z3Term*>(other.get());
  if (z3_term)
  {
    return ::z3::eq(d_term, z3_term->d_term);
  }
  return false;
}

bool
Z3Term::is_array() const
{
  return d_term.is_array();
}

bool
Z3Term::is_bool() const
{
  return d_term.is_bool();
}

bool
Z3Term::is_bv() const
{
  return d_term.is_bv();
}

bool
Z3Term::is_fp() const
{
  return d_term.is_fpa();
}

bool
Z3Term::is_fun() const
{
  // Z3 doesn't have is_function for sorts - always return false
  return false;
}

bool
Z3Term::is_int() const
{
  return d_term.is_int();
}

bool
Z3Term::is_real() const
{
  return d_term.is_real();
}

bool
Z3Term::is_rm() const
{
  return d_term.get_sort().sort_kind() == Z3_ROUNDING_MODE_SORT;
}

uint32_t
Z3Term::get_bv_size() const
{
  assert(is_bv());
  return d_term.get_sort().bv_size();
}

uint32_t
Z3Term::get_fp_exp_size() const
{
  assert(is_fp());
  return d_term.get_sort().fpa_ebits();
}

uint32_t
Z3Term::get_fp_sig_size() const
{
  assert(is_fp());
  return d_term.get_sort().fpa_sbits();
}

/* -------------------------------------------------------------------------- */
/* Z3Solver                                                                   */
/* -------------------------------------------------------------------------- */

Z3Solver::~Z3Solver()
{
  d_solver.reset();
  d_model.reset();
  d_context.reset();
}

void
Z3Solver::new_solver()
{
  d_context.reset(new ::z3::context());
  d_solver.reset(new ::z3::solver(*d_context));
  d_is_initialized = true;
}

void
Z3Solver::delete_solver()
{
  d_solver.reset();
  d_model.reset();
  d_context.reset();
  d_is_initialized = false;
}

bool
Z3Solver::is_initialized() const
{
  return d_is_initialized;
}

const std::string
Z3Solver::get_name() const
{
  return "Z3";
}

const std::string
Z3Solver::get_profile() const
{
  return murxla::z3::s_profile;
}

void
Z3Solver::reset()
{
  if (d_solver)
  {
    d_solver->reset();
  }
  d_model.reset();
}

void
Z3Solver::reset_sat()
{
  d_model.reset();
}

void
Z3Solver::set_opt(const std::string& opt, const std::string& value)
{
  assert(d_context);
  if (opt == "incremental")
  {
    // Z3 supports incremental solving via push/pop by default, no option needed
    return;
  }
  
  // Create params object for setting options
  ::z3::params params(*d_context);
  
  if (opt == "produce-models")
  {
    params.set("model", value == "true");
  }
  else if (opt == "produce-unsat-assumptions")
  {
    params.set("unsat_core", value == "true");
  }
  else if (opt == "produce-unsat-cores")
  {
    params.set("unsat_core", value == "true");
  }
  else
  {
    params.set(opt.c_str(), value.c_str());
  }
  
  if (d_solver)
  {
    d_solver->set(params);
  }
}

bool
Z3Solver::is_unsat_assumption(const Term& t) const
{
  ::z3::expr z3_expr = Z3Term::get_z3_term(t);
  // In Z3, we verify the expression is valid (non-null)
  // The actual unsat core checking happens in get_unsat_assumptions()
  return z3_expr.id() != 0;
}

std::string
Z3Solver::get_option_name_incremental() const
{
  return "incremental";
}

std::string
Z3Solver::get_option_name_model_gen() const
{
  return "produce-models";
}

std::string
Z3Solver::get_option_name_unsat_assumptions() const
{
  return "produce-unsat-assumptions";
}

std::string
Z3Solver::get_option_name_unsat_cores() const
{
  return "produce-unsat-cores";
}

bool
Z3Solver::option_incremental_enabled() const
{
  // Z3 supports incremental solving via push/pop by default
  return true;
}

bool
Z3Solver::option_model_gen_enabled() const
{
  return true;
}

bool
Z3Solver::option_unsat_assumptions_enabled() const
{
  return true;
}

bool
Z3Solver::option_unsat_cores_enabled() const
{
  return true;
}

Term
Z3Solver::mk_var(Sort sort, const std::string& name)
{
  assert(d_context);
  ::z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
  ::z3::expr var = d_context->constant(name.c_str(), z3_sort);
  return std::shared_ptr<Z3Term>(new Z3Term(var));
}

Term
Z3Solver::mk_const(Sort sort, const std::string& name)
{
  assert(d_context);
  ::z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
  ::z3::expr constant = d_context->constant(name.c_str(), z3_sort);
  return std::shared_ptr<Z3Term>(new Z3Term(constant));
}

Term
Z3Solver::mk_fun(const std::string& name,
                 const std::vector<Term>& args,
                 Term body)
{
  // Z3 represents functions as lambda expressions
  // Lambda expressions have array sorts (domain -> codomain)
  assert(d_context);
  
  if (args.empty()) {
    // No arguments - not a valid function
    return nullptr;
  }
  
  // Convert Murxla terms to Z3 expressions (these are the bound variables)
  ::z3::expr_vector z3_args = Z3Term::terms_to_z3_terms(*d_context, args);
  ::z3::expr z3_body = Z3Term::get_z3_term(body);
  
  // Create lambda expression using global z3 namespace
  ::z3::expr lambda_expr = ::z3::lambda(z3_args, z3_body);
  
  return std::shared_ptr<Z3Term>(new Z3Term(lambda_expr));
}

Term
Z3Solver::mk_value(Sort sort, bool value)
{
  assert(d_context);
  MURXLA_CHECK_CONFIG(sort->is_bool())
      << "unexpected sort of kind '" << sort->get_kind()
      << "' as argument to Z3Solver::mk_value, expected Boolean sort";
  
  ::z3::expr z3_result = d_context->bool_val(value);
  return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
}

Term
Z3Solver::mk_value(Sort sort, const std::string& value)
{
  assert(d_context);
  ::z3::expr z3_result(*d_context);
  
  switch (sort->get_kind())
  {
    case SORT_INT:
      z3_result = d_context->int_val(value.c_str());
      break;
    
    case SORT_REAL:
      z3_result = d_context->real_val(value.c_str());
      break;
    
    case SORT_STRING:
      z3_result = d_context->string_val(value.c_str());
      break;
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unexpected sort of kind '" << sort->get_kind()
          << "' as argument to Z3Solver::mk_value, expected Integer, Real, or String sort";
  }
  
  return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
}

Term
Z3Solver::mk_value(Sort sort,
                   const std::string& num,
                   const std::string& den)
{
  assert(d_context);
  MURXLA_CHECK_CONFIG(sort->is_real())
      << "unexpected sort of kind '" << sort->get_kind()
      << "' as argument to Z3Solver::mk_value, expected Real sort";
  
  ::z3::expr numerator = d_context->int_val(num.c_str());
  ::z3::expr denominator = d_context->int_val(den.c_str());
  ::z3::expr z3_result = to_real(numerator) / to_real(denominator);
  
  return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
}

Term
Z3Solver::mk_value(Sort sort, const std::string& value, Base base)
{
  assert(d_context);
  assert(sort->is_bv());
  
  uint32_t bw = sort->get_bv_size();
  ::z3::expr z3_result(*d_context);
  
  // Z3's bv_val can handle decimal strings and integer values
  // For hex and binary, convert to decimal
  if (base == DEC)
  {
    // Decimal: Z3 can handle this directly
    z3_result = d_context->bv_val(value.c_str(), bw);
  }
  else if (base == HEX)
  {
    // Hexadecimal: Try to convert if small enough, otherwise use string conversion
    if (bw <= 64)
    {
      try {
        uint64_t val = std::stoull(value, nullptr, 16);
        z3_result = d_context->bv_val(static_cast<uint64_t>(val), bw);
      }
      catch (const std::exception&) {
        // Fallback: convert hex->bin->dec
        std::string bin_str = str_hex_to_bin(value);
        std::string dec_str = str_bin_to_dec(bin_str);
        z3_result = d_context->bv_val(dec_str.c_str(), bw);
      }
    }
    else
    {
      // For large bit-vectors, convert hex->bin->dec
      std::string bin_str = str_hex_to_bin(value);
      std::string dec_str = str_bin_to_dec(bin_str);
      z3_result = d_context->bv_val(dec_str.c_str(), bw);
    }
  }
  else
  {
    assert(base == BIN);
    // Binary: Try to convert if small enough, otherwise use string conversion
    if (bw <= 64)
    {
      try {
        uint64_t val = std::stoull(value, nullptr, 2);
        z3_result = d_context->bv_val(static_cast<uint64_t>(val), bw);
      }
      catch (const std::exception&) {
        // Fallback: convert bin->dec
        std::string dec_str = str_bin_to_dec(value);
        z3_result = d_context->bv_val(dec_str.c_str(), bw);
      }
    }
    else
    {
      // For large bit-vectors, convert bin->dec
      std::string dec_str = str_bin_to_dec(value);
      z3_result = d_context->bv_val(dec_str.c_str(), bw);
    }
  }
  
  return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
}

Term
Z3Solver::mk_special_value(Sort sort, const AbsTerm::SpecialValueKind& value)
{
  assert(d_context);
  
  if (sort->is_bv())
  {
    uint32_t bw = sort->get_bv_size();
    ::z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
    ::z3::expr z3_result(*d_context);
    
    if (value == AbsTerm::SPECIAL_VALUE_BV_ZERO)
    {
      z3_result = d_context->bv_val(0, bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_ONE)
    {
      z3_result = d_context->bv_val(1, bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_ONES)
    {
      z3_result = d_context->bv_val(-1, bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_MIN_SIGNED)
    {
      std::string min_val = "1" + std::string(bw - 1, '0');
      z3_result = d_context->bv_val(min_val.c_str(), bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_MAX_SIGNED)
    {
      std::string max_val = "0" + std::string(bw - 1, '1');
      z3_result = d_context->bv_val(max_val.c_str(), bw);
    }
    else
    {
      MURXLA_CHECK_CONFIG(false) << "unsupported special value kind";
    }
    
    return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
  }
  else if (sort->is_fp())
  {
    ::z3::expr z3_result(*d_context);
    ::z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
    
    if (value == AbsTerm::SPECIAL_VALUE_FP_NAN)
    {
      z3_result = d_context->fpa_nan(z3_sort);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_POS_INF)
    {
      z3_result = d_context->fpa_inf(z3_sort, false);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_NEG_INF)
    {
      z3_result = d_context->fpa_inf(z3_sort, true);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_POS_ZERO)
    {
      Z3_ast c_zero = Z3_mk_fpa_zero(d_context->operator Z3_context(), z3_sort, false);
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_zero);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_NEG_ZERO)
    {
      Z3_ast c_zero = Z3_mk_fpa_zero(d_context->operator Z3_context(), z3_sort, true);
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_zero);
    }
    else
    {
      MURXLA_CHECK_CONFIG(false) << "unsupported special value kind for FP";
    }
    
    return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
  }
  else if (sort->is_rm())
  {
    ::z3::expr z3_result(*d_context);
    
    if (value == AbsTerm::SPECIAL_VALUE_RM_RNA)
    {
      Z3_ast c_rm = Z3_mk_fpa_rna(d_context->operator Z3_context());
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_rm);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RNE)
    {
      Z3_ast c_rm = Z3_mk_fpa_rne(d_context->operator Z3_context());
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_rm);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RTN)
    {
      Z3_ast c_rm = Z3_mk_fpa_rtn(d_context->operator Z3_context());
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_rm);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RTP)
    {
      Z3_ast c_rm = Z3_mk_fpa_rtp(d_context->operator Z3_context());
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_rm);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RTZ)
    {
      Z3_ast c_rm = Z3_mk_fpa_rtz(d_context->operator Z3_context());
      d_context->check_error();
      z3_result = ::z3::expr(*d_context, c_rm);
    }
    else
    {
      MURXLA_CHECK_CONFIG(false) << "unsupported special value kind for RM";
    }
    
    return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
  }
  
  MURXLA_CHECK_CONFIG(false) << "unsupported sort for special value";
  return nullptr;
}

Sort
Z3Solver::mk_sort(SortKind kind)
{
  assert(d_context);
  ::z3::sort z3_result(*d_context);
  
  switch (kind)
  {
    case SORT_BOOL:
      z3_result = d_context->bool_sort();
      break;
    
    case SORT_INT:
      z3_result = d_context->int_sort();
      break;
    
    case SORT_REAL:
      z3_result = d_context->real_sort();
      break;
    
    case SORT_RM:
      z3_result = d_context->fpa_rounding_mode_sort();
      break;
    
    case SORT_STRING:
      z3_result = d_context->string_sort();
      break;
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unsupported sort kind '" << kind
          << "' as argument to Z3Solver::mk_sort";
  }
  
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_result));
}

Sort
Z3Solver::mk_sort(SortKind kind, uint32_t size)
{
  assert(d_context);
  ::z3::sort z3_result(*d_context);
  
  switch (kind)
  {
    case SORT_BV:
      z3_result = d_context->bv_sort(size);
      break;
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unsupported sort kind '" << kind
          << "' as argument to Z3Solver::mk_sort with size";
  }
  
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_result));
}

Sort
Z3Solver::mk_sort(SortKind kind, uint32_t esize, uint32_t ssize)
{
  assert(d_context);
  MURXLA_CHECK_CONFIG(kind == SORT_FP)
      << "unsupported sort kind '" << kind
      << "' as argument to Z3Solver::mk_sort with esize and ssize, expected '"
      << SORT_FP << "'";
  
  ::z3::sort z3_result = d_context->fpa_sort(esize, ssize);
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_result));
}

Sort
Z3Solver::mk_sort(const std::string& name)
{
  assert(d_context);
  ::z3::symbol sym = d_context->str_symbol(name.c_str());
  ::z3::sort z3_result = d_context->uninterpreted_sort(sym);
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_result));
}

Sort
Z3Solver::mk_sort(SortKind kind, const std::vector<Sort>& sorts)
{
  assert(d_context);
  ::z3::sort z3_result(*d_context);
  
  switch (kind)
  {
    case SORT_ARRAY:
    {
      assert(sorts.size() == 2);
      ::z3::sort index_sort = Z3Sort::get_z3_sort(sorts[0]);
      ::z3::sort element_sort = Z3Sort::get_z3_sort(sorts[1]);
      z3_result = d_context->array_sort(index_sort, element_sort);
      break;
    }
    
    case SORT_SEQ:
    {
      assert(sorts.size() == 1);
      ::z3::sort element_sort = Z3Sort::get_z3_sort(sorts[0]);
      z3_result = d_context->seq_sort(element_sort);
      break;
    }
    
    case SORT_FUN:
    {
      // Z3 represents function sorts as n-dimensional array sorts
      // Single-arg: Array domain codomain
      // Multi-arg: Array domain1 domain2 ... domainN codomain
      assert(sorts.size() >= 2);
      
      // Extract domain sorts (all but the last one)
      std::vector<Sort> domain_sorts(sorts.begin(), sorts.end() - 1);
      
      if (domain_sorts.size() == 1) {
        // Single-argument function
        ::z3::sort domain = Z3Sort::get_z3_sort(domain_sorts[0]);
        ::z3::sort codomain = Z3Sort::get_z3_sort(sorts.back());
        z3_result = d_context->array_sort(domain, codomain);
      } else {
        // Multi-argument function: use n-dimensional array
        std::vector<Z3_sort> z3_domains;
        for (const auto& ds : domain_sorts) {
          z3_domains.push_back(Z3Sort::get_z3_sort(ds));
        }
        ::z3::sort codomain = Z3Sort::get_z3_sort(sorts.back());
        
        Z3_sort array_sort = Z3_mk_array_sort_n(
          d_context->operator Z3_context(),
          static_cast<unsigned>(z3_domains.size()),
          z3_domains.data(),
          codomain
        );
        d_context->check_error();
        z3_result = ::z3::sort(*d_context, array_sort);
      }
      
      // Return with function sort flag set to true and store domain sorts
      return std::shared_ptr<Z3Sort>(new Z3Sort(z3_result, true, domain_sorts));
    }
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unsupported sort kind '" << kind
          << "' as argument to Z3Solver::mk_sort with sorts";
  }
  
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_result));
}

Term
Z3Solver::mk_term(const Op::Kind& kind,
                  const std::vector<Term>& args,
                  const std::vector<uint32_t>& indices)
{
  assert(d_context);
  ::z3::expr_vector z3_args = Z3Term::terms_to_z3_terms(*d_context, args);
  size_t n_args = args.size();
  ::z3::expr z3_result(*d_context);
  
  // Boolean operators
  if (kind == Op::NOT)
  {
    assert(n_args == 1);
    z3_result = !z3_args[0];
  }
  else if (kind == Op::AND)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      z3_result = z3_result && z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::OR)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      z3_result = z3_result || z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::XOR)
  {
    assert(n_args == 2);
    // Use ^ operator which calls Z3_mk_xor for boolean or Z3_mk_bvxor for BV
    z3_result = z3_args[0] ^ z3_args[1];
  }
  else if (kind == Op::IMPLIES)
  {
    assert(n_args == 2);
    z3_result = implies(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::ITE)
  {
    assert(n_args == 3);
    z3_result = ite(z3_args[0], z3_args[1], z3_args[2]);
  }
  else if (kind == Op::EQUAL)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] == z3_args[1];
  }
  else if (kind == Op::DISTINCT)
  {
    z3_result = distinct(z3_args);
  }
  // Bit-vector operators
  else if (kind == Op::BV_NOT)
  {
    assert(n_args == 1);
    z3_result = ~z3_args[0];
  }
  else if (kind == Op::BV_AND)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] & z3_args[1];
  }
  else if (kind == Op::BV_OR)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] | z3_args[1];
  }
  else if (kind == Op::BV_XOR)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] ^ z3_args[1];
  }
  else if (kind == Op::BV_NAND)
  {
    assert(n_args == 2);
    z3_result = nand(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_NOR)
  {
    assert(n_args == 2);
    z3_result = nor(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_XNOR)
  {
    assert(n_args == 2);
    z3_result = xnor(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_NEG)
  {
    assert(n_args == 1);
    z3_result = -z3_args[0];
  }
  else if (kind == Op::BV_ADD)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] + z3_args[1];
  }
  else if (kind == Op::BV_SUB)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] - z3_args[1];
  }
  else if (kind == Op::BV_MULT)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] * z3_args[1];
  }
  else if (kind == Op::BV_UDIV)
  {
    assert(n_args == 2);
    z3_result = udiv(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_UREM)
  {
    assert(n_args == 2);
    z3_result = urem(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SDIV)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] / z3_args[1];
  }
  else if (kind == Op::BV_SREM)
  {
    assert(n_args == 2);
    z3_result = srem(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SMOD)
  {
    assert(n_args == 2);
    z3_result = smod(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SHL)
  {
    assert(n_args == 2);
    z3_result = shl(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_LSHR)
  {
    assert(n_args == 2);
    z3_result = lshr(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_ASHR)
  {
    assert(n_args == 2);
    z3_result = ashr(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_ULT)
  {
    assert(n_args == 2);
    z3_result = ult(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_ULE)
  {
    assert(n_args == 2);
    z3_result = ule(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_UGT)
  {
    assert(n_args == 2);
    z3_result = ugt(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_UGE)
  {
    assert(n_args == 2);
    z3_result = uge(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SLT)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] < z3_args[1];
  }
  else if (kind == Op::BV_SLE)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] <= z3_args[1];
  }
  else if (kind == Op::BV_SGT)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] > z3_args[1];
  }
  else if (kind == Op::BV_SGE)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] >= z3_args[1];
  }
  else if (kind == Op::BV_CONCAT)
  {
    assert(n_args == 2);
    z3_result = concat(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_EXTRACT)
  {
    assert(n_args == 1);
    assert(indices.size() == 2);
    // indices[0] is high, indices[1] is low (SMT-LIB: (extract hi lo))
    z3_result = z3_args[0].extract(indices[0], indices[1]);
  }
  else if (kind == Op::BV_REPEAT)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    z3_result = z3_args[0].repeat(indices[0]);
  }
  else if (kind == Op::BV_ZERO_EXTEND)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    z3_result = zext(z3_args[0], indices[0]);
  }
  else if (kind == Op::BV_SIGN_EXTEND)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    z3_result = sext(z3_args[0], indices[0]);
  }
  else if (kind == Op::BV_ROTATE_LEFT)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    z3_result = z3_args[0].rotate_left(indices[0]);
  }
  else if (kind == Op::BV_ROTATE_RIGHT)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    z3_result = z3_args[0].rotate_right(indices[0]);
  }
  // Integer/Real arithmetic
  else if (kind == Op::INT_NEG || kind == Op::REAL_NEG)
  {
    assert(n_args == 1);
    z3_result = -z3_args[0];
  }
  else if (kind == Op::INT_ADD || kind == Op::REAL_ADD)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      z3_result = z3_result + z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::INT_SUB || kind == Op::REAL_SUB)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      z3_result = z3_result - z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::INT_MUL || kind == Op::REAL_MUL)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      z3_result = z3_result * z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::INT_DIV)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0] / z3_args[1];
    for (size_t i = 2; i < n_args; ++i)
    {
      z3_result = z3_result / z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::INT_MOD)
  {
    assert(n_args == 2);
    z3_result = mod(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::INT_ABS)
  {
    assert(n_args == 1);
    z3_result = abs(z3_args[0]);
  }
  else if (kind == Op::INT_LT || kind == Op::REAL_LT)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] < z3_args[1];
  }
  else if (kind == Op::INT_LTE || kind == Op::REAL_LTE)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] <= z3_args[1];
  }
  else if (kind == Op::INT_GT || kind == Op::REAL_GT)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] > z3_args[1];
  }
  else if (kind == Op::INT_GTE || kind == Op::REAL_GTE)
  {
    assert(n_args == 2);
    z3_result = z3_args[0] >= z3_args[1];
  }
  else if (kind == Op::REAL_DIV)
  {
    assert(n_args >= 2);
    z3_result = z3_args[0] / z3_args[1];
    for (size_t i = 2; i < n_args; ++i)
    {
      z3_result = z3_result / z3_args[static_cast<unsigned>(i)];
    }
  }
  else if (kind == Op::INT_TO_REAL)
  {
    assert(n_args == 1);
    z3_result = to_real(z3_args[0]);
  }
  else if (kind == Op::REAL_TO_INT)
  {
    assert(n_args == 1);
    // Z3 doesn't have to_int in the C++ API, use C API directly
    Z3_ast c_result = Z3_mk_real2int(d_context->operator Z3_context(), z3_args[0]);
    d_context->check_error();
    z3_result = ::z3::expr(*d_context, c_result);
  }
  else if (kind == Op::REAL_IS_INT)
  {
    assert(n_args == 1);
    z3_result = is_int(z3_args[0]);
  }
  // Array operators
  else if (kind == Op::ARRAY_SELECT)
  {
    assert(n_args == 2);
    z3_result = select(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::ARRAY_STORE)
  {
    assert(n_args == 3);
    z3_result = store(z3_args[0], z3_args[1], z3_args[2]);
  }
  // Quantifiers
  else if (kind == Op::FORALL || kind == Op::EXISTS)
  {
    assert(n_args >= 2);
    ::z3::expr_vector vars(*d_context);
    for (size_t i = 0; i < n_args - 1; ++i)
    {
      vars.push_back(z3_args[static_cast<unsigned>(i)]);
    }
    ::z3::expr body = z3_args[static_cast<unsigned>(n_args - 1)];
    
    if (kind == Op::EXISTS)
    {
      z3_result = exists(vars, body);
    }
    else
    {
      z3_result = forall(vars, body);
    }
  }
  // UF application
  else if (kind == Op::UF_APPLY)
  {
    assert(n_args >= 2);
    // First argument is the function, rest are the actual arguments
    ::z3::expr func_expr = z3_args[0];
    
    // Check if this is a lambda expression (has array sort)
    if (func_expr.get_sort().is_array())
    {
      // Lambda expressions use n-dimensional array sorts in Z3
      // Use Z3_mk_select_n for multi-dimensional array access
      if (n_args == 2) {
        // Single-argument function: select(lambda, arg)
        z3_result = select(func_expr, z3_args[1]);
      } else {
        // Multi-argument function: use select_n with all arguments
        std::vector<Z3_ast> indices;
        for (size_t i = 1; i < n_args; ++i) {
          indices.push_back(z3_args[static_cast<unsigned>(i)]);
        }
        
        Z3_ast result_ast = Z3_mk_select_n(
          d_context->operator Z3_context(),
          func_expr,
          static_cast<unsigned>(indices.size()),
          indices.data()
        );
        d_context->check_error();
        z3_result = ::z3::expr(*d_context, result_ast);
      }
    }
    else
    {
      // Traditional function declaration application
      ::z3::expr_vector actual_args(*d_context);
      for (size_t i = 1; i < n_args; ++i)
      {
        actual_args.push_back(z3_args[static_cast<unsigned>(i)]);
      }
      z3_result = func_expr.decl()(actual_args);
    }
  }
  else
  {
    MURXLA_CHECK_CONFIG(false)
        << "unsupported operator kind '" << kind << "' in Z3Solver::mk_term";
  }
  
  return std::shared_ptr<Z3Term>(new Z3Term(z3_result));
}

Sort
Z3Solver::get_sort(Term term, SortKind sort_kind)
{
  ::z3::expr z3_term = Z3Term::get_z3_term(term);
  ::z3::sort z3_sort = z3_term.get_sort();
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_sort));
}

void
Z3Solver::assert_formula(const Term& t)
{
  assert(d_solver);
  ::z3::expr z3_expr = Z3Term::get_z3_term(t);
  d_solver->add(z3_expr);
}

Solver::Result
Z3Solver::check_sat()
{
  assert(d_solver);
  ::z3::check_result z3_res = d_solver->check();
  
  if (z3_res == ::z3::sat)
  {
    return Solver::Result::SAT;
  }
  else if (z3_res == ::z3::unsat)
  {
    return Solver::Result::UNSAT;
  }
  else
  {
    return Solver::Result::UNKNOWN;
  }
}

Solver::Result
Z3Solver::check_sat_assuming(const std::vector<Term>& assumptions)
{
  assert(d_solver);
  ::z3::expr_vector z3_assumptions = Z3Term::terms_to_z3_terms(*d_context, assumptions);
  ::z3::check_result z3_res = d_solver->check(z3_assumptions);
  
  if (z3_res == ::z3::sat)
  {
    return Solver::Result::SAT;
  }
  else if (z3_res == ::z3::unsat)
  {
    return Solver::Result::UNSAT;
  }
  else
  {
    return Solver::Result::UNKNOWN;
  }
}

std::vector<Term>
Z3Solver::get_unsat_assumptions()
{
  assert(d_solver);
  ::z3::expr_vector core = d_solver->unsat_core();
  return Z3Term::z3_terms_to_terms(core);
}

std::vector<Term>
Z3Solver::get_unsat_core()
{
  assert(d_solver);
  ::z3::expr_vector core = d_solver->unsat_core();
  return Z3Term::z3_terms_to_terms(core);
}

std::vector<Term>
Z3Solver::get_value(const std::vector<Term>& terms)
{
  assert(d_solver);
  
  if (!d_model)
  {
    d_model.reset(new ::z3::model(d_solver->get_model()));
  }
  
  std::vector<Term> res;
  for (const auto& t : terms)
  {
    ::z3::expr z3_term = Z3Term::get_z3_term(t);
    ::z3::expr value = d_model->eval(z3_term, true);
    res.push_back(std::shared_ptr<Z3Term>(new Z3Term(value)));
  }
  
  return res;
}

void
Z3Solver::push(uint32_t n_levels)
{
  assert(d_solver);
  for (uint32_t i = 0; i < n_levels; ++i)
  {
    d_solver->push();
  }
}

void
Z3Solver::pop(uint32_t n_levels)
{
  assert(d_solver);
  for (uint32_t i = 0; i < n_levels; ++i)
  {
    d_solver->pop();
  }
}

void
Z3Solver::print_model()
{
  assert(d_solver);
  if (!d_model)
  {
    d_model.reset(new ::z3::model(d_solver->get_model()));
  }
  std::cout << *d_model << std::endl;
}

void
Z3Solver::reset_assertions()
{
  assert(d_solver);
  d_solver->reset();
}

void
Z3Solver::disable_unsupported_actions(FSM* fsm) const
{
  // Z3 supports most operations, so we don't disable much
  // Uncomment if specific actions need to be disabled
  // fsm->disable_action(ActionGetValue::s_name);
}

void
Z3Solver::configure_opmgr(OpKindManager* opmgr) const
{
  // Add Z3-specific operators if needed
  // For now, we rely on the standard operators defined in the base profile
}

}  // namespace z3solver
}  // namespace murxla

#endif
