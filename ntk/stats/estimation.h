#ifndef   	NTK_STATS_ESTIMATION_H_
# define   	NTK_STATS_ESTIMATION_H_

# include <ntk/core.h>
# include <ntk/stats/distributions.h>
# include <map>

namespace ntk
{
 	
  double model_log_likelihood(const std::map<double,double>& distrib, 
                              double nbelem,
                              const Distribution& model);
  
#if 0
  
  struct WeibullEstimates
  {
    double beta;
    double etha;
    double gamma;
    double logl;
  };
   
  void estimate_weibull(std::map<double,double> distribution, 
                        WeibullEstimates& init);
  
  void estimate_weibull3(std::map<double,double> distribution, 
                         WeibullEstimates& init);

  // Finds parameters b, n in the confidence region induced by confidence
  // which maximize wblinv(c, b, n)
	WeibullEstimates
  estimate_weibull_upper_bounds(std::map<double,double> distribution,
                                const WeibullEstimates& best_params,
                                double confidence,
                                double c);
  
  double estimate_lognormal(const std::map<double,double>& distrib,
                            LogNormalDistrib::Params& init);
  
  double estimate_exponential(const std::map<double,double>& distrib,
                              ExpDistrib::Params& init);
  
  double estimate_one_minus_exponential(const std::map<double,double>& distrib,
                                        OneMinusExpDistrib::Params& init);
  
  
  double estimate_fishertippett(const std::map<double,double>& distrib,
                                FisherTippettDistrib::Params& init);
  
  double estimate_frechet(const std::map<double,double>& distrib,
                          FrechetDistrib::Params& init);
  
  double estimate_frechet3(const std::map<double,double>& distrib,
                           FrechetDistrib::Params& init);

  double estimate_gamma(const std::map<double,double>& distrib,
			GammaDistrib::Params& init);
  
#endif
  
}

#endif 		  // !NTK_STATS_ESTIMATION_H_
