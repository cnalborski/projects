
typedef struct {
   F32 x,y,z;
} VERTEX;

typedef F32 MATRIX[4][4];

typedef struct {
   U32 SCN_Mat;
   U32 SCN_Obj;
   U32 SCN_Vertex;
   U32 SCN_Face;
} SCENE;

typedef struct {
   U32 OBJ_Vertex;
   U32 OBJ_Face;
   U32 OBJ_Type;
   U32 OBJ_Fill;
} OBJECT;