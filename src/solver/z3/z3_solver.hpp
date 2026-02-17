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

#ifndef __MURXLA__Z3_SOLVER_H
#define __MURXLA__Z3_SOLVER_H

#include <z3++.h>

#include "fsm.hpp"
#include "solver/solver.hpp"
#include "theory.hpp"

namespace murxla {
namespace z3solver {

/* -------------------------------------------------------------------------- */
/* Z3Sort */
/* -------------------------------------------------------------------------- */

class Z3Sort : public AbsSort
{
  friend class Z3Solver;

 public:
  /** Get wrapped Z3 sort from Murxla sort. */
  static z3::sort get_z3_sort(Sort sort);
  /** Convert vector of Murxla sorts to Z3 sorts. */
  static z3::sort_vector sorts_to_z3_sorts(z3::context& ctx,
                                            const std::vector<Sort>& sorts);

  Z3Sort(z3::sort sort) : d_sort(sort) {}
  ~Z3Sort() override {}
  size_t hash() const override;
  bool equals(const Sort& other) const override;
  std::string to_string() const override;
  bool is_array() const override;
  bool is_bool() const override;
  bool is_bv() const override;
  bool is_dt() const override;
  bool is_fp() const override;
  bool is_fun() const override;
  bool is_int() const override;
  bool is_real() const override;
  bool is_rm() const override;
  bool is_uninterpreted() const override;
  uint32_t get_bv_size() const override;
  uint32_t get_fp_exp_size() const override;
  uint32_t get_fp_sig_size() const override;
  std::string get_dt_name() const override;
  Sort get_array_index_sort() const override;
  Sort get_array_element_sort() const override;
  uint32_t get_fun_arity() const override;
  Sort get_fun_codomain_sort() const override;
  std::vector<Sort> get_fun_domain_sorts() const override;

 private:
  z3::sort d_sort;
};

/* -------------------------------------------------------------------------- */
/* Z3Term */
/* -------------------------------------------------------------------------- */

class Z3Term : public AbsTerm
{
  friend class Z3Solver;

 public:
  /** Get wrapped Z3 term from Murxla term. */
  static z3::expr get_z3_term(Term term);
  /** Convert vector of Z3 terms to vector Murxla terms. */
  static std::vector<Term> z3_terms_to_terms(const z3::expr_vector& terms);
  /** Convert vector of Murxla terms to vector Z3 terms. */
  static z3::expr_vector terms_to_z3_terms(z3::context& ctx,
                                            const std::vector<Term>& terms);

  Z3Term(z3::expr term) : d_term(term) {}
  ~Z3Term() override {}
  size_t hash() const override;
  std::string to_string() const override;
  bool equals(const Term& other) const override;
  bool is_array() const override;
  bool is_bool() const override;
  bool is_bv() const override;
  bool is_fp() const override;
  bool is_fun() const override;
  bool is_int() const override;
  bool is_real() const override;
  bool is_rm() const override;

  uint32_t get_bv_size() const override;
  uint32_t get_fp_exp_size() const override;
  uint32_t get_fp_sig_size() const override;

 private:
  z3::expr d_term;
};

/* -------------------------------------------------------------------------- */
/* Z3Solver */
/* -------------------------------------------------------------------------- */

class Z3Solver : public Solver
{
 public:
  Z3Solver(SolverSeedGenerator& sng) : Solver(sng) {}
  ~Z3Solver() override;

  void new_solver() override;
  void delete_solver() override;
  bool is_initialized() const override;
  const std::string get_name() const override;
  const std::string get_profile() const override;

  void disable_unsupported_actions(FSM* fsm) const override;
  void configure_opmgr(OpKindManager* opmgr) const override;
  void reset() override;
  void reset_sat() override;

  void set_opt(const std::string& opt, const std::string& value) override;

  bool is_unsat_assumption(const Term& t) const override;

  std::string get_option_name_incremental() const override;
  std::string get_option_name_model_gen() const override;
  std::string get_option_name_unsat_assumptions() const override;
  std::string get_option_name_unsat_cores() const override;

  bool option_incremental_enabled() const override;
  bool option_model_gen_enabled() const override;
  bool option_unsat_assumptions_enabled() const override;
  bool option_unsat_cores_enabled() const override;

  Term mk_var(Sort sort, const std::string& name) override;

  Term mk_const(Sort sort, const std::string& name) override;

  Term mk_fun(const std::string& name,
              const std::vector<Term>& args,
              Term body) override;

  Term mk_value(Sort sort, bool value) override;
  Term mk_value(Sort sort, const std::string& value) override;
  Term mk_value(Sort sort,
                const std::string& num,
                const std::string& den) override;
  Term mk_value(Sort sort, const std::string& value, Base base) override;

  Term mk_special_value(Sort sort,
                        const AbsTerm::SpecialValueKind& value) override;

  Sort mk_sort(SortKind kind) override;
  Sort mk_sort(SortKind kind, uint32_t size) override;
  Sort mk_sort(SortKind kind, uint32_t esize, uint32_t ssize) override;
  Sort mk_sort(const std::string& name) override;

  Sort mk_sort(SortKind kind, const std::vector<Sort>& sorts) override;

  Term mk_term(const Op::Kind& kind,
               const std::vector<Term>& args,
               const std::vector<uint32_t>& indices) override;

  Sort get_sort(Term term, SortKind sort_kind) override;

  void assert_formula(const Term& t) override;

  Result check_sat() override;
  Result check_sat_assuming(const std::vector<Term>& assumptions) override;

  std::vector<Term> get_unsat_assumptions() override;

  std::vector<Term> get_unsat_core() override;

  std::vector<Term> get_value(const std::vector<Term>& terms) override;

  void push(uint32_t n_levels) override;
  void pop(uint32_t n_levels) override;

  void print_model() override;

  void reset_assertions() override;

 private:
  bool d_is_initialized = false;
  std::unique_ptr<z3::context> d_context;
  std::unique_ptr<z3::solver> d_solver;
  std::unique_ptr<z3::model> d_model;
  z3::params d_params;
};

}  // namespace z3solver
}  // namespace murxla

#endif
#endif
