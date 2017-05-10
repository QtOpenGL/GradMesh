#ifndef ENUMS
#define ENUMS

enum SELECTED_POINT_TYPE{
    CONTROL,
    GRADIENT,
    NONE
};

enum SELECT_TYPE {
    POINTS,
    EDGES,
    FACES,
    GRADS
};

enum REFINERS{
    TERNARY,
    CATMULLCLARK
};

enum class DISPLAY{
    ORIGINAL = 0,
    MESHREF = 1,
    CTRLVECTOR = 2,
    NUMDISPLAYS = 3
};

#endif // ENUMS

