#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * edges,
          struct matrix * polygons,
          screen s
Returns:
Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         sphere: add a sphere to the POLYGON matrix -
                 takes 4 arguemnts (cx, cy, cz, r)
         torus: add a torus to the POLYGON matrix -
                takes 5 arguemnts (cx, cy, cz, r1, r2)
         box: add a rectangular prism to the POLYGON matrix -
              takes 6 arguemnts (x, y, z, width, height, depth)
         clear: clears the edge and POLYGON matrices
         circle: add a circle to the edge matrix -
                 takes 4 arguments (cx, cy, cz, r)
         hermite: add a hermite curve to the edge matrix -
                  takes 8 arguments (x0, y0, x1, y1, rx0, ry0, rx1, ry1)
         bezier: add a bezier curve to the edge matrix -
                 takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         move: create a translation matrix,
               then multiply the transform matrix by the translation matrix -
               takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
                and to the POLYGON matrix
         display: clear the screen, then
                  draw the lines of the edge and POLYGON matrices to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge and POLYGON matrces to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing
See the file script for an example of the file format
IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  struct matrix * polygons,
                  screen s) {

  FILE *f;
  char line[255];
  clear_screen(s);
  color c;
  c.red = 0;
  c.green = 255;
  c.blue = 255;

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");

  while ( fgets(line, sizeof(line), f) != NULL ) {
    line[strlen(line)-1]='\0';
    //printf(":%s:\n",line);

    double xvals[4];
    double yvals[4];
    double zvals[4];
    struct matrix *tmp;
    double r, r1;
    double theta;
    char axis;
    int type;
    int steps_3d = 20;
    int steps_2d = 100;

    if ( strncmp(line, "box", strlen(line)) == 0 ) {
      fgets(line, sizeof(line), f);
      //printf("BOX\t%s", line);

      sscanf(line, "%lf %lf %lf %lf %lf %lf",
       xvals, yvals, zvals,
       xvals+1, yvals+1, zvals+1);
      add_box(polygons, xvals[0], yvals[0], zvals[0],
        xvals[1], yvals[1], zvals[1]);
    }//end of box

    else if ( strncmp(line, "sphere", strlen(line)) == 0 ) {
      fgets(line, sizeof(line), f);
      //printf("SPHERE\t%s", line);

      sscanf(line, "%lf %lf %lf %lf",
       xvals, yvals, zvals, &r);
      add_sphere(polygons, xvals[0], yvals[0], zvals[0], r, steps_3d);
    }//end of sphere

    else if ( strncmp(line, "torus", strlen(line)) == 0 ) {
      fgets(line, sizeof(line), f);
      //printf("torus\t%s", line);

      sscanf(line, "%lf %lf %lf %lf %lf",
       xvals, yvals, zvals, &r, &r1);
      add_torus(polygons, xvals[0], yvals[0], zvals[0], r, r1, steps_3d);
    }//end of torus

    else if ( strncmp(line, "circle", strlen(line)) == 0 ) {
      fgets(line, sizeof(line), f);
      //printf("CIRCLE\t%s", line);

      sscanf(line, "%lf %lf %lf %lf",
             xvals, yvals, zvals, &r);
      add_circle( edges, xvals[0], yvals[0], zvals[0], r, steps_2d);
    }//end of circle

    else if ( strncmp(line, "hermite", strlen(line)) == 0 ||
              strncmp(line, "bezier", strlen(line)) == 0 ) {
      if (strncmp(line, "hermite", strlen(line)) == 0 )
        type = HERMITE;
      else
        type = BEZIER;
      
      fgets(line, sizeof(line), f);
      //printf("CURVE\t%s", line);

          sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                 xvals, yvals, xvals+1, yvals+1,
                 xvals+2, yvals+2, xvals+3, yvals+3);
          /* printf("%lf %lf %lf %lf %lf %lf %lf %lf\n", */
          /*       xvals[0], yvals[0], */
          /*       xvals[1], yvals[1], */
          /*       xvals[2], yvals[2], */
          /*       xvals[3], yvals[3]); */
      
          //printf("%d\n", type);
          add_curve( edges, xvals[0], yvals[0], xvals[1], yvals[1],
                     xvals[2], yvals[2], xvals[3], yvals[3], steps_2d, type);
        }//end of curve
        else if ( strncmp(line, "line", strlen(line)) == 0 ) {
          fgets(line, sizeof(line), f);
          //printf("LINE\t%s", line);

          sscanf(line, "%lf %lf %lf %lf %lf %lf",
                 xvals, yvals, zvals,
                 xvals+1, yvals+1, zvals+1);
          /*printf("%lf %lf %lf %lf %lf %lf",
            xvals[0], yvals[0], zvals[0],
            xvals[1], yvals[1], zvals[1]) */
          add_edge(edges, xvals[0], yvals[0], zvals[0],
                   xvals[1], yvals[1], zvals[1]);
        }//end line

        else if ( strncmp(line, "scale", strlen(line)) == 0 ) {
          fgets(line, sizeof(line), f);
          //printf("SCALE\t%s", line);
          sscanf(line, "%lf %lf %lf",
                 xvals, yvals, zvals);
          /* printf("%lf %lf %lf\n", */
          /* xvals[0], yvals[0], zvals[0]); */
      tmp = make_scale( xvals[0], yvals[0], zvals[0]);
      matrix_mult(tmp, transform);
    }//end scale

    else if ( strncmp(line, "move", strlen(line)) == 0 ) {
      fgets(line, sizeof(line), f);
      //printf("MOVE\t%s", line);
      sscanf(line, "%lf %lf %lf",
             xvals, yvals, zvals);
      /* printf("%lf %lf %lf\n", */
      /* xvals[0], yvals[0], zvals[0]); */
      tmp = make_translate( xvals[0], yvals[0], zvals[0]);
      matrix_mult(tmp, transform);
    }//end translate

    else if ( strncmp(line, "rotate", strlen(line)) == 0 ) {
      fgets(line, sizeof(line), f);
      //printf("Rotate\t%s", line);
      sscanf(line, "%c %lf",
             &axis, &theta);
      /* printf("%c %lf\n", */
      /*axis, theta); */
      theta = theta * (M_PI / 180);
      if ( axis == 'x' )
        tmp = make_rotX( theta );
      else if ( axis == 'y' )
        tmp = make_rotY( theta );
      else
        tmp = make_rotZ( theta );

      matrix_mult(tmp, transform);
    }//end rotate


    else if ( strncmp(line, "clear", strlen(line)) == 0 ) {
      //printf("clear\t%s", line);
      edges->lastcol = 0;
      polygons->lastcol = 0;
    }//end clear

    else if ( strncmp(line, "ident", strlen(line)) == 0 ) {
      //printf("IDENT\t%s", line);
      ident(transform);
    }//end ident

    else if ( strncmp(line, "apply", strlen(line)) == 0 ) {
      //printf("APPLY\t%s", line);
      matrix_mult(transform, edges);
      matrix_mult(transform,polygons);
    }//end apply

    else if ( strncmp(line, "display", strlen(line)) == 0 ) {
      //printf("DISPLAY\t%s", line);
      clear_screen(s);
      draw_lines(edges, s, c);
      draw_polygons(polygons,s,c);
      display( s );
    }//end display

    else if ( strncmp(line, "save", strlen(line)) == 0 ) {
      //printf("SAVE\t%s", line);
      fgets(line, sizeof(line), f);
      *strchr(line, '\n') = 0;
      //printf("name: %s\n", line);
      clear_screen(s);
      draw_lines(edges, s, c);
      draw_polygons(polygons,s,c);
      save_extension(s, line);
    }//end save
  }
}
