#ifdef MURXLA_USE_Z3

#include "gtest/gtest.h"
#include <z3++.h>

// Simple test using Z3's C++ API directly to verify Z3 integration
class Z3BasicTest : public ::testing::Test
{
 protected:
  void SetUp() override
  {
    ctx = std::make_unique<z3::context>();
    solver = std::make_unique<z3::solver>(*ctx);
  }

  void TearDown() override
  {
    solver.reset();
    ctx.reset();
  }

  std::unique_ptr<z3::context> ctx;
  std::unique_ptr<z3::solver> solver;
};

TEST_F(Z3BasicTest, basic_context_creation)
{
  ASSERT_NE(ctx, nullptr);
  ASSERT_NE(solver, nullptr);
}

TEST_F(Z3BasicTest, boolean_sorts_and_values)
{
  z3::sort bool_sort = ctx->bool_sort();
  z3::expr true_expr = ctx->bool_val(true);
  z3::expr false_expr = ctx->bool_val(false);
  
  ASSERT_TRUE(bool_sort.is_bool());
  ASSERT_TRUE(true_expr.is_bool());
  ASSERT_TRUE(false_expr.is_bool());
}

TEST_F(Z3BasicTest, integer_operations)
{
  z3::sort int_sort = ctx->int_sort();
  z3::expr x = ctx->int_const("x");
  z3::expr y = ctx->int_const("y");
  z3::expr zero = ctx->int_val(0);
  
  ASSERT_TRUE(int_sort.is_int());
  ASSERT_TRUE(x.is_int());
  ASSERT_TRUE(y.is_int());
  
  z3::expr sum = x + y;
  ASSERT_TRUE(sum.is_int());
}

TEST_F(Z3BasicTest, bitvector_operations)
{
  z3::sort bv8_sort = ctx->bv_sort(8);
  z3::expr a = ctx->bv_const("a", 8);
  z3::expr b = ctx->bv_const("b", 8);
  
  ASSERT_TRUE(bv8_sort.is_bv());
  ASSERT_TRUE(a.is_bv());
  ASSERT_EQ(a.get_sort().bv_size(), 8);
  
  z3::expr sum = a + b;
  ASSERT_TRUE(sum.is_bv());
}

TEST_F(Z3BasicTest, array_operations)
{
  z3::sort int_sort = ctx->int_sort();
  z3::sort bool_sort = ctx->bool_sort();
  z3::sort array_sort = ctx->array_sort(int_sort, bool_sort);
  
  ASSERT_TRUE(array_sort.is_array());
  
  z3::expr arr = ctx->constant("arr", array_sort);
  z3::expr idx = ctx->int_val(0);
  z3::expr val = ctx->bool_val(true);
  
  z3::expr select_expr = z3::select(arr, idx);
  ASSERT_TRUE(select_expr.is_bool());
  
  z3::expr store_expr = z3::store(arr, idx, val);
  ASSERT_TRUE(store_expr.get_sort().is_array());
}

TEST_F(Z3BasicTest, check_sat_unsat)
{
  z3::expr x = ctx->bool_const("x");
  z3::expr not_x = !x;
  z3::expr contradiction = x && not_x;
  
  solver->add(contradiction);
  z3::check_result result = solver->check();
  ASSERT_EQ(result, z3::unsat);
}

TEST_F(Z3BasicTest, check_sat_sat)
{
  z3::expr x = ctx->int_const("x");
  z3::expr zero = ctx->int_val(0);
  z3::expr x_gt_zero = x > zero;
  
  solver->add(x_gt_zero);
  z3::check_result result = solver->check();
  ASSERT_EQ(result, z3::sat);
}

TEST_F(Z3BasicTest, push_pop)
{
  z3::expr x = ctx->bool_const("x");
  
  solver->add(x);
  solver->push();
  
  z3::expr not_x = !x;
  solver->add(not_x);
  
  z3::check_result result = solver->check();
  ASSERT_EQ(result, z3::unsat);
  
  solver->pop();
  result = solver->check();
  ASSERT_EQ(result, z3::sat);
}

TEST_F(Z3BasicTest, get_model)
{
  z3::expr x = ctx->int_const("x");
  z3::expr five = ctx->int_val(5);
  z3::expr x_eq_five = x == five;
  
  solver->add(x_eq_five);
  z3::check_result result = solver->check();
  ASSERT_EQ(result, z3::sat);
  
  z3::model m = solver->get_model();
  z3::expr x_value = m.eval(x);
  ASSERT_TRUE(x_value.is_numeral());
}

TEST_F(Z3BasicTest, string_sort)
{
  z3::sort string_sort = ctx->string_sort();
  z3::expr s = ctx->string_val("hello");
  
  ASSERT_TRUE(s.get_sort().to_string() == "String");
}

TEST_F(Z3BasicTest, floating_point_sort)
{
  z3::sort fp_sort = ctx->fpa_sort(8, 24);  // single precision
  z3::expr fp = ctx->constant("fp", fp_sort);
  
  ASSERT_TRUE(fp_sort.is_fpa());
}

TEST_F(Z3BasicTest, real_operations)
{
  z3::sort real_sort = ctx->real_sort();
  z3::expr x = ctx->real_const("x");
  z3::expr y = ctx->real_const("y");
  
  ASSERT_TRUE(real_sort.is_real());
  ASSERT_TRUE(x.is_real());
  
  z3::expr sum = x + y;
  z3::expr product = x * y;
  ASSERT_TRUE(sum.is_real());
  ASSERT_TRUE(product.is_real());
}

TEST_F(Z3BasicTest, multi_argument_division)
{
  z3::expr a = ctx->int_val(100);
  z3::expr b = ctx->int_val(10);
  z3::expr c = ctx->int_val(2);
  
  // Test left-associative division: (100 / 10) / 2 = 5
  z3::expr result = (a / b) / c;
  
  solver->add(result == ctx->int_val(5));
  z3::check_result check = solver->check();
  ASSERT_EQ(check, z3::sat);
}

TEST_F(Z3BasicTest, lambda_functions)
{
  // Test that Z3 supports lambda expressions (function sorts)
  z3::expr x = ctx->int_const("x");
  z3::expr body = x > ctx->int_val(0);
  
  // Create lambda: lambda x. x > 0
  z3::expr lambda_expr = z3::lambda(x, body);
  
  // Verify the lambda has an array sort (Z3 represents functions as arrays)
  ASSERT_TRUE(lambda_expr.get_sort().is_array());
  
  // The array sort should be Int -> Bool
  ASSERT_TRUE(lambda_expr.get_sort().array_domain().is_int());
  ASSERT_TRUE(lambda_expr.get_sort().array_range().is_bool());
  
  // Test lambda application
  z3::expr five = ctx->int_val(5);
  z3::expr app = z3::select(lambda_expr, five);
  
  // lambda(5) should be 5 > 0, which is true
  solver->add(app);
  z3::check_result check = solver->check();
  ASSERT_EQ(check, z3::sat);
}

TEST_F(Z3BasicTest, multi_argument_lambda)
{
  // Test multi-argument lambda: lambda x, y. x + y > 10
  z3::expr x = ctx->int_const("x");
  z3::expr y = ctx->int_const("y");
  z3::expr body = (x + y) > ctx->int_val(10);
  
  z3::expr_vector vars(*ctx);
  vars.push_back(x);
  vars.push_back(y);
  
  // Create lambda: lambda x, y. x + y > 10
  z3::expr lambda_expr = z3::lambda(vars, body);
  
  // Verify the lambda has an n-dimensional array sort
  // Z3 uses n-dimensional arrays: (Array Int Int Bool)
  ASSERT_TRUE(lambda_expr.get_sort().is_array());
  std::cout << "Multi-arg lambda sort: " << lambda_expr.get_sort() << std::endl;
  
  // For n-dimensional arrays, domain gives only the first dimension
  ASSERT_TRUE(lambda_expr.get_sort().array_domain().is_int());
  ASSERT_TRUE(lambda_expr.get_sort().array_range().is_bool());
  
  // Test application using Z3 C API for n-dimensional select
  Z3_ast indices[] = {ctx->int_val(5), ctx->int_val(6)};
  Z3_ast result_ast = Z3_mk_select_n(
    ctx->operator Z3_context(),
    lambda_expr,
    2,
    indices
  );
  z3::expr app(*ctx, result_ast);
  
  // lambda(5, 6) should be (5 + 6 > 10) = (11 > 10) = true
  solver->add(app);
  z3::check_result check = solver->check();
  ASSERT_EQ(check, z3::sat);
}

#endif  // MURXLA_USE_Z3
