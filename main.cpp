#include <cmath>
#include <numbers>
#include <iostream>
#include <random>

double N(double z) {
  // returns cumulative probability of z-value
    return 0.5 * (1 + std::erf(z / std::sqrt(2.0)));
}

class Option {
public:
  std::string underlying; // underlying asset symbol/name
  double c;               // current stock price
  double s;               // strike price
  double t;               // time to expiration (years)
  double r;               // risk free interest rate
  double v;               // volatility of the underlying asset

  Option () {}

  Option(double C, double S, double T, double R, double V) {
    c = C;
    s = S;
    t = T;
    r = R;
    v = V;
  }

  double d1() {
    // T = time to expiration
    // v = volatility
    // r = risk free rate

    // d1 = (ln(S / K) + (r + ((v ^ 2) / 2))(T)) / (v * (T ^ 1/2))
    // some equation symbols adjusted for class symbols
    return (std::log(c / s) + ((r + (std::pow(v, 2) / 2)) * t)) /
           (v * std::pow(t, 0.5));
  }

  double d2() {
    // T = time to expiration
    // v = volatility
    // r = risk free rate

    // d2 = d1 - (v * (T ^ 1/2))
    return d1() - (v * std::pow(t, 0.5));
  }
};

class EuroCallOption : Option { // european call option
public:
  using Option::Option;
  using Option::d1;
  using Option::d2;

  double price() {
    // C = (S * N(d1)) - (K * (e^-rt) * N(d2))
    return (c * N(d1())) - (s * std::pow(std::numbers::e, -1 * r * t) * N(d2()));
  }
};

class EuroPutOption : Option { // european put option
public:
  using Option::Option;
  using Option::d1;
  using Option::d2;

  double price() {
    return (s * std::pow(std::numbers::e, -1 * r * t) * N(-1 * d2())) - (c * N(-1 * d1()));
  }
};

class RandDouble {
private:
  std::random_device rd;
  std::default_random_engine re;
  std::uniform_real_distribution<double> rng;

public:
  RandDouble(double min, double max) {
    re = std::default_random_engine(rd());
    rng = std::uniform_real_distribution<double>(min, max);
  }

  double next() { return rng(re); }
};

int main() {
  std::cout << "Option Pricer - Joshua O'Riordan" << std::endl << std::endl;

  if (true) { // comments
    /* Option Attributes:

    Current Stock Price - present price of the underlying asset
    Strike Price - price at which the option can be exercised
    Time To Expiration - duration for which the option agreement is valid
    Risk Free Rate - guaranteed rate of return (? some sort of discount rate anyway)
    Volatility - rate of price change in the underlying */


    /* Call Price Formula:

    C = (S * N(d1)) - (K * (e^-rt) * N(d2))

    C = call price
    S = stock price
    N(d1) = cumulative conditional probability, describes the delta between current price and expected price IF the option expires in the money
    K = strike price
    e^-rt = e ^ (-1 * risk free rate * time to exp.f)
    N(d2) = cumulative probability that the option will expire in the money */


    /* Put Price Formula:

    P = (K * (e^-rt) * N(-d2)) - (S * N(-d1))

    P = put price
    all other symbols same as call */


    /* N(d1) = delta (sizes the hedge that should be used against the option position)
    N(d2) = cumulative prob. that the option is ITM

    d1 = (ln(S / K) + (r + ((v ^ 2) / 2))(T)) / (v * (T ^ 1/2))
    d2 = d1 - (v * (T ^ 1/2))

    T = time to expiration
    v = volatility
    r = risk free rate */

    /* The Greeks:

    Delta - measures change in an options price or premium as a result of a price change in the underlying asset
    Theta - measures price decay of the option as time passes
    Gamma - measures the rate of change of delta over time
    Vega - measures the risk of changes in implied volatility or the expected (future) volatility of the UA price

    */
  }

  if (true) { // verifying d1 and d2 implementation
    Option o (100, 105, 1, 0.05, 0.1985); // 19.85% v sourced from https://www.gurufocus.com/term/volatility/MSFT (Jul 07, 2024)
    std::cout << o.d1() << std::endl;
    std::cout << o.d2() << std::endl;
    std::cout << std::endl;
  }

  if (true) { // verifying cumulative prob. implementation
    std::cout << N(-3) << std::endl;
    std::cout << N(-2) << std::endl;
    std::cout << N(-1) << std::endl;
    std::cout << N(0) << std::endl;
    std::cout << N(1) << std::endl;
    std::cout << N(2) << std::endl;
    std::cout << N(3) << std::endl;
    std::cout << std::endl;
  }


  // https://www.option-price.com/index.php 

  if (true) { // verifying euro call pricing
    EuroCallOption eco(100, 105, 1, 0.05, 0.1985);
    std::cout << eco.price() << std::endl << std::endl;
    // expected 7.96186
    // calculated 7.96184
  }

  if (true) { // verifying euro put pricing
    EuroPutOption epo(100, 95, 1, 0.05, 0.1985);
    std::cout << epo.price() << std::endl << std::endl;
    // expected 7.84095
    // calculated 7.84093
  }

  // slight mismatch to expected value, could be:
  // improper implementation of N(), d1() or d2()
  // unlikely to be bad pricing formulae, otherwise error would be larger (right ?)

  // TODO: implement methods for calculation of 'greeks'

  return 0;
}
