#define OPTIM_ENABLE_EIGEN_WRAPPERS
#include "optim/optim.hpp"
        
#define OPTIM_PI 3.14159265358979

double 
ackley_fn(const Eigen::VectorXd& vals_inp, Eigen::VectorXd* , void* )
{
    const double x = vals_inp(0);
    const double y = vals_inp(1);

    const double obj_val = 20 + std::exp(1) - 20*std::exp( -0.2*std::sqrt(0.5*(x*x + y*y)) ) - std::exp( 0.5*(std::cos(2 * OPTIM_PI * x) + std::cos(2 * OPTIM_PI * y)) );
            
    return obj_val;
}