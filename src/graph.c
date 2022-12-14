#include "graph.h"
#include "RPN.h"
#include "io.h"
#include "op_stack.h"
#include <math.h>

void generate_graph(point *points, token **RPN_tokens, int tokens_size,
                    int *flag) {
  int idx = 0, start_idx = 0;
  *flag = 1;
  for (double x = 0; x < 4 * M_PI; x += (double)(4 * M_PI / 80)) {
    double function =
        evaluate_RPN(RPN_tokens, tokens_size, &start_idx, x, flag, 0, 1);
    if (*flag) {
      double sum_coeff = function == 0 ? function : (function > 0 ? -1 : 1);
      point next_point = {x, function * 13 + sum_coeff};
      points[idx++] = next_point;
    } else {
      point next_point = {-1, FIELD_HEIGHT};
      points[idx++] = next_point;
    }
    start_idx = 0;
    *flag = 1;
  }
}

void show_graph(const point *points) {
  int field[FIELD_HEIGHT][FIELD_WIDTH] = {0};
  for (int i = 0; i < FIELD_WIDTH; i++) {
    const point curr_point = points[i];
    int y_idx = (FIELD_HEIGHT - 1) - 12 - curr_point.y;
    if (y_idx >= 0 && y_idx < FIELD_HEIGHT)
      field[y_idx][i] = 1;
  }
  print_field(field);
}

int main() {
  start_plotting();
  return 0;
}
