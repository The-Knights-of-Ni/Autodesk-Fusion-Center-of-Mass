#ifndef MATHS
#define MATHS

#include "misc.h"
#include <math.h>

#define pi 3.1415926535897932384626433832795
#define e  2.71828182845904523536
#define sq(a) ((a)*(a))

#pragma pack(push, 16)

struct v2f
{
    union
    {
        struct
        {
            float x;
            float y;
        };
        float data[2];
    };

    inline float &operator[](int a)
    {
        return data[a];
    }

};

//x^(-1/2)
float invSqrt(float a)
{
    int isqrti;
    float isqrt = a;

    isqrti = *(int*) &isqrt;
    isqrti = 0x5f375a86 - (isqrti >> 1); //magic

    isqrt = *(float*) &isqrti;
    isqrt *= (1.5f - (a * 0.5f * isqrt * isqrt));
    isqrt *= (1.5f - (a * 0.5f * isqrt * isqrt)); //second itteration, uncomment if greater accuracy is needed
    return isqrt;
}

/* Start of 2D Vector Functions */
v2f operator+(v2f a, v2f b)
{
    v2f output;
    output.x = a.x + b.x;
    output.y = a.y + b.y;
    return output;
}

v2f operator-(v2f a, v2f b)
{
    v2f output;
    output.x = a.x - b.x;
    output.y = a.y - b.y;
    return output;
}

v2f operator*(v2f a, float b)
{
    v2f output;
    output.x = a.x * b;
    output.y = a.y * b;
    return output;
}

v2f operator*(float b, v2f a)
{
    v2f output;
    output.x = a.x * b;
    output.y = a.y * b;
    return output;
}

v2f operator/(v2f a, float b)//Shouldn't need other configuration, since it makes no sense
{
    v2f output;
    output.x = a.x / b;
    output.y = a.y / b;
    return output;
}

float dot(v2f a, v2f b)
{
    return a.x * b.x + a.y * b.y;
}

float norm(v2f a)
{
    return sqrt(sq(a.x) + sq(a.y));
}

float normSq(v2f a)
{
    return sq(a.x) + sq(a.y);
}

v2f normalize(v2f a)
{
    v2f output;
    output.x = a.x / norm(a);
    output.y = a.y / norm(a);
    return output;
}

v2f complexMultiply(v2f a, v2f b)
{
    v2f output;
    output.x = a.x * b.x - a.y * b.y;
    output.y = a.x * b.y + a.y * b.x;
    return output;
}

v2f project(v2f a, v2f b)
{
    return dot(a, b) / normSq(b) * b;
}

v2f reject(v2f a, v2f b)
{
    return a - project(a, b);
}

v2f normalizeScale(v2f a, float s)
{
    v2f output;
    float normsq = normSq(a);
    output.x = a.x * invSqrt(normsq) * s;
    output.y = a.y * invSqrt(normsq) * s;
    return output;
}
/* End of 2D Vector Functions */

struct v3f
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        struct
        {
            float i;
            float j;
            float k;
        };
        struct //euler angles
        {
            float heading;
            float tilt;
            float roll;
        };
        float data[3];
    };

    inline float &operator[](int a)
    {
        return data[a];
    }
};

v3f operator+(v3f a, v3f b)
{
    v3f output;
    output.x = a.x + b.x;
    output.y = a.y + b.y;
    output.z = a.z + b.z;
    return output;
}

v3f operator-(v3f a, v3f b)
{
    v3f output;
    output.x = a.x - b.x;
    output.y = a.y - b.y;
    output.z = a.z - b.z;
    return output;
}

v3f operator*(v3f a, float b)
{
    v3f output;
    output.x = a.x * b;
    output.y = a.y * b;
    output.z = a.z * b;
    return output;
}

v3f operator*(float b, v3f a)
{
    v3f output;
    output.x = a.x * b;
    output.y = a.y * b;
    output.z = a.z * b;
    return output;
}

v3f operator/(v3f a, float b)//Shouldn't need other configuration, since it makes no sense
{
    v3f output;
    output.x = a.x / b;
    output.y = a.y / b;
    output.z = a.z / b;
    return output;
}


#define op_set(op, type_a, type_b)              \
    v3f operator op=(type_a & a, type_b b)      \
    {                                           \
        return a = a op b;                      \
    }
    
    op_set(+, v3f, v3f)
    op_set(-, v3f, v3f)
    op_set(*, v3f, float)
    op_set(/, v3f, float)
    
float dot(v3f a, v3f b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

v3f cross(v3f a, v3f b)
{
    v3f output;
    output.x = a.y*b.z - b.y*a.z;
    output.y = -a.x*b.z + b.x*a.z;
    output.z = a.x*b.y - b.x*a.y;
    return output;
}

v3f hadamard(v3f a, v3f b)
{
    v3f output;
    output.x = a.x*b.x;
    output.y = a.y*b.y;
    output.z = a.z*b.z;
    return output;
}

float norm(v3f a)
{
    return sqrt(sq(a.x) + sq(a.y) + sq(a.z));
}

float normSq(v3f a)
{
    return sq(a.x) + sq(a.y)  + sq(a.z);
}

v3f normalize(v3f a)
{
    v3f output;
    output.x = a.x / norm(a);
    output.y = a.y / norm(a);
    output.z = a.z / norm(a);
    return output;
}

v3f project(v3f a, v3f b)
{
    return dot(a, b)/normSq(b) * b;
}

v3f reject(v3f a, v3f b)
{
    return a - project(a, b);
}

float scalarTripleProduct(v3f a, v3f b, v3f c)
{
    return dot(a, cross(b, c));
}

v3f normalizeScale(v3f a, float s)
{
    v3f output;
    float normsq = normSq(a);
    output.x = a.x * invSqrt(normsq) * s;
    output.y = a.y * invSqrt(normsq) * s;
    output.z = a.z * invSqrt(normsq) * s;
    return output;
}
/* End of 3D Vector Functions */
struct v4f
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        struct
        { //quaternion notation
            float i;
            float j;
            float k;
            float r;
        };
        float data[4];
    };

    inline float &operator[](int a)
    {
        return data[a];
    }
};

struct v5f
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
            float v;
        };
        float data[5];
    };
    
    inline float & operator[](int a)
    {
        return data[a];
    }
};
/* Start of 5D Vector Functions */
v5f operator*(v5f a, float b)
{
    v5f output;
    output[0] = a[0] * b;
    output[1] = a[1] * b;
    output[2] = a[2] * b;
    output[3] = a[3] * b;
    output[4] = a[4] * b;
    return output;
}

v5f operator-(v5f a, v5f b)
{
    v5f output;
    output[0] = a[0] * b[0];
    output[1] = a[1] * b[1];
    output[2] = a[2] * b[2];
    output[3] = a[3] * b[3];
    output[4] = a[4] * b[4];
    return output;
}

/* End of 5D Vector Functions */

struct m3x3f
{
    union
    {
        float data[9];
        struct
        {
            v3f r0;
            v3f r1;
            v3f r2;
        };
        v3f rows[3];
    };

    inline float &operator[](int a)
    {
        return data[a];
    }
};

struct m4x4f
{
    union
    {
        float data[16];
        struct
        {
            v4f r0;
            v4f r1;
            v4f r2;
            v4f r3;
        };
        v4f rows[4];
    };

    inline float &operator[](int a)
    {
        return data[a];
    }
};

struct m4x5f
{
    union
    {
        float data[20];
        struct
        {
            v5f r0;
            v5f r1;
            v5f r2;
            v5f r3;
        };
        v5f rows[4];
    };

    inline float & operator[](int a)
    {
        return data[a];
    }
};
#pragma pack(pop)

/* Start of 4x5 matrix Functions */
//TODO: handle 0s in unfortunate spots
v4f solve(m4x5f equations)
{
    /* printf("before\n"); */
    /* for(int r = 0; r < 4; r++) */
    /* { */
    /*     for(int c = 0; c < 5; c++) */
    /*     { */
    /*         printf("%f, ", equations.rows[r][c]); */
    /*     } */
    /*     printf("\n"); */
    /* } */
    /* printf("\n"); */
    
    float epsilon = 0.1f;
    float zero = 0.0001f;

    //Guass-Jordan algorithm
    for(int i = 0, j = 0; i < 4 && j < 5; i++, j++)
    {
        if(fabs(equations.rows[i][j]) < epsilon)
        {
            for(int r = i+1; r < 4; r++)
            {
                //TODO handle case where entire column is 0
                if(fabs(equations.rows[r][j]) >= epsilon)
                {
                    v5f temp = equations.rows[i];
                    equations.rows[i] = equations.rows[r];
                    equations.rows[r] = temp;
                }
            }
        }
        equations.rows[i] = equations.rows[i]*(1.0f/equations.rows[i][j]);
        for(int r = 0; r < 4; r++)
        {
            if(r == i) continue;
            do
            {
                equations.rows[r] = equations.rows[r] - equations.rows[i]*equations.rows[r][j];
            } while(fabs(equations.rows[r][j]) > zero); //for floating point error
            
        /*     for(int r = 0; r < 4; r++) */
        /*     { */
        /*         for(int c = 0; c < 5; c++) */
        /*         { */
        /*             printf("%f, ", equations.rows[r][c]); */
        /*         } */
        /*         printf("\n"); */
        /*     } */
        /*     printf("\n"); */
        }
    }

    /* printf("after\n"); */
    /* for(int r = 0; r < 4; r++) */
    /* { */
    /*     for(int c = 0; c < 5; c++) */
    /*     { */
    /*         printf("%f, ", equations.rows[r][c]); */
    /*     } */
    /*     printf("\n"); */
    /* } */
    /* printf("\n"); */
    
    v4f solutions;
    for(int i = 0; i < 4; i++)
    {
        solutions[i] = equations.rows[i][4];
        /* printf("solutions[%d] = %f = %f / %f\n", i, solutions[i], equations.rows[i][4], equations.rows[i][i]); */
    }
    return solutions;
}
/* End of 4x5 matrix Functions */

float bound(float bounded, float lower, float upper)
{
    if (bounded > upper)
        return upper;
    if (bounded < lower)
        return lower;
    return bounded;
}

float lerp(float a, float b, float t)
{
    if (t > 1.0f) return b;
    if (t < 0.0f) return a;
    return a + (b - a) * t;
}

bool isAngleGreaterDeg(float a, float b)//TODO: Better name
{
    //going to hope the imu knows that 360 = 0
    if((b-a) < 180)
        return a < b;
    else
        return a > (b-360);

}

float signedCanonicalizeAngleDeg(float a)
{
    return a-360.0f*floor(a/360.0f+0.5f); //NOTE: this can return both -180 and +180, the sign will be unchanged for these angles
}

bool tolerantEquals(float a, float b, float tolerance)
{
    return (a + tolerance) >= b && (a - tolerance) <= b;
}

float cubicBezier(float t, float p0, float p1, float p2, float p3)
{
    return (1-t)*(1-t)*(1-t)*p0 + 3*t*(1-t)*(1-t)*p1 + 3*t*t*(1-t)*p2 + t*t*t*p3;
}

float quadraticBezier(float t, float p0, float p1, float p2)
{
    return (1-t)*(1-t)*p0 + 2*t*(1-t)*p1 + t*t*p2;
}

#endif
