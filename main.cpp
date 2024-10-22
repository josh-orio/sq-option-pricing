#include <cmath>
#include <iostream>
#include <numbers>
#include <random>
#include <format>

double N(double z) { // normal distribution cumulative probability
  return 0.5 * (1 + std::erf(z / std::sqrt(2.0)));
}

double dN(double z) { // normal distribution probability density
  return (1.0 / std::sqrt(2 * std::numbers::pi)) *
         std::pow(std::numbers::e, -0.5 * std::pow(z, 2));
}

class Option {
public:
  std::string underlying; // underlying asset symbol/name
  double sp;              // current stock price (spot)
  double st;              // strike price (strike)
  double t;               // time to expiration (years)
  double r;               // risk free interest rate
  double v;               // volatility of the underlying asset

  Option() {}

  Option(double SP, double ST, double T, double R, double V) {
    sp = SP;
    st = ST;
    t = T;
    r = R;
    v = V;
  }

  double d1() {
    return (std::log(sp / st) + (t * (r + (std::pow(v, 2) / 2)))) /
           (v * std::sqrt(t));
  }

  double d2() { return d1() - (v * std::sqrt(t)); }
};

class EuroCallOption : Option { // european call option
public:
  using Option::d1;
  using Option::d2;
  using Option::Option;
  using Option::r;
  using Option::sp;
  using Option::st;
  using Option::t;
  using Option::v;

  double price() {
    // C = (SP * N(d1)) - (ST * (e^-RT) * N(d2))
    return (sp * N(d1())) -
           (st * std::pow(std::numbers::e, -1 * r * t) * N(d2()));
  }

  double delta() {
    // D = N(d1)
    return N(d1());
  }

  double gamma() {
    // G = N'(d1) / (SP * V * (T ^ 0.5))
    return dN(d1()) / (sp * v * std::sqrt(t));
  }

  double theta() {
    // T = ((SP * N'(d1) * V) / (2 * (T ^ 0.5))) - (R * ST * (e ^ (-1 * R * T))
    // * N(d2))
    return ((sp * dN(d1()) * v) / (2 * (std::sqrt(t)))) -
           (r * st * std::pow(std::numbers::e, -1 * r * t) * N(d2()));
  }

  double vega() {
    // V = SP * (T ^ 0.5) * N'(d1)
    std::cout << sp << " " << std::sqrt(t) << " " << t << " " << dN(d1()) << " "
              << d1() << std::endl;
    return sp * std::sqrt(t) * dN(d1());
  }
};

class EuroPutOption : Option { // european put option
public:
  using Option::d1;
  using Option::d2;
  using Option::Option;
  using Option::r;
  using Option::sp;
  using Option::st;
  using Option::t;
  using Option::v;

  double price() {
    // P = (ST * (e^-RT) * N(-d2)) - (SP * N(-d1))
    return (st * std::pow(std::numbers::e, -1 * r * t) * N(-1 * d2())) -
           (sp * N(-1 * d1()));
  }

  double delta() {
    // D = 1 - N(d1)
    return 1 - N(d1());
  }

  double gamma() {
    // G = N'(d1) / (SP * V * (T ^ 0.5))
    return dN(d1()) / (sp * v * std::sqrt(t));
  }

  double theta() {
    // T = ((SP * N'(d1) * V) / (2 * (T ^ 0.5))) - (R * ST * (e ^ (-1 * R * T))
    // * N(-d2))
    return ((sp * dN(d1()) * v) / (2 * std::sqrt(t))) -
           (r * st * std::pow(std::numbers::e, -1 * r * t) * N(d2()));
  }

  double vega() {
    // V = SP * (T ^ 0.5) * N'(d1)
    return sp * std::sqrt(t) * dN(d1());
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

    (C/P) Price (call/put) - price of the contract

    (SP) Spot Price - current price of the underlying asset
    (ST) Strike Price - price at which the option can be exercised
    (T)  Time To Expiration - duration for which the option agreement is valid
    (R)  Risk Free Rate - same as discount rate
    (V)  Volatility - rate of price change in the underlying

    (N) - normal distribution cumulative probability
    (d1) - measures delta between spot and strike, considering time to
    expiration and volatility (d2) - represents probability that the option
    expires ITM */

    /* Call Price:
    C = (SP * N(d1)) - (ST * (e^-RT) * N(d2))

    Put Price:
    P = (ST * (e^-RT) * N(-d2)) - (SP * N(-d1))

    D1 & D2:
    d1 = (ln(SP / ST) + (R + (T * (v ^ 2) / 2))) / (V * (T ^ 1/2))
    d2 = d1 - (V * (T ^ 1/2)) */

    /* The Greeks:

    (D)   Delta - measures change in an options price or premium as a result of
    a price change in the underlying asset (C)   D = N(d1) (P)   D = 1 - N(d1)

    (G)   Gamma - measures the rate of change of delta over time
    (C&P) G = N'(d1) / (SP * V * (T ^ 0.5))

    (T)   Theta - measures price decay of the option as time passes
    (C)   T = ((SP * N'(d1) * V) / (2 * (T ^ 0.5))) - (R * ST * (e ^ (-1 * R *
    T)) * N(d2)) (P)   T = ((SP * N'(d1) * V) / (2 * (T ^ 0.5))) - (R * ST * (e
    ^ (-1 * R * T)) * N(-d2))

    (V)   Vega - measures the risk of changes in implied volatility or the
    expected (future) volatility of the UA price (C&P) V = SP * (T ^ 0.5) *
    N'(d1)

    NOTES:
    - vega calculation is currently set for unit change, not percentage change
    of volatility e.g. 1.0 -> 2.0 (NOT 1% -> 2%)
    */
  }

  if (false) { // verifying d1 and d2 implementation
    Option o(100, 105, 1, 0.05,
             0.1985); // 19.85% v sourced from
                      // https://www.gurufocus.com/term/volatility/MSFT
                      // (Jul 07, 2024)

    std::cout << std::format("d1: {:.6f}\n", o.d1())
              << std::endl; // 0.105345 calculated
    std::cout << std::format("d2: {:.6f}\n", o.d2())
              << std::endl; // -0.093155 calculated
    std::cout << std::endl;

    // https://www.calkoo.com/en/black-scholes-option-pricing-model
    // ^ this website agrees, some others dont ¯\_(ツ)_/¯
    // possibly due to in-process rounding?
  }

  if (false) { // verifying cumulative prob. implementation
    for (int i = -3; i <= 3; i++) {
      std::cout << std::format("{} -> {:.6f}\n", i, N(i));
    }
  }

  // https://www.option-price.com/index.php
  if (true) { // verifying euro call pricing
    EuroCallOption eco(100, 105, 1, 0.05, 0.1985);
    std::cout << std::format("C Price: {:.6f}\n", eco.price());
    // expected 7.96186 (from op.com ^^^)
    // calculated 7.96184 (got same from calkoo.com)

    // verifying greeks
    std::cout << std::format("D: {:.6f}\n", eco.delta())
              << std::format("G: {:.6f}\n", eco.gamma())
              << std::format("T: {:.6f}\n", eco.theta()) // just wrong
              << std::format("V: {:.6f}\n", eco.vega()) << std::endl;
  }

  if (true) { // verifying euro put pricing
    EuroPutOption epo(100, 95, 1, 0.05, 0.1985);
    std::cout << std::format("P Price: {:.6f}\n", epo.price());
    // expected 7.84095
    // calculated 7.84093

    // verifying greeks
    std::cout << std::format("D: {:.6f}\n", epo.delta()) // should be negative
              << std::format("G: {:.6f}\n", epo.gamma())
              << std::format("T: {:.6f}\n", epo.theta()) // just wrong
              << std::format("V: {:.6f}\n", epo.vega()) << std::endl;
  }

  // To fix/investigate:
  // Call - theta
  // Put - delta, theta

  return 0;
}
