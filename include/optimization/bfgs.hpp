#pragma once

#include <cmath>
#include <eigen3/Eigen/Core>
#include <iostream>
#include <numeric>
#include <vector>

namespace bfgs {

class result {
public:
  result() = delete;
  result(bool success, const Eigen::VectorXd &x, double fval, double nfev,
         double njev, double nit, const std::vector<Eigen::VectorXd> &path)
      : success(success), x(x), fval(fval), nfev(nfev), njev(njev), nit(nit),
        path(path) {}
  friend std::ostream &operator<<(std::ostream &os, const result &r) {
    auto msg = r.success ? "true" : "false";
    os << "success : " << msg << std::endl;
    os << "x : [";
    for (int i = 0; i < r.x.size() - 1; ++i) {
      os << r.x(i) << ", ";
    }
    os << r.x(r.x.size() - 1) << "]" << std::endl;
    os << "f(x) : " << r.fval << std::endl;
    os << "nfev : " << r.nfev << std::endl;
    os << "njev : " << r.njev << std::endl;
    os << "nit : " << r.nit; // << std::endl;
    return os;
  }
  bool success;
  Eigen::VectorXd x;
  double fval;
  double nfev;
  double njev;
  double nit;
  std::vector<Eigen::VectorXd> path;
};

const double epsilon = std::sqrt(std::numeric_limits<double>::epsilon());
// use scipy's epsilon for reproducibility
// const double epsilon = 1.4901161193847656e-08;
template <typename T> class optimizer {
public:
  optimizer() = delete;
  /*
  maxiter (200*n_params)
       The algorithm terminates if maxiter iterations were executed.
  _gtol(1e-5)
       The algorithm terminates if the norm of the gradient drops below
       this. For well-scaled problems, a rule of thumb is that you can
       expect to reduce the gradient norm by 8 orders of magnitude
       (sqrt(eps)) compared to the gradient norm at a "typical" point (a
       rough initial iterate for example). Further decrease is sometimes
       possible, but inexact floating point arithmetic will eventually
       limit the final accuracy. If _gtol is set too low, the
       algorithm may end up iterating forever (or at least until another
       stopping criterion triggers).
  */
  optimizer(T f, const Eigen::VectorXd &x0, bool retpath = false)
      : _objective(f), _n_params(x0.size()), _max_iter(200 * _n_params),
        _x0(x0), _gtol(5), _nfev(0), _njev(0), _retpath(retpath){};

  // set maximum iterations
  void max_iter(unsigned int iter) { _max_iter = iter; }
  // set gradient tolerance for termination
  void max_iter(double tol) { _gtol = tol; }

  // bfgs algorithm
  result optimize() {
    // objective function value at starting point
    double old_fval = _f(_x0);
    // gradient at starting point
    auto gfk = _fprime(_x0);
    // infinite norm of gradient
    double gnorm = gfk.cwiseAbs().maxCoeff();
    // initial step guess to dx ~ 1
    double old_old_fval = old_fval + gfk.norm() / 2;

    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(_n_params, _n_params);
    // hessian matrix at starting point
    Eigen::MatrixXd Hk = I;
    Eigen::MatrixXd A1 = Eigen::MatrixXd::Zero(_n_params, _n_params);
    Eigen::MatrixXd A2 = Eigen::MatrixXd::Zero(_n_params, _n_params);

    // current point
    Eigen::VectorXd xk(_x0);
    // search direction
    Eigen::VectorXd pk(_n_params);
    // point after step
    Eigen::VectorXd xpk1(_n_params);
    // gradient after step
    Eigen::VectorXd gfkp1(_n_params);
    // ?
    Eigen::VectorXd sk(_n_params);
    Eigen::VectorXd yk(_n_params);
    // step size
    double alpha_k;
    //?
    double rhok;
    bool status = false;
    // number of iterations
    unsigned int k = 0;
    while ((gnorm > _gtol) && (k < _max_iter)) {
      if (_retpath) {
        _path.push_back(xk);
      }
      pk = -(Hk * gfk);
      status =
          _line_search(alpha_k, xk, pk, gfk, gfkp1, old_fval, old_old_fval);
      if (!status) {
        // TODO WARNFLAG
        // std::cerr << e.what() << std::endl;
        break;
      }
      xpk1 = xk + alpha_k * pk;
      sk = xpk1 - xk;
      xk = xpk1;
      yk = gfkp1 - gfk;
      gfk = gfkp1;
      ++k;
      gnorm = gfk.cwiseAbs().maxCoeff();
      if (gnorm <= _gtol) {
        break;
      }
      rhok = 1.0 / yk.dot(sk);
      if (std::isnan(rhok) || std::isinf(rhok)) {
        // Divide-by-zero encountered: rhok assumed large
        rhok = 1000.0;
      }
      A1 = I - sk * yk.transpose() * rhok;
      A2 = I - yk * sk.transpose() * rhok;
      Hk = A1 * Hk * A2 + rhok * sk * sk.transpose();
    }

    if (k >= _max_iter) {
      return result(false, xk, old_fval, _nfev, _njev, k, _path);
    }

    // TODO return struct result
    return result(true, xk, old_fval, _nfev, _njev, k, _path);
  }

private:
  void dcstep(double &stx, double &fx, double &dx, double &sty, double &fy,
              double &dy, double &stp, double const &fp, double const &dp,
              bool &brackt, double const &stpmin, double const &stpmax) {

    double sgnd = dp * (dx / std::abs(dx));
    // First case: A higher function value. The minimum is bracketed.
    // If the cubic step is closer to stx than the quadratic step, the
    // cubic step is taken, otherwise the average of the cubic and
    // quadratic steps is taken.

    const double three = 3.0;
    double theta = 0.0;
    double s = 0.0;
    double gamma = 0.0;
    double p = 0.0;
    double q = 0.0;
    double r = 0.0;
    double stpc = 0.0;
    const double two = 2.0;
    double stpq = 0.0;
    double stpf = 0.0;
    const double p66 = 0.66e0;
    if (fp > fx) {
      theta = three * (fx - fp) / (stp - stx) + dx + dp;
      s = std::max(std::abs(theta), std::abs(dx));
      s = std::max(s, std::abs(dp));
      gamma = s * std::sqrt(std::pow((theta / s), 2.0) - (dx / s) * (dp / s));
      if (stp < stx) {
        gamma = -gamma;
      }
      p = (gamma - dx) + theta;
      q = ((gamma - dx) + gamma) + dp;
      r = p / q;
      stpc = stx + r * (stp - stx);
      stpq = stx + ((dx / ((fx - fp) / (stp - stx) + dx)) / two) * (stp - stx);
      if (std::abs(stpc - stx) < std::abs(stpq - stx)) {
        stpf = stpc;
      } else {
        stpf = stpc + (stpq - stpc) / two;
      }
      brackt = true;
      // Second case: A lower function value and derivatives of opposite
      // sign. The minimum is bracketed. If the cubic step is farther from
      // stp than the secant step, the cubic step is taken, otherwise the
      // secant step is taken.
    } else if (sgnd < 0.0) {
      theta = three * (fx - fp) / (stp - stx) + dx + dp;
      s = std::max(std::abs(theta), std::abs(dx));
      s = std::max(s, std::abs(dp));
      gamma = s * std::sqrt(std::pow((theta / s), 2.0) - (dx / s) * (dp / s));
      if (stp > stx) {
        gamma = -gamma;
      }
      p = (gamma - dp) + theta;
      q = ((gamma - dp) + gamma) + dx;
      r = p / q;
      stpc = stp + r * (stx - stp);
      stpq = stp + (dp / (dp - dx)) * (stx - stp);
      if (std::abs(stpc - stp) > std::abs(stpq - stp)) {
        stpf = stpc;
      } else {
        stpf = stpq;
      }
      brackt = true;
      // Third case: A lower function value, derivatives of the same sign,
      // and the magnitude of the derivative decreases.
    } else if (std::abs(dp) < std::abs(dx)) {
      // The cubic step is computed only if the cubic tends to infinity
      // in the direction of the step or if the minimum of the cubic
      // is beyond stp. Otherwise the cubic step is defined to be the
      // secant step.
      theta = three * (fx - fp) / (stp - stx) + dx + dp;
      s = std::max(std::abs(theta), std::abs(dx));
      s = std::max(s, std::abs(dp));
      // The case gamma = 0 only arises if the cubic does not tend
      // to infinity in the direction of the step.
      gamma = s * std::sqrt(std::max(0.0, std::pow((theta / s), 2.0) -
                                              (dx / s) * (dp / s)));
      if (stp > stx) {
        gamma = -gamma;
      }
      p = (gamma - dp) + theta;
      q = (gamma + (dx - dp)) + gamma;
      r = p / q;
      if (r < 0.0 && gamma != 0.0) {
        stpc = stp + r * (stx - stp);
      } else if (stp > stx) {
        stpc = stpmax;
      } else {
        stpc = stpmin;
      }
      stpq = stp + (dp / (dp - dx)) * (stx - stp);
      if (brackt) {
        // A minimizer has been bracketed. If the cubic step is
        // closer to stp than the secant step, the cubic step is
        // taken, otherwise the secant step is taken.
        if (std::abs(stpc - stp) < std::abs(stpq - stp)) {
          stpf = stpc;
        } else {
          stpf = stpq;
        }
        if (stp > stx) {
          stpf = std::min(stp + p66 * (sty - stp), stpf);
        } else {
          stpf = std::max(stp + p66 * (sty - stp), stpf);
        }
      } else {
        // A minimizer has not been bracketed. If the cubic step is
        // farther from stp than the secant step, the cubic step is
        // taken, otherwise the secant step is taken.
        if (std::abs(stpc - stp) > std::abs(stpq - stp)) {
          stpf = stpc;
        } else {
          stpf = stpq;
        }
        stpf = std::min(stpmax, stpf);
        stpf = std::max(stpmin, stpf);
      }
      // Fourth case: A lower function value, derivatives of the same
      // sign and the magnitude of the derivative does not decrease. If
      // the minimum is not bracketed, the step is either stpmin or
      // stpmax, otherwise the cubic step is taken.
    } else {
      if (brackt) {
        theta = three * (fp - fy) / (sty - stp) + dy + dp;
        s = std::max(std::abs(theta), std::abs(dy));
        s = std::max(s, std::abs(dp));
        gamma = s * std::sqrt(std::pow((theta / s), 2.0) - (dy / s) * (dp / s));
        if (stp > sty) {
          gamma = -gamma;
        }
        p = (gamma - dp) + theta;
        q = ((gamma - dp) + gamma) + dy;
        r = p / q;
        stpc = stp + r * (sty - stp);
        stpf = stpc;
      } else if (stp > stx) {
        stpf = stpmax;
      } else {
        stpf = stpmin;
      }
    }
    // Update the interval which contains a minimizer.
    if (fp > fx) {
      sty = stp;
      fy = fp;
      dy = dp;
    } else {
      if (sgnd < 0.0) {
        sty = stx;
        fy = fx;
        dy = dx;
      }
      stx = stp;
      fx = fp;
      dx = dp;
    }
    // Compute the new step.
    stp = stpf;
  }

  /*
  Scalar line search in direction pk

  Parameters
  ----------
  xk : array
      Current point
  pk : array
      Search direction
  gfk : array
      Gradient of f at point xk
  old_fval : double
      Value of f at point xk
  old_old_fval : double
      Value of f at point preceding xk

  Returns
  -------
  alpha : double
      Step size, trow if no suitable step was found
  Updates
  -------
  old_fval : double
      fval at step size
  old_old_fval : double
      initial old_fval
  gfk : array
      Gradient of f at the final point
  gfkp1 : array
      Gradient of f at the final point
  */
  bool _line_search(double &stp, const Eigen::VectorXd &xk,
                    const Eigen::VectorXd &pk, const Eigen::VectorXd &gfk,
                    Eigen::VectorXd &gfkp1, double &old_fval,
                    double &old_old_fval) {

    auto phi = [&](double s) { return _f(xk + s * pk); };
    auto derphi = [&](double s) {
      gfkp1 = _fprime(xk + s * pk);
      return gfkp1.dot(pk);
    };

    /*
    Scalar function search for alpha that satisfies strong Wolfe conditions.
    alpha > 0 is assumed to be a descent direction.
    */

    double c1 = 1e-4;
    double c2 = 0.9;
    double stpmax = 1e100;
    double stpmin = 1e-100;
    double xtol = 1e-14;

    double phi0 = old_fval;
    double old_phi0 = old_old_fval;
    old_old_fval = phi0;
    double derphi0 = gfk.dot(pk);
    if (derphi0 != 0.0) {
      stp = std::min(1.0, 1.01 * 2 * (phi0 - old_phi0) / derphi0);
      if (stp < 0.0) {
        stp = 1.0;
      }
    } else {
      stp = 1.0;
    }

    double f = phi0;
    double g = derphi0;
    double ftol = c1;
    double gtol = c2;

    // fortran initialization
    unsigned int stage = 1;
    double xtrapu = 4.0;
    double xtrapl = 0.66;
    double p66 = 0.66;
    double p5 = 0.5;
    bool brackt = false;
    double finit = phi0;
    double ginit = derphi0;
    double gtest = ftol * ginit;
    double width = stpmax - stpmin;
    double width1 = width / p5;
    // The variables stx, fx, gx contain the values of the step,
    // function, and derivative at the best step.
    // The variables sty, fy, gy contain the value of the step,
    // function, and derivative at sty.
    // The variables stp, f, g contain the values of the step,
    // function, and derivative at stp.
    double stx = 0;
    double sty = 0;
    double fx = finit;
    double fy = finit;
    double gx = ginit;
    double gy = ginit;
    double stmin = 0;
    double stmax = stp + xtrapu * stp;
    double ftest, fm, fxm, fym, gm, gxm, gym;
    // used for debugging fortran code
    // double res[] = {ginit, gtest, gx,    gy,    finit, fx,    fy,
    //                stx,   sty,   stmin, stmax, width, width1};
    unsigned int maxiter = 5;
    for (unsigned int i = 0; i < maxiter; ++i) {
      // Obtain another function and derivative.
      f = phi(stp);
      g = derphi(stp);
      old_fval = f;
      // If psi(stp) <= 0 and f'(stp) >= 0 for some step, then the
      // algorithm enters the second stage.
      ftest = finit + stp * gtest;
      if (stage == 1 && f <= ftest && g >= 0.0) {
        stage = 2;
      }
      // Test for warnings
      if (brackt && (stp <= stmin || stp >= stmax)) {
        return false;
        // throw std::runtime_error("WARNING: ROUNDING ERRORS PREVENT
        // PROGRESS");
      }
      if (brackt && stmax - stmin <= xtol * stmax) {
        return false;
        // throw std::runtime_error("WARNING: XTOL TEST SATISFIED");
      }
      if (stp == stpmax && f <= ftest && g <= gtest) {
        return false;
        // throw std::runtime_error("WARNING: STP = STPMAX");
      }
      if (stp == stpmin && (f > ftest || g >= gtest)) {
        return false;
        // throw std::runtime_error("WARNING: STP = STPMIN");
      }
      // Test for convergence
      if (f <= ftest && std::abs(g) <= gtol * (-ginit)) {
        // Update f values
        return true;
      }
      // A modified function is used to predict the step during the
      // first stage if a lower function value has been obtained but
      // the decrease is not sufficient.
      if (stage == 1 && f <= fx && f > ftest) {
        // Define the modified function and derivative values
        fm = f - stp * gtest;
        fxm = fx - stx * gtest;
        fym = fy - sty * gtest;
        gm = g - gtest;
        gxm = gx - gtest;
        gym = gy - gtest;
        // Call dcstep to update stx, sty, and to compute the new step.
        dcstep(stx, fxm, gxm, sty, fym, gym, stp, fm, gm, brackt, stmin, stmax);
        // Reset the function and derivative values for f.
        fx = fxm + stx * gtest;
        fy = fym + sty * gtest;
        gx = gxm + gtest;
        gy = gym + gtest;
      } else {
        // Call dcstep to update stx, sty, and to compute the new step.
        dcstep(stx, fx, gx, sty, fy, gy, stp, f, g, brackt, stmin, stmax);
      }
      // Decide if a bisection step is needed.
      if (brackt) {
        if (std::abs(sty - stx) >= p66 * width1) {
          stp = stx + p5 * (sty - stx);
        }
        width1 = width;
        width = std::abs(sty - stx);
      }
      // Set the minimum and maximum steps allowed for stp.
      if (brackt) {
        stmin = std::min(stx, sty);
        stmax = std::max(stx, sty);
      } else {
        stmin = stp + xtrapl * (stp - stx);
        stmax = stp + xtrapu * (stp - stx);
      }

      // Force the step to be within the bounds stpmax and stpmin.
      stp = std::max(stp, stpmin);
      stp = std::min(stp, stpmax);
      // If further progress is not possible, let stp be the best
      // point obtained during the search.
      if ((brackt && (stp <= stmin || stp >= stmax)) ||
          (brackt && stmax - stmin <= xtol * stmax)) {
        stp = stx;
      }
    }
    // throw if no result after maxiter
    return false;
    // throw std::runtime_error("line search did not converge");
  }

  // wrapper around the objective function counting calls
  double _f(const Eigen::VectorXd &xk) {
    ++_nfev;
    return _objective(xk);
  }
  // f(x) - objective function to minimize
  T _objective;
  // f'(x) - numerical approximation of gradient by forward finite differences
  Eigen::VectorXd _fprime(const Eigen::VectorXd &xk) {
    ++_njev;
    Eigen::VectorXd grad = Eigen::VectorXd::Zero(_n_params);
    Eigen::VectorXd dx = Eigen::VectorXd::Zero(_n_params);
    double yk = _f(xk);
    for (unsigned int k = 0; k < _n_params; ++k) {
      dx(k) = epsilon;
      grad(k) = (_f(xk + dx) - yk) / epsilon;
      dx(k) = 0.0;
    }
    return grad;
  }

  // dimension of input vector
  unsigned int _n_params;

  // maximum iteration for convergence
  unsigned int _max_iter;

  // initial vector
  Eigen::VectorXd _x0;

  // minimum infinite norm of gradient
  double _gtol;

  // number of evaluations of the objective function
  unsigned int _nfev;

  // number of evaluations of the gradient of the objective function
  unsigned int _njev;

  // return all steps
  bool _retpath;
  std::vector<Eigen::VectorXd> _path;
};

} // namespace bfgs