#include <igclib/pointcollection.hpp>
#include <igclib/xc_optimization.hpp>
#include <vector>

int heuristic::broken_line(const PointCollection &points) {
  const int n_approx = 50;
  const int step_ratio = points.size() / n_approx;

  std::vector<int> previous_distances(n_approx);
  std::vector<int> next_distances(n_approx);

  for (int step = 0; step < 5; ++step) {
    for (std::size_t i = 0; i < n_approx; ++i) {
      int max_distance = 0;
      for (std::size_t j = 0; j < i; ++j) {
        int candidate = previous_distances[j] +
                        points[i * step_ratio].distance(points[j * step_ratio]);
        if (candidate > max_distance) {
          max_distance = candidate;
        }
      }
      next_distances[i] = max_distance;
    }
    previous_distances = next_distances;
  }

  return *std::max_element(next_distances.begin(), next_distances.end());
}