
void increment()
{
  int n = 100;
  double b[n][n][n];
  int i;
  int j;
  int k;
  for (k = 0; k < n; ++k) {
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
        b[i + 1][(j + k) + 1][k + 1] = (b[i][j][k] + 1);
      }
    }
  }
}
