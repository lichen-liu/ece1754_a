void jacobi() {
  int n=100;
  int m=100;
  int tmax = 10000;

  double in[n][m];
  double out[n][m];

  int t,i,j;

  for (t=0;t<tmax;++t) {
      for (i=0;i<n;i++) {
          for (j=0;j<m;j++) {
             out[i][j] = (in[i-1][j]+in[i+1][j]+in[i][j-1]+in[i][j+1])/4.0;
          }
      }

      for (i=0;i<n;i++) {
          for (j=0;j<m;j++) {
             in[i][j] = out[i][j];
          }
      }
  }
}

