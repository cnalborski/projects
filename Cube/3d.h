#define SCN_MAX_MAT 32
#define SCN_MAX_OBJ 128
#define SCN_MAX_VERTEX 6000
#define SCN_MAX_FACE 6000

#define ns 300.0

typedef struct {
   short ID;
   U32 LEN;
} HEADER;

typedef struct {
   F32 x;
   F32 y;
   F32 z;
} VERTEX;

typedef struct {
   F32 x;
   F32 y;
   F32 z;
} VERTEX_2;

typedef struct {
   F32 u;
   F32 v;
} MAPUV;

typedef struct {
   U32 u;
   U32 v;
} MAPUV_2;

typedef struct {
   U16 v1;
   U16 v2;
   U16 v3;
   U16 Flags;
} FACE3DS;

typedef struct {
   U32 v1;
   U32 v2;
   U32 v3;
   U32 v4;
} FACE;

typedef struct {
   U32 OBJ_Vertex;
   U32 OBJ_Face;
   U32 OBJ_Type;
   U32 OBJ_Fill;
} OBJECT;

typedef struct {
   char MatName[17];
   U16 Type;
} MATERIAL;

typedef struct {
   char MatFile[13];
} TEXTURE;

typedef struct {
   F32 a,b,c;
   F32 d,e,f;
   F32 g,h,i;
   F32 x,y,z;
} MTX3DS;

typedef F32 MTX[4][3];