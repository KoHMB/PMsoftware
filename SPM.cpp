#include <TMB.hpp>

template<class Type>
bool isNA(Type x){
  return R_IsNA(asDouble(x));
}

template<class Type>
Type objective_function<Type>::operator() ()
{
  DATA_VECTOR(cpue1);
  DATA_VECTOR(Catch);

  PARAMETER(logit_D0);
  PARAMETER(log_r);
  PARAMETER(log_K);
  PARAMETER(log_m);
  PARAMETER(log_q1);
  PARAMETER(log_sig1);
  PARAMETER(log_tau);
  PARAMETER_VECTOR(log_Dep);

  int n = Catch.size();
  Type r = exp(log_r);
  Type K = exp(log_K);
  Type m = exp(log_m);
  Type q1 = exp(log_q1);
  Type sig1 = exp(log_sig1);
  Type tau = exp(log_tau);
  Type MSY = r*K/(pow(m,(m/(m-1))));
  Type Bmsy = K*pow(m, -1/(m-1));

  vector<Type> Dep(n);
  vector<Type> B(n);
  vector<Type> mu(n);

  Type nll;
  nll=0;

  for(int i=0;i<n;i++){
    Dep(i) = exp(log_Dep(i));
    B(i) = K*Dep(i);
  }

  mu(0) = 1/(1+exp(-logit_D0));
  nll += (-1.0)*dnorm(log(Dep(0)),log(mu(0)),tau,true);

  for(int i=1;i<n;i++){
    mu(i) = Dep(i-1) + (r/(m-1))*Dep(i-1)*(1-pow(Dep(i-1),m-1)) - Catch(i-1)/K;
    nll += (-1.0)*dnorm(log_Dep(i),log(mu(i)),tau,true);
  }

  for(int i=0;i<n;i++){
    if(!isNA(cpue1(i))){
      nll += (-1.0) * dnorm(log(cpue1(i)), log(q1*B(i)), sig1, true);
    }
  }

  ADREPORT(r);
  ADREPORT(K);
  ADREPORT(MSY);
  ADREPORT(Bmsy);
  ADREPORT(Dep);
  ADREPORT(B);
  ADREPORT(q1);
  ADREPORT(sig1);
  ADREPORT(tau);

  return nll;
}
