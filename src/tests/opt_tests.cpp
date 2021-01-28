#include <stdexcept>
#include <iostream>
#include <gsl/gsl_vector.h>
#include "doctest.h"
#include "opt.hpp"

using namespace qsc;
using doctest::Approx;

TEST_CASE("Each term in the objective function should be approximately independent of nphi") {
  if (single) return;
  
  Opt o1, o2;
  o1.q = Qsc("r2 section 5.5");
  o2.q = Qsc("r2 section 5.5");
  // std::cout << "nphi=" << o1.q.nphi << "  R0c=" << o1.q.R0c << " etabar=" << o1.q.eta_bar << " iota=" << o1.q.iota << std::endl;

  o1.q.nphi = 51;
  o2.q.nphi = 101;

  o1.q.init();
  o2.q.init();
  o1.q.calculate();
  o2.q.calculate();

  // Turn on all residual terms:
  o1.weight_B20 = 2.0;
  o1.weight_iota = 3.0;
  o1.weight_R0 = 4.0;
  o1.min_R0 = 0.8;
  o1.weight_d2_volume_d_psi2 = 5.0;
  o1.weight_XY2 = 6.0;
  o1.weight_XY2Prime = 7.0;
  o1.weight_XY3 = 8.0;
  o1.weight_XY3Prime = 9.0;
  o1.weight_grad_grad_B = 10.0;
  
  o2.weight_B20 = 2.0;
  o2.weight_iota = 3.0;
  o2.weight_R0 = 4.0;
  o2.min_R0 = 0.8;
  o2.weight_d2_volume_d_psi2 = 5.0;
  o2.weight_XY2 = 6.0;
  o2.weight_XY2Prime = 7.0;
  o2.weight_XY3 = 8.0;
  o2.weight_XY3Prime = 9.0;
  o2.weight_grad_grad_B = 10.0;

  o1.init();
  o2.init();
  
  gsl_vector *res1 = gsl_vector_alloc(o1.n_terms);
  gsl_vector *res2 = gsl_vector_alloc(o2.n_terms);
  o1.set_residuals(res1);
  o2.set_residuals(res2);

  qscfloat tol = 1.0e-8;
  CHECK(Approx(o1.objective_function).epsilon(tol) == o2.objective_function);
  CHECK(Approx(o1.B20_term).epsilon(tol) == o2.B20_term);
  CHECK(Approx(o1.iota_term).epsilon(tol) == o2.iota_term);
  CHECK(Approx(o1.R0_term).epsilon(1.0e-4) == o2.R0_term); // This term needs a wider tolerance
  CHECK(Approx(o1.d2_volume_d_psi2_term).epsilon(tol) == o2.d2_volume_d_psi2_term);
  CHECK(Approx(o1.XY2_term).epsilon(tol) == o2.XY2_term);
  CHECK(Approx(o1.XY2Prime_term).epsilon(tol) == o2.XY2Prime_term);
  CHECK(Approx(o1.XY3_term).epsilon(tol) == o2.XY3_term);
  CHECK(Approx(o1.XY3Prime_term).epsilon(tol) == o2.XY3Prime_term);
  CHECK(Approx(o1.grad_grad_B_term).epsilon(tol) == o2.grad_grad_B_term);

}

TEST_CASE("Running standalone QSC on each configuration in the optimization history should give the corresponding iter_ values. Also confirm that inputs were fixed or varied as requested.") {
  if (single) return;
  int j, k;

  std::string config = "r2 section 5.5";
  // q0 will be a reference configuration from before the optimization
  Qsc q0(config);
  q0.nphi = 51;
  q0.init();
  q0.calculate();

  for (int vary_axis_option = 0; vary_axis_option < 3; vary_axis_option++) {
    CAPTURE(vary_axis_option);
    for (int vary_scalars_option = 0; vary_scalars_option < 7; vary_scalars_option++) {
      CAPTURE(vary_scalars_option);
      
      Opt opt;
      opt.q = Qsc(config);
      opt.max_iter = 60;

      opt.q.nphi = q0.nphi;
      opt.q.verbose = 0;

      // Turn on all residual terms:
      opt.weight_B20 = 2.0;
      opt.weight_iota = 3.0;
      opt.weight_R0 = 4.0;
      opt.min_R0 = 0.8;
      opt.weight_d2_volume_d_psi2 = 5.0;
      opt.weight_XY2 = 6.0;
      opt.weight_XY2Prime = 7.0;
      opt.weight_XY3 = 8.0;
      opt.weight_XY3Prime = 9.0;
      opt.weight_grad_grad_B = 10.0;
      
      switch (vary_axis_option) {
      case 0:
	// Do not vary the axis
	opt.vary_R0c = {false, false};
	opt.vary_R0s = {false, false};
	opt.vary_Z0c = {false, false};
	opt.vary_Z0s = {false, false};
	break;
      case 1:
	// Do vary the axis, except for the major radius
	opt.vary_R0c = {false, true};
	opt.vary_R0s = {false, true};
	opt.vary_Z0c = {false, true};
	opt.vary_Z0s = {false, true};
	break;
      case 2:
	// Vary only selected Fourier modes
	opt.vary_R0c = {false, false};
	opt.vary_R0s = {false, true};
	opt.vary_Z0c = {false, true};
	opt.vary_Z0s = {false, true};
	break;
      default:
	CHECK(false); // Should not get here
      }

      switch (vary_scalars_option) {
      case 0:
	opt.vary_eta_bar = true;
	opt.vary_B2c = true;
	opt.vary_B2s = false;
	opt.vary_sigma0 = false;
	break;
      case 1:
	opt.vary_eta_bar = false;
	opt.vary_B2c = false;
	opt.vary_B2s = true;
	opt.vary_sigma0 = true;
	break;
      case 2:
	// Only eta_bar
	opt.vary_eta_bar = true;
	opt.vary_B2c = false;
	opt.vary_B2s = false;
	opt.vary_sigma0 = false;
	break;
      case 3:
	// Only B2c
	opt.vary_eta_bar = false;
	opt.vary_B2c = true;
	opt.vary_B2s = false;
	opt.vary_sigma0 = false;
	break;
      case 4:
	// Only B2s
	opt.vary_eta_bar = false;
	opt.vary_B2c = false;
	opt.vary_B2s = true;
	opt.vary_sigma0 = false;
	break;
      case 5:
	// Only sigma0
	opt.vary_eta_bar = false;
	opt.vary_B2c = false;
	opt.vary_B2s = false;
	opt.vary_sigma0 = true;
	break;
      case 6:
	// Vary everything
	opt.vary_eta_bar = true;
	opt.vary_B2c = true;
	opt.vary_B2s = true;
	opt.vary_sigma0 = true;
	break;
      default:
	CHECK(false); // Should not get here
      }

      // Run the optimization:
      opt.allocate();
      opt.optimize();
      // opt.outfilename = "qsc_out.opt_test.nc";
      // opt.write_netcdf();

      // Iteration 0 should exactly match the standalone reference q0:
      CHECK(Approx(opt.iter_eta_bar[0]) == q0.eta_bar);
      CHECK(Approx(opt.iter_B2c[0]) == q0.B2c);
      CHECK(Approx(opt.iter_B2s[0]) == q0.B2s);
      CHECK(Approx(opt.iter_sigma0[0]) == q0.sigma0);
      for (k = 0; k < q0.R0c.size(); k++) {
      	CHECK(Approx(opt.iter_R0c(k, 0)) == q0.R0c[k]);
	CHECK(Approx(opt.iter_R0s(k, 0)) == q0.R0s[k]);
	CHECK(Approx(opt.iter_Z0c(k, 0)) == q0.Z0c[k]);
	CHECK(Approx(opt.iter_Z0s(k, 0)) == q0.Z0s[k]);
      }
      CHECK(Approx(q0.grid_min_R0) == opt.iter_min_R0[0]);
      CHECK(Approx(q0.grid_max_curvature) == opt.iter_max_curvature[0]);
      CHECK(Approx(q0.iota) == opt.iter_iota[0]);
      CHECK(Approx(q0.grid_max_elongation) == opt.iter_max_elongation[0]);
      CHECK(Approx(q0.grid_min_L_grad_B) == opt.iter_min_L_grad_B[0]);
      CHECK(Approx(q0.grid_min_L_grad_grad_B) == opt.iter_min_L_grad_grad_B[0]);
      CHECK(Approx(q0.r_singularity_robust) == opt.iter_r_singularity[0]);
      CHECK(Approx(q0.B20_grid_variation) == opt.iter_B20_variation[0]);
      CHECK(Approx(q0.B20_residual) == opt.iter_B20_residual[0]);
      CHECK(Approx(q0.d2_volume_d_psi2) == opt.iter_d2_volume_d_psi2[0]);
      CHECK(Approx(q0.DMerc_times_r2) == opt.iter_DMerc_times_r2[0]);
      CHECK(Approx(q0.standard_deviation_of_R) == opt.iter_standard_deviation_of_R[0]);
      CHECK(Approx(q0.standard_deviation_of_Z) == opt.iter_standard_deviation_of_Z[0]);

      // Now set up a standalone QSC to check each iteration
      Qsc q;
      q.nphi = opt.q.nphi;
      q.nfp = opt.q.nfp;
      q.I2 = opt.q.I2;
      q.p2 = opt.q.p2;
      q.verbose = 0;
      q.order_r_option = opt.q.order_r_option;
      q.R0c.resize(opt.q.R0c.size(), 0.0);
      q.R0s.resize(opt.q.R0c.size(), 0.0);
      q.Z0c.resize(opt.q.R0c.size(), 0.0);
      q.Z0s.resize(opt.q.R0c.size(), 0.0);
      q.init();
      for (j = 0; j < opt.n_iter; j++) {
	CAPTURE(j);
	// Take inputs from one element of the optimization history and
	// plug them into a standalone QSC:
	q.eta_bar = opt.iter_eta_bar[j];
	q.sigma0 = opt.iter_sigma0[j];
	q.B2c = opt.iter_B2c[j];
	q.B2s = opt.iter_B2s[j];
	for (k = 0; k < opt.q.R0c.size(); k++) {
	  q.R0c[k] = opt.iter_R0c(k, j);
	  q.R0s[k] = opt.iter_R0s(k, j);
	  q.Z0c[k] = opt.iter_Z0c(k, j);
	  q.Z0s[k] = opt.iter_Z0s(k, j);
	}
	// Run standalone QSC:
	q.calculate();
	
	// Compare:
	CHECK(Approx(q.grid_min_R0) == opt.iter_min_R0[j]);
	CHECK(Approx(q.grid_max_curvature) == opt.iter_max_curvature[j]);
	CHECK(Approx(q.iota) == opt.iter_iota[j]);
	CHECK(Approx(q.grid_max_elongation) == opt.iter_max_elongation[j]);
	CHECK(Approx(q.grid_min_L_grad_B) == opt.iter_min_L_grad_B[j]);
	CHECK(Approx(q.grid_min_L_grad_grad_B) == opt.iter_min_L_grad_grad_B[j]);
	CHECK(Approx(q.r_singularity_robust) == opt.iter_r_singularity[j]);
	CHECK(Approx(q.B20_grid_variation) == opt.iter_B20_variation[j]);
	CHECK(Approx(q.B20_residual) == opt.iter_B20_residual[j]);
	CHECK(Approx(q.d2_volume_d_psi2) == opt.iter_d2_volume_d_psi2[j]);
	CHECK(Approx(q.DMerc_times_r2) == opt.iter_DMerc_times_r2[j]);
	CHECK(Approx(q.standard_deviation_of_R) == opt.iter_standard_deviation_of_R[j]);
	CHECK(Approx(q.standard_deviation_of_Z) == opt.iter_standard_deviation_of_Z[j]);

      
	// Check if things were fixed that were supposed to be fixed,
	// and things were varied that were supposed to be varied:
	CHECK(Approx(opt.iter_R0c(0, j)) == q0.R0c[0]);
	CHECK(Approx(opt.iter_R0s(0, j)) == q0.R0s[0]);
	CHECK(Approx(opt.iter_Z0c(0, j)) == q0.Z0c[0]);
	CHECK(Approx(opt.iter_Z0s(0, j)) == q0.Z0s[0]);
	switch (vary_axis_option) {
	case 0:
	  // Do not vary the axis
	  CHECK(Approx(opt.iter_R0c(1, j)) == q0.R0c[1]);
	  CHECK(Approx(opt.iter_R0s(1, j)) == q0.R0s[1]);
	  CHECK(Approx(opt.iter_Z0c(1, j)) == q0.Z0c[1]);
	  CHECK(Approx(opt.iter_Z0s(1, j)) == q0.Z0s[1]);
	  break;
	case 1:
	  // Do vary the axis, except for the major radius
	  if (j > 0) CHECK(Approx(opt.iter_R0c(1, j)) != q0.R0c[1]);
	  if (j > 0) CHECK(Approx(opt.iter_R0s(1, j)) != q0.R0s[1]);
	  if (j > 0) CHECK(Approx(opt.iter_Z0c(1, j)) != q0.Z0c[1]);
	  if (j > 0) CHECK(Approx(opt.iter_Z0s(1, j)) != q0.Z0s[1]);
	  break;
	case 2:
	  // Vary only selected Fourier modes
	  CHECK(Approx(opt.iter_R0c(1, j)) == q0.R0c[1]);
	  if (j > 0) CHECK(Approx(opt.iter_R0s(1, j)) != q0.R0s[1]);
	  if (j > 0) CHECK(Approx(opt.iter_Z0c(1, j)) != q0.Z0c[1]);
	  if (j > 0) CHECK(Approx(opt.iter_Z0s(1, j)) != q0.Z0s[1]);
	  break;
	default:
	  CHECK(false); // Should not get here
	}
	
	switch (vary_scalars_option) {
	case 0:
	  if (j > 0) CHECK(Approx(opt.iter_eta_bar[j]) != q0.eta_bar);
	  if (j > 0) CHECK(Approx(opt.iter_B2c[j]) != q0.B2c);
	  CHECK(Approx(opt.iter_B2s[j]) == q0.B2s);
	  CHECK(Approx(opt.iter_sigma0[j]) == q0.sigma0);
	  break;
	case 1:
	  CHECK(Approx(opt.iter_eta_bar[j]) == q0.eta_bar);
	  CHECK(Approx(opt.iter_B2c[j]) == q0.B2c);
	  if (j > 0) CHECK(Approx(opt.iter_B2s[j]) != q0.B2s);
	  if (j > 0) CHECK(Approx(opt.iter_sigma0[j]) != q0.sigma0);
	  break;
	case 2:
	  // Only eta_bar
	  if (j > 0) CHECK(Approx(opt.iter_eta_bar[j]) != q0.eta_bar);
	  CHECK(Approx(opt.iter_B2c[j]) == q0.B2c);
	  CHECK(Approx(opt.iter_B2s[j]) == q0.B2s);
	  CHECK(Approx(opt.iter_sigma0[j]) == q0.sigma0);
	  break;
	case 3:
	  // Only B2c
	  CHECK(Approx(opt.iter_eta_bar[j]) == q0.eta_bar);
	  if (j > 0) CHECK(Approx(opt.iter_B2c[j]) != q0.B2c);
	  CHECK(Approx(opt.iter_B2s[j]) == q0.B2s);
	  CHECK(Approx(opt.iter_sigma0[j]) == q0.sigma0);
	  break;
	case 4:
	  // Only B2s
	  CHECK(Approx(opt.iter_eta_bar[j]) == q0.eta_bar);
	  CHECK(Approx(opt.iter_B2c[j]) == q0.B2c);
	  if (j > 0) CHECK(Approx(opt.iter_B2s[j]) != q0.B2s);
	  CHECK(Approx(opt.iter_sigma0[j]) == q0.sigma0);
	  break;
	case 5:
	  // Only sigma0
	  CHECK(Approx(opt.iter_eta_bar[j]) == q0.eta_bar);
	  CHECK(Approx(opt.iter_B2c[j]) == q0.B2c);
	  CHECK(Approx(opt.iter_B2s[j]) == q0.B2s);
	  if (j > 0) CHECK(Approx(opt.iter_sigma0[j]) != q0.sigma0);
	  break;
	case 6:
	  // Vary everything
	  if (j > 0) CHECK(Approx(opt.iter_eta_bar[j]) != q0.eta_bar);
	  if (j > 0) CHECK(Approx(opt.iter_B2c[j]) != q0.B2c);
	  if (j > 0) CHECK(Approx(opt.iter_B2s[j]) != q0.B2s);
	  if (j > 0) CHECK(Approx(opt.iter_sigma0[j]) != q0.sigma0);
	  break;
	default:
	  CHECK(false); // Should not get here
	}
      }
    }
  }


}
