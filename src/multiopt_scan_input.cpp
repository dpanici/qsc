#include <cassert>
#include <mpi.h>
#include "multiopt_scan.hpp"
#include "toml.hpp"
#include "toml_util.hpp"

using namespace qsc;

/** Read in a scan input file
 */
void MultiOptScan::input(std::string filename)
{

  MPI_Comm_rank(mpi_comm, &mpi_rank);
  MPI_Barrier(mpi_comm);

  mo_ref.input(filename);

  auto toml_file = toml::parse(filename);
  auto indata = toml::find(toml_file, "multiopt_scan");

  std::vector<std::string> varlist;

  toml_read(varlist, indata, "verbose", verbose);
  toml_read(varlist, indata, "quit_after_init", quit_after_init);
  toml_read(varlist, indata, "save_period", save_period);
  toml_read(varlist, indata, "print_status_period", print_status_period);
  toml_read(varlist, indata, "max_seconds", max_seconds);

  toml_read(varlist, indata, "params", params);
  toml_read(varlist, indata, "params_min", params_min);
  toml_read(varlist, indata, "params_max", params_max);
  toml_read(varlist, indata, "params_n", params_n);
  toml_read(varlist, indata, "params_stage", params_stage);
  toml_read(varlist, indata, "params_log", params_log);

  toml_read(varlist, indata, "keep_all", keep_all);
  toml_read(varlist, indata, "min_R0_to_keep", min_R0_to_keep);
  toml_read(varlist, indata, "max_R0_to_keep", max_R0_to_keep);
  toml_read(varlist, indata, "min_iota_to_keep", min_iota_to_keep);
  toml_read(varlist, indata, "max_elongation_to_keep", max_elongation_to_keep);
  toml_read(varlist, indata, "min_L_grad_B_to_keep", min_L_grad_B_to_keep);
  toml_read(varlist, indata, "min_L_grad_grad_B_to_keep", min_L_grad_grad_B_to_keep);
  toml_read(varlist, indata, "max_B20_variation_to_keep", max_B20_variation_to_keep);
  toml_read(varlist, indata, "min_r_singularity_to_keep", min_r_singularity_to_keep);
  toml_read(varlist, indata, "max_d2_volume_d_psi2_to_keep", max_d2_volume_d_psi2_to_keep);
  toml_read(varlist, indata, "min_DMerc_times_r2_to_keep", min_DMerc_times_r2_to_keep);
  toml_read(varlist, indata, "max_XY2_to_keep", max_XY2_to_keep);
  toml_read(varlist, indata, "max_Z2_to_keep", max_Z2_to_keep);
  toml_read(varlist, indata, "max_XY3_to_keep", max_XY3_to_keep);
  toml_read(varlist, indata, "max_d_XY2_d_varphi_to_keep", max_d_XY2_d_varphi_to_keep);
  toml_read(varlist, indata, "max_d_Z2_d_varphi_to_keep", max_d_Z2_d_varphi_to_keep);
  toml_read(varlist, indata, "max_d_XY3_d_varphi_to_keep", max_d_XY3_d_varphi_to_keep);

  toml_unused(varlist, indata);

  ndim = params.size();
  assert(params_max.size() == ndim);
  assert(params_min.size() == ndim);
  assert(params_n.size() == ndim);
  assert(params_log.size() == ndim);
  assert(params_stage.size() == ndim);

  if (mpi_rank == 0)
  {
    std::cout << "----- MultiOpt Scan parameters -----" << std::endl;
    std::cout << "parameters to scan, min, max, n, log, stage: " << std::endl;
    for (int j = 0; j < ndim; j++)
    {
      std::cout << " " << params[j] << ", " << params_min[j] << ", " << params_max[j];
      std::cout << ", " << params_n[j] << ", " << params_log[j] << ", " << params_stage[j] << std::endl;
    }
    std::cout << "save_period: " << save_period << std::endl;
    std::cout << "max_seconds: " << max_seconds << std::endl;
    std::cout << "keep_all: " << keep_all << std::endl;
    if (!keep_all)
    {
      std::cout << "min_R0_to_keep: " << min_R0_to_keep << std::endl;
      std::cout << "max_R0_to_keep: " << max_R0_to_keep << std::endl;
      std::cout << "min_iota_to_keep: " << min_iota_to_keep << std::endl;
      std::cout << "max_elongation_to_keep: " << max_elongation_to_keep << std::endl;
      std::cout << "min_L_grad_B_to_keep: " << min_L_grad_B_to_keep << std::endl;
      std::cout << "min_L_grad_grad_B_to_keep: " << min_L_grad_grad_B_to_keep << std::endl;
      std::cout << "max_B20_variation_to_keep: " << max_B20_variation_to_keep << std::endl;
      std::cout << "min_r_singularity_to_keep: " << min_r_singularity_to_keep << std::endl;
      std::cout << "max_d2_volume_d_psi2_to_keep: " << max_d2_volume_d_psi2_to_keep << std::endl;
      std::cout << "min_DMerc_times_r2_to_keep: " << min_DMerc_times_r2_to_keep << std::endl;
      std::cout << "max_XY2_to_keep: " << max_XY2_to_keep << std::endl;
      std::cout << "max_Z2_to_keep: " << max_Z2_to_keep << std::endl;
      std::cout << "max_XY3_to_keep: " << max_XY3_to_keep << std::endl;
      std::cout << "max_d_XY2_d_varphi_to_keep: " << max_d_XY2_d_varphi_to_keep << std::endl;
      std::cout << "max_d_Z2_d_varphi_to_keep: " << max_d_Z2_d_varphi_to_keep << std::endl;
      std::cout << "max_d_XY3_d_varphi_to_keep: " << max_d_XY3_d_varphi_to_keep << std::endl;
    }
    std::cout << "Leaving multiopt_scan_input" << std::endl;
  }
}
