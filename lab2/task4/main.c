#include "funcs.h"


int main() {
    Point points1[] = {{4, 4},
                       {2, 2},
                       {4, 0}};
    int result1 = is_convex_polygon(3, points1[0], points1[1], points1[2]);
    printf("Actual result: %d\n\n", result1);

    // Example 2: A Concave Quadrilateral
    Point points2[] = {{0,   0},
                       {4,   0},
                       {2,   2},
                       {4.0, 4.0},
                       {0,   4}};
    int result2 = is_convex_polygon(4, points2[0], points2[1], points2[2], points2[3]);
    printf("Example 2: A Concave Quadrilateral\n");
    printf("Expected result: 0 (concave)\n");
    printf("Actual result: %d\n\n", result2);

    // Example 3: A Convex Pentagon
    Point points3[] = {{0, 0},
                       {2, 0},
                       {4, 2},
                       {2, 4},
                       {0, 2}};
    int result3 = is_convex_polygon(5, points3[0], points3[1], points3[2], points3[3], points3[4]);
    printf("Example 3: A Convex Pentagon\n");
    printf("Expected result: 1 (convex)\n");
    printf("Actual result: %d\n\n", result3);

    // Example 4: A Concave Pentagon
    Point points4[] = {{0, 0},
                       {4, 0},
                       {4, 4},
                       {2, 2},
                       {0, 4}};
    int result4 = is_convex_polygon(5, points4[0], points4[1], points4[2], points4[3], points4[4]);
    printf("Example 4: A Concave Pentagon\n");
    printf("Expected result: 0 (concave)\n");
    printf("Actual result: %d\n\n", result4);

    double result;
    Error_codes status = calculate_polynomial(&result, 2.0, 3, 1.0, -2.0, 4.0, 3.0);
    if (status == OK) {
        printf("%.6f\n\n", result);
    } else printf("Error: %d\n", status);


    check_kaprekar_numbers_in_base(16, 5, "0000000000000000FFFF", "AB", "297", "A", "10");
    check_kaprekar_numbers_in_base(10, 4, "000000000000000000045", "9", "297", "10");
    check_kaprekar_numbers_in_base(8, 3, "77777", "111112", "1616");
    printf("\n\n");
    kaprekar_v2(16, 5, "0000000000000000FFFF", "AB", "297", "A", "10");
    kaprekar_v2(10, 4, "000000000000000000045", "9", "297", "10");
    kaprekar_v2(8, 3, "0000000000077777", "111112", "1616");
    return 0;
}