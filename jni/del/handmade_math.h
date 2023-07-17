#ifndef _HANDMADE_MATH_H_
#define _HANDMADE_MATH_H_


typedef struct {
    real32 X;
    real32 Y;
} v2;


internal v2
V2(real32 X, real32 Y) {
    v2 result = {X, Y};
    return result;
}


v2
operator- (v2 A, v2 B) 
{
    v2 Result = {};
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return Result;
}

v2
operator-= (v2 A, v2 B) 
{
    v2 Result = A - B;
    return Result;
}

v2
operator+ (v2 A, v2 B) 
{
    v2 Result = {};
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return Result;
}

inline v2 &
operator+= (v2 &A, v2 B) 
{
    A = A + B;
    return A;
}

v2
operator* (v2 A, real32 Value) 
{
    v2 Result = {};
    Result.X = A.X * Value;
    Result.Y = A.Y * Value;
    return Result;
}

v2
operator* (real32 Value, v2 A) 
{
    v2 Result = {};
    Result.X = A.X * Value;
    Result.Y = A.Y * Value;
    return Result;
}

v2
operator*= (v2 &A, real32 Value) 
{
    A = A * Value;
    return A;
}




real32
Square(real32 A) {
    real32 Result = A * A;
    return Result;
}


real32
Inner(v2 A, v2 B) {
    real32 Result = A.X*B.X  + A.Y*B.Y;
    return Result;
}

inline real32
LengthSq(v2 A)
{
    real32 Result = Inner(A, A);

    return(Result);
}


#endif
