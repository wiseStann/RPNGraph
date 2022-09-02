#include <math.h>
#include "graph.h"
#include "c_type.h"
#include "io.h"
#include "op_stack.h"
#include "RPN.h"


void generate_graph(point *points, token **RPN_tokens, int tokens_size, int *flag) {
  int idx = 0;
  *flag = 1;
  for (double x = 0; x < 4 * M_PI && *flag; x += (double)(4 * M_PI / 79)) {
    double function = evaluate_RPN(RPN_tokens, tokens_size, x, flag);
    if (*flag) {
      point next_point = {x, function};
      points[idx] = next_point;
      idx++;
    }
  }
}

void show_graph(const point *points) {
    int field[FIELD_HEIGHT][FIELD_WIDTH] = {0};
    for (int i = 0; i < FIELD_WIDTH; i++) {
        const point curr_point = points[i];
        field[FIELD_HEIGHT - curr_point.y - 1 - 13][curr_point.x] = 1;
    }
    print_field(field);
}

int main() {
  start_plotting();
  return 0;
}