#include <stdio.h>
#include <float.h>

int main(void)
    {
    float ef = 1.0F, rf = 1.0F;

    while (1)
        {
        float af = ef + rf;

        if (af == rf)
            {
            break;
            }
        else
            {
            ef /= 2.0F;
            }
        }

    printf("Epsilon (float): %.50f\n", ef * 2.0F); // Multiple by 2.0 (float) back
    printf("FLT_EPSILON = %.50f\n", FLT_EPSILON);
    printf("Difference: %.50f\n", FLT_EPSILON - ef * 2.0F);

    double ed = 1.0, rd = 1.0;

    while (1)
        {
        double ad = ed + rd;

        if (ad == rd)
            {
            break;
            }
        else
            {
            ed /= 2.0;
            }
        }

    printf("Epsilon (double): %.50f\n", ed * 2.0); // Multiple by 2.0 (double) back
    printf("DBL_EPSILON = %.50f\n", DBL_EPSILON);
    printf("Difference: %.50f\n", DBL_EPSILON - ed * 2.0);

    return 0;
    }