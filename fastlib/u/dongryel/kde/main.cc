#include "fastlib/fastlib_int.h"
#include "fft_kde.h"
#include "fgt_kde.h"
#include "kde.h"

int main(int argc, char *argv[]) {

  fx_init(argc, argv);

  const char *algorithm = fx_param_str_req(NULL, "method");
  bool do_naive = fx_param_exists(NULL, "do_naive");

  if(!strcmp(algorithm, "fft")) {
    FFTKde fft_kde;
    Vector fft_kde_results;
    Matrix query_dataset;
    Matrix reference_dataset;
    fft_kde.Init();
    fft_kde.Compute();

    fft_kde_results.Copy(fft_kde.get_density_estimates());
    query_dataset.Copy(fft_kde.get_query_dataset());
    reference_dataset.Copy(fft_kde.get_reference_dataset());
    if(fx_param_exists(NULL, "fft_kde_output")) {
      fft_kde.PrintDebug();
    }

    if(do_naive) {
      NaiveKde<GaussianKernel> naive_kde;
      naive_kde.Init(query_dataset, reference_dataset);
      naive_kde.Compute();
      
      if(fx_param_exists(NULL, "naive_kde_output")) {
	naive_kde.PrintDebug();
      }
      naive_kde.ComputeMaximumRelativeError(fft_kde_results);
    }
  }
  else if(!strcmp(algorithm, "fgt")) {
    FGTKde fgt_kde;
    Vector fgt_kde_results;
    Matrix query_dataset;
    Matrix reference_dataset;
    fgt_kde.Init();    
    fgt_kde.Compute();
    fgt_kde_results.Copy(fgt_kde.get_density_estimates());
    query_dataset.Copy(fgt_kde.get_query_dataset());
    reference_dataset.Copy(fgt_kde.get_reference_dataset());
    if(fx_param_exists(NULL, "fgt_kde_output")) {
      fgt_kde.PrintDebug();
    }

    if(do_naive) {
      NaiveKde<GaussianKernel> naive_kde;
      naive_kde.Init(query_dataset, reference_dataset);
      naive_kde.Compute();
      
      if(fx_param_exists(NULL, "naive_kde_output")) {
	naive_kde.PrintDebug();
      }
      naive_kde.ComputeMaximumRelativeError(fgt_kde_results);
    }
  }
  else if(!strcmp(algorithm, "fast")) {
    if(!strcmp(fx_param_str(NULL, "kernel", "gaussian"), "gaussian")) {

      Vector fast_kde_results;
      Matrix query_dataset;
      Matrix reference_dataset;

      // for O(p^D) expansion
      if(fx_param_exists(NULL, "multiplicative_expansion")) {

	printf("O(p^D) expansion KDE\n");
	FastKde<GaussianKernelMultAux> fast_kde;
	fast_kde.Init();
	fast_kde.Compute();
      
	if(fx_param_exists(NULL, "fast_kde_output")) {
	  fast_kde.PrintDebug();
	}
      
	fast_kde.get_density_estimates(&fast_kde_results);
	query_dataset.Copy(fast_kde.get_query_dataset());
	reference_dataset.Copy(fast_kde.get_reference_dataset());
      }

      // otherwise do O(D^p) expansion
      else {

	printf("O(D^p) expansion KDE\n");
	FastKde<GaussianKernelAux> fast_kde;
	fast_kde.Init();
	fast_kde.Compute();
      
	if(fx_param_exists(NULL, "fast_kde_output")) {
	  fast_kde.PrintDebug();
	}
      
	fast_kde.get_density_estimates(&fast_kde_results);
	query_dataset.Copy(fast_kde.get_query_dataset());
	reference_dataset.Copy(fast_kde.get_reference_dataset());
      }
    
      if(do_naive) {
	NaiveKde<GaussianKernel> naive_kde;
	naive_kde.Init(query_dataset, reference_dataset);
	naive_kde.Compute();
      
	if(fx_param_exists(NULL, "naive_kde_output")) {
	  naive_kde.PrintDebug();
	}
	naive_kde.ComputeMaximumRelativeError(fast_kde_results);
      }
    
    }
    else if(!strcmp(fx_param_str(NULL, "kernel", "epan"), "epan")) {
      FastKde<EpanKernelAux> fast_kde;
      fast_kde.Init();
      fast_kde.Compute();
    
      if(fx_param_exists(NULL, "fast_kde_output")) {
	fast_kde.PrintDebug();
      }
      Vector fast_kde_results;
      fast_kde.get_density_estimates(&fast_kde_results);
    
      if(do_naive) {
	NaiveKde<EpanKernel> naive_kde;
	naive_kde.Init(fast_kde.get_query_dataset(),
		       fast_kde.get_reference_dataset());
	naive_kde.Compute();
      
	if(fx_param_exists(NULL, "naive_kde_output")) {
	  naive_kde.PrintDebug();
	}
	naive_kde.ComputeMaximumRelativeError(fast_kde_results);
      }
    }
  }

  fx_done();
  return 0;
}
