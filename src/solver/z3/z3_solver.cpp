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

namespace murxla {
namespace z3solver {

/* -------------------------------------------------------------------------- */
/* Z3Sort                                                                     */
/* -------------------------------------------------------------------------- */

z3::sort
Z3Sort::get_z3_sort(Sort sort)
{
  Z3Sort* z3_sort = checked_cast<Z3Sort*>(sort.get());
  assert(z3_sort);
  return z3_sort->d_sort;
}

z3::sort_vector
Z3Sort::sorts_to_z3_sorts(z3::context& ctx, const std::vector<Sort>& sorts)
{
  z3::sort_vector res(ctx);
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
    return z3::eq(d_sort, z3_sort->d_sort);
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
  return d_sort.is_array();
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
  return d_sort.is_function();
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
  return d_sort.is_uninterpreted();
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
  return d_sort.function_arity();
}

Sort
Z3Sort::get_fun_codomain_sort() const
{
  assert(is_fun());
  return std::shared_ptr<Z3Sort>(new Z3Sort(d_sort.function_range()));
}

std::vector<Sort>
Z3Sort::get_fun_domain_sorts() const
{
  assert(is_fun());
  std::vector<Sort> res;
  uint32_t arity = d_sort.function_arity();
  for (uint32_t i = 0; i < arity; ++i)
  {
    res.push_back(std::shared_ptr<Z3Sort>(new Z3Sort(d_sort.function_domain(i))));
  }
  return res;
}

/* -------------------------------------------------------------------------- */
/* Z3Term                                                                     */
/* -------------------------------------------------------------------------- */

z3::expr
Z3Term::get_z3_term(Term term)
{
  Z3Term* z3_term = checked_cast<Z3Term*>(term.get());
  assert(z3_term);
  return z3_term->d_term;
}

std::vector<Term>
Z3Term::z3_terms_to_terms(const z3::expr_vector& terms)
{
  std::vector<Term> res;
  for (uint32_t i = 0; i < terms.size(); ++i)
  {
    res.push_back(std::shared_ptr<Z3Term>(new Z3Term(terms[i])));
  }
  return res;
}

z3::expr_vector
Z3Term::terms_to_z3_terms(z3::context& ctx, const std::vector<Term>& terms)
{
  z3::expr_vector res(ctx);
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
    return z3::eq(d_term, z3_term->d_term);
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
  return d_term.get_sort().is_function();
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
  d_context.reset(new z3::context());
  d_params = z3::params(*d_context);
  d_solver.reset(new z3::solver(*d_context));
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
  return s_profile;
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
    // Z3 is incremental by default
    return;
  }
  else if (opt == "produce-models")
  {
    d_params.set("model", value == "true");
  }
  else if (opt == "produce-unsat-assumptions")
  {
    d_params.set("unsat_core", value == "true");
  }
  else if (opt == "produce-unsat-cores")
  {
    d_params.set("unsat_core", value == "true");
  }
  else
  {
    d_params.set(opt.c_str(), value.c_str());
  }
  
  if (d_solver)
  {
    d_solver->set(d_params);
  }
}

bool
Z3Solver::is_unsat_assumption(const Term& t) const
{
  z3::expr z3_expr = Z3Term::get_z3_term(t);
  return true;
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
  return true;  // Z3 is always incremental
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
  z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
  z3::expr var = d_context->constant(name.c_str(), z3_sort);
  return std::shared_ptr<Z3Term>(new Z3Term(var));
}

Term
Z3Solver::mk_const(Sort sort, const std::string& name)
{
  assert(d_context);
  z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
  z3::expr constant = d_context->constant(name.c_str(), z3_sort);
  return std::shared_ptr<Z3Term>(new Z3Term(constant));
}

Term
Z3Solver::mk_fun(const std::string& name,
                 const std::vector<Term>& args,
                 Term body)
{
  assert(d_context);
  z3::expr_vector z3_args = Z3Term::terms_to_z3_terms(*d_context, args);
  z3::expr z3_body = Z3Term::get_z3_term(body);
  
  z3::sort_vector domain(*d_context);
  for (const auto& arg : args)
  {
    domain.push_back(Z3Term::get_z3_term(arg).get_sort());
  }
  
  z3::func_decl fun_decl = d_context->function(name.c_str(), domain, z3_body.get_sort());
  z3::expr result = fun_decl(z3_args);
  
  return std::shared_ptr<Z3Term>(new Z3Term(result));
}

Term
Z3Solver::mk_value(Sort sort, bool value)
{
  assert(d_context);
  MURXLA_CHECK_CONFIG(sort->is_bool())
      << "unexpected sort of kind '" << sort->get_kind()
      << "' as argument to Z3Solver::mk_value, expected Boolean sort";
  
  z3::expr result = d_context->bool_val(value);
  return std::shared_ptr<Z3Term>(new Z3Term(result));
}

Term
Z3Solver::mk_value(Sort sort, const std::string& value)
{
  assert(d_context);
  z3::expr result(*d_context);
  
  switch (sort->get_kind())
  {
    case SORT_INT:
      result = d_context->int_val(value.c_str());
      break;
    
    case SORT_REAL:
      result = d_context->real_val(value.c_str());
      break;
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unexpected sort of kind '" << sort->get_kind()
          << "' as argument to Z3Solver::mk_value, expected Integer or Real sort";
  }
  
  return std::shared_ptr<Z3Term>(new Z3Term(result));
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
  
  z3::expr numerator = d_context->int_val(num.c_str());
  z3::expr denominator = d_context->int_val(den.c_str());
  z3::expr result = to_real(numerator) / to_real(denominator);
  
  return std::shared_ptr<Z3Term>(new Z3Term(result));
}

Term
Z3Solver::mk_value(Sort sort, const std::string& value, Base base)
{
  assert(d_context);
  assert(sort->is_bv());
  
  uint32_t bw = sort->get_bv_size();
  z3::expr result(*d_context);
  
  if (base == DEC)
  {
    result = d_context->bv_val(value.c_str(), bw);
  }
  else if (base == HEX)
  {
    result = d_context->bv_val(value.c_str(), bw);
  }
  else
  {
    assert(base == BIN);
    result = d_context->bv_val(value.c_str(), bw);
  }
  
  return std::shared_ptr<Z3Term>(new Z3Term(result));
}

Term
Z3Solver::mk_special_value(Sort sort, const AbsTerm::SpecialValueKind& value)
{
  assert(d_context);
  
  if (sort->is_bv())
  {
    uint32_t bw = sort->get_bv_size();
    z3::sort z3_sort = Z3Sort::get_z3_sort(sort);
    z3::expr result(*d_context);
    
    if (value == AbsTerm::SPECIAL_VALUE_BV_ZERO)
    {
      result = d_context->bv_val(0, bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_ONE)
    {
      result = d_context->bv_val(1, bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_ONES)
    {
      result = d_context->bv_val(-1, bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_MIN_SIGNED)
    {
      std::string min_val = "1" + std::string(bw - 1, '0');
      result = d_context->bv_val(min_val.c_str(), bw);
    }
    else if (value == AbsTerm::SPECIAL_VALUE_BV_MAX_SIGNED)
    {
      std::string max_val = "0" + std::string(bw - 1, '1');
      result = d_context->bv_val(max_val.c_str(), bw);
    }
    else
    {
      MURXLA_CHECK_CONFIG(false) << "unsupported special value kind";
    }
    
    return std::shared_ptr<Z3Term>(new Z3Term(result));
  }
  else if (sort->is_fp())
  {
    z3::expr result(*d_context);
    
    if (value == AbsTerm::SPECIAL_VALUE_FP_NAN)
    {
      result = d_context->fpa_nan(Z3Sort::get_z3_sort(sort));
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_POS_INF)
    {
      result = d_context->fpa_pinf(Z3Sort::get_z3_sort(sort));
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_NEG_INF)
    {
      result = d_context->fpa_ninf(Z3Sort::get_z3_sort(sort));
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_POS_ZERO)
    {
      result = d_context->fpa_pzero(Z3Sort::get_z3_sort(sort));
    }
    else if (value == AbsTerm::SPECIAL_VALUE_FP_NEG_ZERO)
    {
      result = d_context->fpa_nzero(Z3Sort::get_z3_sort(sort));
    }
    else
    {
      MURXLA_CHECK_CONFIG(false) << "unsupported special value kind for FP";
    }
    
    return std::shared_ptr<Z3Term>(new Z3Term(result));
  }
  else if (sort->is_rm())
  {
    z3::expr result(*d_context);
    
    if (value == AbsTerm::SPECIAL_VALUE_RM_RNA)
    {
      result = d_context->fpa_rna();
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RNE)
    {
      result = d_context->fpa_rne();
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RTN)
    {
      result = d_context->fpa_rtn();
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RTP)
    {
      result = d_context->fpa_rtp();
    }
    else if (value == AbsTerm::SPECIAL_VALUE_RM_RTZ)
    {
      result = d_context->fpa_rtz();
    }
    else
    {
      MURXLA_CHECK_CONFIG(false) << "unsupported special value kind for RM";
    }
    
    return std::shared_ptr<Z3Term>(new Z3Term(result));
  }
  
  MURXLA_CHECK_CONFIG(false) << "unsupported sort for special value";
  return nullptr;
}

Sort
Z3Solver::mk_sort(SortKind kind)
{
  assert(d_context);
  z3::sort result(*d_context);
  
  switch (kind)
  {
    case SORT_BOOL:
      result = d_context->bool_sort();
      break;
    
    case SORT_INT:
      result = d_context->int_sort();
      break;
    
    case SORT_REAL:
      result = d_context->real_sort();
      break;
    
    case SORT_RM:
      result = d_context->fpa_rounding_mode_sort();
      break;
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unsupported sort kind '" << kind
          << "' as argument to Z3Solver::mk_sort";
  }
  
  return std::shared_ptr<Z3Sort>(new Z3Sort(result));
}

Sort
Z3Solver::mk_sort(SortKind kind, uint32_t size)
{
  assert(d_context);
  z3::sort result(*d_context);
  
  switch (kind)
  {
    case SORT_BV:
      result = d_context->bv_sort(size);
      break;
    
    case SORT_FP:
      // For FP, size encodes both exponent and significand
      // This is a simplified version; actual encoding may differ
      result = d_context->fpa_sort(size / 2, size / 2);
      break;
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unsupported sort kind '" << kind
          << "' as argument to Z3Solver::mk_sort with size";
  }
  
  return std::shared_ptr<Z3Sort>(new Z3Sort(result));
}

Sort
Z3Solver::mk_sort(const std::string& name)
{
  assert(d_context);
  z3::symbol sym = d_context->str_symbol(name.c_str());
  z3::sort result = d_context->uninterpreted_sort(sym);
  return std::shared_ptr<Z3Sort>(new Z3Sort(result));
}

Sort
Z3Solver::mk_sort(SortKind kind, const std::vector<Sort>& sorts)
{
  assert(d_context);
  z3::sort result(*d_context);
  
  switch (kind)
  {
    case SORT_ARRAY:
    {
      assert(sorts.size() == 2);
      z3::sort index_sort = Z3Sort::get_z3_sort(sorts[0]);
      z3::sort element_sort = Z3Sort::get_z3_sort(sorts[1]);
      result = d_context->array_sort(index_sort, element_sort);
      break;
    }
    
    case SORT_FUN:
    {
      assert(sorts.size() >= 2);
      z3::sort_vector domain = Z3Sort::sorts_to_z3_sorts(*d_context, 
                                                         std::vector<Sort>(sorts.begin(), sorts.end() - 1));
      z3::sort range = Z3Sort::get_z3_sort(sorts.back());
      result = d_context->function_sort(domain, range);
      break;
    }
    
    default:
      MURXLA_CHECK_CONFIG(false)
          << "unsupported sort kind '" << kind
          << "' as argument to Z3Solver::mk_sort with sorts";
  }
  
  return std::shared_ptr<Z3Sort>(new Z3Sort(result));
}

Term
Z3Solver::mk_term(const Op::Kind& kind,
                  const std::vector<Term>& args,
                  const std::vector<uint32_t>& indices)
{
  assert(d_context);
  z3::expr_vector z3_args = Z3Term::terms_to_z3_terms(*d_context, args);
  size_t n_args = args.size();
  z3::expr result(*d_context);
  
  // Boolean operators
  if (kind == Op::NOT)
  {
    assert(n_args == 1);
    result = !z3_args[0];
  }
  else if (kind == Op::AND)
  {
    assert(n_args >= 2);
    result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      result = result && z3_args[i];
    }
  }
  else if (kind == Op::OR)
  {
    assert(n_args >= 2);
    result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      result = result || z3_args[i];
    }
  }
  else if (kind == Op::XOR)
  {
    assert(n_args == 2);
    result = z3::xor_(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::IMPLIES)
  {
    assert(n_args == 2);
    result = z3::implies(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::ITE)
  {
    assert(n_args == 3);
    result = z3::ite(z3_args[0], z3_args[1], z3_args[2]);
  }
  else if (kind == Op::EQUAL)
  {
    assert(n_args == 2);
    result = z3_args[0] == z3_args[1];
  }
  else if (kind == Op::DISTINCT)
  {
    result = z3::distinct(z3_args);
  }
  // Bit-vector operators
  else if (kind == Op::BV_NOT)
  {
    assert(n_args == 1);
    result = ~z3_args[0];
  }
  else if (kind == Op::BV_AND)
  {
    assert(n_args == 2);
    result = z3_args[0] & z3_args[1];
  }
  else if (kind == Op::BV_OR)
  {
    assert(n_args == 2);
    result = z3_args[0] | z3_args[1];
  }
  else if (kind == Op::BV_XOR)
  {
    assert(n_args == 2);
    result = z3_args[0] ^ z3_args[1];
  }
  else if (kind == Op::BV_NAND)
  {
    assert(n_args == 2);
    result = z3::nand(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_NOR)
  {
    assert(n_args == 2);
    result = z3::nor(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_XNOR)
  {
    assert(n_args == 2);
    result = z3::xnor(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_NEG)
  {
    assert(n_args == 1);
    result = -z3_args[0];
  }
  else if (kind == Op::BV_ADD)
  {
    assert(n_args == 2);
    result = z3_args[0] + z3_args[1];
  }
  else if (kind == Op::BV_SUB)
  {
    assert(n_args == 2);
    result = z3_args[0] - z3_args[1];
  }
  else if (kind == Op::BV_MUL)
  {
    assert(n_args == 2);
    result = z3_args[0] * z3_args[1];
  }
  else if (kind == Op::BV_UDIV)
  {
    assert(n_args == 2);
    result = z3::udiv(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_UREM)
  {
    assert(n_args == 2);
    result = z3::urem(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SDIV)
  {
    assert(n_args == 2);
    result = z3_args[0] / z3_args[1];
  }
  else if (kind == Op::BV_SREM)
  {
    assert(n_args == 2);
    result = z3::srem(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SMOD)
  {
    assert(n_args == 2);
    result = z3::smod(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SHL)
  {
    assert(n_args == 2);
    result = z3::shl(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_LSHR)
  {
    assert(n_args == 2);
    result = z3::lshr(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_ASHR)
  {
    assert(n_args == 2);
    result = z3::ashr(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_ULT)
  {
    assert(n_args == 2);
    result = z3::ult(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_ULE)
  {
    assert(n_args == 2);
    result = z3::ule(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_UGT)
  {
    assert(n_args == 2);
    result = z3::ugt(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_UGE)
  {
    assert(n_args == 2);
    result = z3::uge(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_SLT)
  {
    assert(n_args == 2);
    result = z3_args[0] < z3_args[1];
  }
  else if (kind == Op::BV_SLE)
  {
    assert(n_args == 2);
    result = z3_args[0] <= z3_args[1];
  }
  else if (kind == Op::BV_SGT)
  {
    assert(n_args == 2);
    result = z3_args[0] > z3_args[1];
  }
  else if (kind == Op::BV_SGE)
  {
    assert(n_args == 2);
    result = z3_args[0] >= z3_args[1];
  }
  else if (kind == Op::BV_CONCAT)
  {
    assert(n_args == 2);
    result = z3::concat(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::BV_EXTRACT)
  {
    assert(n_args == 1);
    assert(indices.size() == 2);
    result = z3_args[0].extract(indices[0], indices[1]);
  }
  else if (kind == Op::BV_REPEAT)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    result = z3_args[0].repeat(indices[0]);
  }
  else if (kind == Op::BV_ZERO_EXTEND)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    result = z3::zext(z3_args[0], indices[0]);
  }
  else if (kind == Op::BV_SIGN_EXTEND)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    result = z3::sext(z3_args[0], indices[0]);
  }
  else if (kind == Op::BV_ROTATE_LEFT)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    result = z3::rotate_left(z3_args[0], indices[0]);
  }
  else if (kind == Op::BV_ROTATE_RIGHT)
  {
    assert(n_args == 1);
    assert(indices.size() == 1);
    result = z3::rotate_right(z3_args[0], indices[0]);
  }
  // Integer/Real arithmetic
  else if (kind == Op::INT_NEG || kind == Op::REAL_NEG)
  {
    assert(n_args == 1);
    result = -z3_args[0];
  }
  else if (kind == Op::INT_ADD || kind == Op::REAL_ADD)
  {
    assert(n_args >= 2);
    result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      result = result + z3_args[i];
    }
  }
  else if (kind == Op::INT_SUB || kind == Op::REAL_SUB)
  {
    assert(n_args >= 2);
    result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      result = result - z3_args[i];
    }
  }
  else if (kind == Op::INT_MUL || kind == Op::REAL_MUL)
  {
    assert(n_args >= 2);
    result = z3_args[0];
    for (size_t i = 1; i < n_args; ++i)
    {
      result = result * z3_args[i];
    }
  }
  else if (kind == Op::INT_DIV)
  {
    assert(n_args == 2);
    result = z3_args[0] / z3_args[1];
  }
  else if (kind == Op::INT_MOD)
  {
    assert(n_args == 2);
    result = z3::mod(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::INT_ABS)
  {
    assert(n_args == 1);
    result = z3::abs(z3_args[0]);
  }
  else if (kind == Op::INT_LT || kind == Op::REAL_LT)
  {
    assert(n_args == 2);
    result = z3_args[0] < z3_args[1];
  }
  else if (kind == Op::INT_LE || kind == Op::REAL_LE)
  {
    assert(n_args == 2);
    result = z3_args[0] <= z3_args[1];
  }
  else if (kind == Op::INT_GT || kind == Op::REAL_GT)
  {
    assert(n_args == 2);
    result = z3_args[0] > z3_args[1];
  }
  else if (kind == Op::INT_GE || kind == Op::REAL_GE)
  {
    assert(n_args == 2);
    result = z3_args[0] >= z3_args[1];
  }
  else if (kind == Op::REAL_DIV)
  {
    assert(n_args == 2);
    result = z3_args[0] / z3_args[1];
  }
  else if (kind == Op::INT_TO_REAL)
  {
    assert(n_args == 1);
    result = z3::to_real(z3_args[0]);
  }
  else if (kind == Op::REAL_TO_INT)
  {
    assert(n_args == 1);
    result = z3::to_int(z3_args[0]);
  }
  else if (kind == Op::REAL_IS_INT)
  {
    assert(n_args == 1);
    result = z3::is_int(z3_args[0]);
  }
  // Array operators
  else if (kind == Op::ARRAY_SELECT)
  {
    assert(n_args == 2);
    result = z3::select(z3_args[0], z3_args[1]);
  }
  else if (kind == Op::ARRAY_STORE)
  {
    assert(n_args == 3);
    result = z3::store(z3_args[0], z3_args[1], z3_args[2]);
  }
  // Quantifiers
  else if (kind == Op::FORALL || kind == Op::EXISTS)
  {
    assert(n_args >= 2);
    z3::expr_vector vars(*d_context);
    for (size_t i = 0; i < n_args - 1; ++i)
    {
      vars.push_back(z3_args[i]);
    }
    z3::expr body = z3_args[n_args - 1];
    
    if (kind == Op::EXISTS)
    {
      result = z3::exists(vars, body);
    }
    else
    {
      result = z3::forall(vars, body);
    }
  }
  // UF application
  else if (kind == Op::UF_APPLY)
  {
    assert(n_args >= 2);
    z3::func_decl func = z3_args[0].decl();
    z3::expr_vector actual_args(*d_context);
    for (size_t i = 1; i < n_args; ++i)
    {
      actual_args.push_back(z3_args[i]);
    }
    result = func(actual_args);
  }
  else
  {
    MURXLA_CHECK_CONFIG(false)
        << "unsupported operator kind '" << kind << "' in Z3Solver::mk_term";
  }
  
  return std::shared_ptr<Z3Term>(new Z3Term(result));
}

Sort
Z3Solver::get_sort(Term term, SortKind sort_kind)
{
  z3::expr z3_term = Z3Term::get_z3_term(term);
  z3::sort z3_sort = z3_term.get_sort();
  return std::shared_ptr<Z3Sort>(new Z3Sort(z3_sort));
}

void
Z3Solver::assert_formula(const Term& t)
{
  assert(d_solver);
  z3::expr z3_expr = Z3Term::get_z3_term(t);
  d_solver->add(z3_expr);
}

Result
Z3Solver::check_sat()
{
  assert(d_solver);
  z3::check_result res = d_solver->check();
  
  if (res == z3::sat)
  {
    return Result::SAT;
  }
  else if (res == z3::unsat)
  {
    return Result::UNSAT;
  }
  else
  {
    return Result::UNKNOWN;
  }
}

Result
Z3Solver::check_sat_assuming(const std::vector<Term>& assumptions)
{
  assert(d_solver);
  z3::expr_vector z3_assumptions = Z3Term::terms_to_z3_terms(*d_context, assumptions);
  z3::check_result res = d_solver->check(z3_assumptions);
  
  if (res == z3::sat)
  {
    return Result::SAT;
  }
  else if (res == z3::unsat)
  {
    return Result::UNSAT;
  }
  else
  {
    return Result::UNKNOWN;
  }
}

std::vector<Term>
Z3Solver::get_unsat_assumptions()
{
  assert(d_solver);
  z3::expr_vector core = d_solver->unsat_core();
  return Z3Term::z3_terms_to_terms(core);
}

std::vector<Term>
Z3Solver::get_unsat_core()
{
  assert(d_solver);
  z3::expr_vector core = d_solver->unsat_core();
  return Z3Term::z3_terms_to_terms(core);
}

std::vector<Term>
Z3Solver::get_value(const std::vector<Term>& terms)
{
  assert(d_solver);
  
  if (!d_model)
  {
    d_model.reset(new z3::model(d_solver->get_model()));
  }
  
  std::vector<Term> res;
  for (const auto& t : terms)
  {
    z3::expr z3_term = Z3Term::get_z3_term(t);
    z3::expr value = d_model->eval(z3_term, true);
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
    d_model.reset(new z3::model(d_solver->get_model()));
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
