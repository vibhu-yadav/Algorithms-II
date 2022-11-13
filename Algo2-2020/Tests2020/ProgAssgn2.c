#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define RESOLUTION 10000
#define ON 0
#define LEFT 1
#define RIGHT -1
#define UPPER 1
#define LOWER 2

typedef struct {
   double x;
   double y;
} point;

typedef struct {
   point center;
   double start;
   double end;
} arc;

typedef struct {
   point start;
   point end;
} tangent;

double getrandval ( double r )
{
   double z;

   do {
      z = (double)rand() / (double)RAND_MAX;
   } while ((z < r) || (z > 1-r));
   return z;
}

point *gencircles ( int n, double r )
{
   int i;
   point *C;

   C = (point *)malloc(n * sizeof(point));
   for (i=0; i<n; ++i) {
      C[i].x = getrandval(r);
      C[i].y = getrandval(r);
      printf("%17.15lf %17.15lf\n", C[i].x, C[i].y);
   }
   return C;
}

FILE *openopfile ( )
{
   FILE *fp;

   fp = (FILE *)fopen("output.fig", "w");
   fprintf(fp, "#FIG 3.2  Produced by AD\n");
   fprintf(fp, "Landscape\nCenter\nMetric\nA4\n100.00\nSingle\n-2\n1200 2\n");
   return fp;
}

void sortcircles ( point *C, int n )
{
   int m, i, j, k;
   point *L, *R;

   if (n <= 1) return;
   m = n / 2;
   L = (point *)malloc(m * sizeof(point));
   for (i=0; i<m; ++i) L[i] = C[i];
   R = (point *)malloc((n-m) * sizeof(point));
   for (i=m; i<n; ++i) R[i-m] = C[i];
   sortcircles(L,m);
   sortcircles(R,n-m);
   i = j = k = 0;
   while ((i < m) || (j < (n-m))) {
      if (i >= m) C[k] = R[j++];
      else if (j >= (n-m)) C[k] = L[i++];
      else if (L[i].x <= R[j].x) C[k] = L[i++];
      else C[k] = R[j++];
      ++k;
   }
   free(L);
   free(R);
}

int side ( point P1, point P2, point P )
{
   double d;

   d = (P2.y - P.y) * P1.x + (P.y - P1.y) * P2.x + (P1.y - P2.y) * P.x;
   if (d == 0) return ON;
   if (d > 0) return LEFT;
   return RIGHT;
}

int Grahamscan ( point *C, int n, point *H, int which )
{
   int top, i;

   if (n == 1) { H[0] = C[0]; return 0; }
   if (which == UPPER) {
      H[0] = C[0];
      H[1] = C[1];
      top = 1;
      for (i=2; i<n; ++i) {
         while (1) {
            if (top == 0) break;
            if (side(H[top-1],H[top],C[i]) == RIGHT) break;
            --top;
         }
         H[++top] = C[i];
      }
   } else {
      H[0] = C[n-1];
      H[1] = C[n-2];
      top = 1;
      for (i=n-3; i>=0; --i) {
         while (1) {
            if (top == 0) break;
            if (side(H[top-1],H[top],C[i]) == RIGHT) break;
            --top;
         }
         H[++top] = C[i];
      }
   }
   return top;
}

void outerhull ( double r, point *UH, int uhsize, point *LH, int lhsize, tangent *T, arc *A )
{
   int i, t, a;
   double theta, prev;

   prev = M_PI; t = a = 0;
   for (i=0; i<uhsize; ++i) {
      theta = atan2(UH[i+1].y - UH[i].y, UH[i+1].x - UH[i].x);
      theta += M_PI_2;

      T[t].start.x = UH[i].x + r * cos(theta);
      T[t].start.y = UH[i].y + r * sin(theta);
      T[t].end.x = UH[i+1].x + r * cos(theta);
      T[t].end.y = UH[i+1].y + r * sin(theta);
      ++t;

      A[a].center = UH[i];
      A[a].start = prev;
      A[a].end = theta;
      ++a;

      prev = theta;
   }
   theta = 0;
   A[a].center = UH[uhsize];
   A[a].start = prev;
   A[a].end = theta;
   ++a;

   prev = 0;
   for (i=0; i<lhsize; ++i) {
      theta = atan2(LH[i+1].y - LH[i].y, LH[i+1].x - LH[i].x);
      theta += M_PI_2;
      if (theta > 0) theta -= 2 * M_PI;

      T[t].start.x = LH[i].x + r * cos(theta);
      T[t].start.y = LH[i].y + r * sin(theta);
      T[t].end.x = LH[i+1].x + r * cos(theta);
      T[t].end.y = LH[i+1].y + r * sin(theta);
      ++t;

      A[a].center = LH[i];
      A[a].start = prev;
      A[a].end = theta;
      ++a;

      prev = theta;
   }
   theta = -M_PI;
   A[a].center = LH[lhsize];
   A[a].start = prev;
   A[a].end = theta;
   ++a;
}

void prnouterhull ( tangent *T, arc *A, int uhsize, int lhsize )
{
   int t, a, i;

   t = a = 0;

   printf("--- Upper section\n");
   for (i=0; i<uhsize; ++i) {
      printf("    Arc     : (%17.15lf,%17.15lf) From %17.15lf to %17.15lf\n",
                            A[a].center.x, A[a].center.y, A[a].start, A[a].end);
      ++a;
      printf("    Tangent : From (%17.15lf,%17.15lf) to (%17.15lf,%17.15lf)\n",
                            T[t].start.x, T[t].start.y, T[t].end.x, T[t].end.y);
      ++t;
   }
   printf("    Arc     : (%17.15lf,%17.15lf) From %17.15lf to %17.15lf\n",
                         A[a].center.x, A[a].center.y, A[a].start, A[a].end);
   ++a;

   printf("--- Lower section\n");
   for (i=0; i<lhsize; ++i) {
      printf("    Arc     : (%17.15lf,%17.15lf) From %17.15lf to %17.15lf\n",
                            A[a].center.x, A[a].center.y, A[a].start, A[a].end);
      ++a;
      printf("    Tangent : From (%17.15lf,%17.15lf) to (%17.15lf,%17.15lf)\n",
                            T[t].start.x, T[t].start.y, T[t].end.x, T[t].end.y);
      ++t;
   }
   printf("    Arc     : (%17.15lf,%17.15lf) From %17.15lf to %17.15lf\n",
                         A[a].center.x, A[a].center.y, A[a].start, A[a].end);
}

void exportaxes ( FILE *fp )
{
   fprintf(fp, "2 1 0 2 2 7 60 -1 -1 0.000 0 0 -1 0 0 5\n\t");
   fprintf(fp, " 0 0");
   fprintf(fp, " 0 %d", RESOLUTION);
   fprintf(fp, " %d %d", RESOLUTION, RESOLUTION);
   fprintf(fp, " %d 0", RESOLUTION);
   fprintf(fp, " 0 0\n");
}

void exportcircle ( FILE *fp, point center, double radius )
{
   int approx;

   fprintf(fp, "1 3 0 4 1 7 50 -1 -1 0.000 1 0.0000 ");
   approx = (int)round((center.x) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((1 - center.y) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round(radius * RESOLUTION); fprintf(fp, "%d %d ", approx, approx);
   approx = (int)round((center.x) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((1 - center.y) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((radius + center.x) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((1 - center.y) * RESOLUTION); fprintf(fp, "%d\n", approx);

   fprintf(fp, "1 3 0 4 0 7 30 -1 0 0.000 1 0.0000 ");
   approx = (int)round((center.x) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((1 - center.y) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round(0.005 * RESOLUTION); fprintf(fp, "%d %d ", approx, approx);
   approx = (int)round((center.x) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((1 - center.y) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((0.005 + center.x) * RESOLUTION); fprintf(fp, "%d ", approx);
   approx = (int)round((1 - center.y) * RESOLUTION); fprintf(fp, "%d\n", approx);
}

void exporthull ( FILE *fp, point *UH, int uhsize, point *LH, int lhsize )
{
   int i, approx;

   fprintf(fp, "2 1 0 4 4 7 40 -1 -1 0.000 0 0 -1 0 0 %d\n\t", uhsize + lhsize + 1);
   for (i=0; i<=uhsize; ++i) {
      approx = (int)round((UH[i].x) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - UH[i].y) * RESOLUTION); fprintf(fp, " %d", approx);
   }
   for (i=1; i<=lhsize; ++i) {
      approx = (int)round((LH[i].x) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - LH[i].y) * RESOLUTION); fprintf(fp, " %d", approx);
   }
   fprintf(fp, "\n");
}

void exporttangents ( FILE *fp, tangent *T, int size )
{
   int i, approx;

   for (i=0; i<size; ++i) {
      fprintf(fp, "2 1 0 4 31 7 40 -1 -1 0.000 0 0 -1 0 0 2\n\t");
      approx = (int)round((T[i].start.x) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - T[i].start.y) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((T[i].end.x) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - T[i].end.y) * RESOLUTION); fprintf(fp, " %d\n", approx);
   }
}

void exportarcs ( FILE *fp, arc *A, int size, double r )
{
   int i, approx;
   double theta;

   for (i=0; i<size; ++i) {
      theta = (A[i].start + A[i].end) / 2;
      fprintf(fp, "5 1 0 4 18 7 40 -1 -1 0.000 0 0 0 0");
      approx = (int)round((A[i].center.x) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - A[i].center.y) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((A[i].center.x + r * cos(A[i].start)) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - (A[i].center.y + r * sin(A[i].start))) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((A[i].center.x + r * cos(theta)) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - (A[i].center.y + r * sin(theta))) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((A[i].center.x + r * cos(A[i].end)) * RESOLUTION); fprintf(fp, " %d", approx);
      approx = (int)round((1 - (A[i].center.y + r * sin(A[i].end))) * RESOLUTION); fprintf(fp, " %d\n", approx);
   }
}

int main ( int argc, char *argv[] )
{
   int n, i, uhsize, lhsize;
   double r, s;
   point *C, *UH, *LH;
   tangent *T;
   arc *A;
   FILE *fp;

   srand((unsigned int)time(NULL));
   // srand(0);
   n = (argc > 1) ? atoi(argv[1]) : 10;
   printf("%d\n", n);
   if (n <= 4) s = sqrt(5+rand()%(21));
   else s = sqrt(n+rand()%(7*n));
   r = 1 / s;
   printf("%17.15lf\n", r);
   C = gencircles(n,r);
   sortcircles(C,n);
   printf("\n+++ Circles after sorting\n");
   for (i=0; i<n; ++i) printf("    %17.15lf %17.15lf\n", C[i].x, C[i].y);

   fp = openopfile();
   exportaxes(fp);
   for (i=0; i<n; ++i) exportcircle(fp,C[i],r);

   UH = (point *)malloc(n * sizeof(point));
   uhsize = Grahamscan(C,n,UH,UPPER);
   printf("\n+++ Upper hull\n");
   for (i=0; i<=uhsize; ++i) printf("    %17.15lf %17.15lf\n", UH[i].x, UH[i].y);

   LH = (point *)malloc(n * sizeof(point));
   lhsize = Grahamscan(C,n,LH,LOWER);
   printf("\n+++ Lower hull\n");
   for (i=0; i<=lhsize; ++i) printf("    %17.15lf %17.15lf\n", LH[i].x, LH[i].y);

   exporthull(fp,UH,uhsize,LH,lhsize);

   T = (tangent *)malloc((uhsize+lhsize)*sizeof(tangent));
   A = (arc *)malloc((uhsize+lhsize+2)*sizeof(arc));
   outerhull(r,UH,uhsize,LH,lhsize,T,A);
   printf("\n+++ The outer hull\n");
   prnouterhull(T,A,uhsize,lhsize);

   exporttangents(fp, T, uhsize+lhsize);
   exportarcs(fp, A, uhsize+lhsize+2, r);

   fclose(fp);
   exit(0);
}
