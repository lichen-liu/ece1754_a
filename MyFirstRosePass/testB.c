void sor() {
  int n=100;
  int m=100;
  int tmax = 10000;

  double a[n][m];

  int t,i,j;

  for (t=0;t<tmax;++t) {
      for (i=0;i<n;i++) {
          for (j=0;j<m;j++) {
             a[i][j] = (a[i-1][j]+a[i+1][j]+a[i][j-1]+a[i][j+1])/4.0;
          }
      }

  }
}

