/* open source: found at cis.k.hosei.ac.jp/~wakahara/mypgm.h on August 14th, 2013*/

/* pgm file IO headerfile ------ mypgm.h */

/* Constant declaration */
#define MAX_IMAGESIZE  1024
#define MAX_BRIGHTNESS  255 /* Maximum gray level */
#define GRAYLEVEL       256 /* No. of gray levels */
#define MAX_FILENAME    256 /* Filename length limit */
#define MAX_BUFFERSIZE  256
/* B is the number of "b"its I keep for the indexing */
#define B_default         4 /* 0-8*/

/* Global constant declaration */
/* Image storage arrays */
unsigned char image1[MAX_IMAGESIZE][MAX_IMAGESIZE],
              image2[MAX_IMAGESIZE][MAX_IMAGESIZE];
int x_size1, y_size1, /* width & height of image1*/
    x_size2, y_size2; /* width & height of image2 */

/* Prototype declaration of functions */
void load_image_file(char *); /* image input */
void save_image_file(char *); /* image output*/

/* Main body of functions */

void load_image_file(char *filename)
/* Input of header & body information of pgm file */
/* for image1[ ][ ]Cx_size1Cy_size1 */
{
  char buffer[MAX_BUFFERSIZE];
  FILE *fp; /* File pointer */
  int max_gray; /* Maximum gray level */
  int x, y; /* Loop variable */
  
  /* Input file open */
  fp = fopen(filename, "rb");
  if (NULL == fp) {
    printf("     The file doesn't exist!\n\n");
    exit(1);
  }
  /* Check of file-type ---P5 */
  fgets(buffer, MAX_BUFFERSIZE, fp);
  if (buffer[0] != 'P' || buffer[1] != '5') {
    printf("     Mistaken file format, not P5!\n\n");
    exit(1);
  }
  /* input of x_size1, y_size1 */
  x_size1 = 0;
  y_size1 = 0;
  while (x_size1 == 0 || y_size1 == 0) {
    fgets(buffer, MAX_BUFFERSIZE, fp);
    if (buffer[0] != '#') {
      sscanf(buffer, "%d %d", &x_size1, &y_size1);
    }
  }
  /* input of max_gray */
  max_gray = 0;
  while (max_gray == 0) {
    fgets(buffer, MAX_BUFFERSIZE, fp);
    if (buffer[0] != '#') {
      sscanf(buffer, "%d", &max_gray);
    }
  }
  
  /* Display of parameters */
  printf("\nImage width = %d, Image height = %d\n", x_size1, y_size1);
  printf("Maximum gray level = %d\n\n",max_gray);
  
  
  if (x_size1 > MAX_IMAGESIZE || y_size1 > MAX_IMAGESIZE) {
    printf("     Image size exceeds %d x %d\n\n", 
	   MAX_IMAGESIZE, MAX_IMAGESIZE);
    printf("     Please use smaller images!\n\n");
    exit(1);
  }
  if (max_gray != MAX_BRIGHTNESS) {
    printf("     Invalid value of maximum gray level!\n\n");
    exit(1);
  }
  /* Input of image data*/
  for (y = 0; y < y_size1; y++) {
    for (x = 0; x < x_size1; x++) {
      image1[y][x] = (unsigned char)fgetc(fp);
    }
  }
  fclose(fp);
}

void save_image_file(char *filename)
/* Output of image2[ ][ ], x_size2, y_size2 */ 
/* into pgm file with header & body information */
{
  FILE *fp; /* File pointer */
  int x, y; /* Loop variable */
  
  fp = fopen(filename, "wb");
  /* output of pgm file header information */
  fputs("P5\n", fp);
  fputs("# Created by Image Processing\n", fp);
  fprintf(fp, "%d %d\n", x_size2, y_size2);
  fprintf(fp, "%d\n", MAX_BRIGHTNESS);
  /* Output of image data */
  for (y = 0; y < y_size2; y++) {
    for (x = 0; x < x_size2; x++) {
      fputc(image2[y][x], fp);
    }
  }
  fclose(fp);
  //printf("\n-----Image data output OK-----\n\n");
  //printf("-----------------------------------------------------\n\n");
}

